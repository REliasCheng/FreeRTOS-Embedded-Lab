#include "sample_processing.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAMPLE_QUEUE_LENGTH 4U
#define RESULT_CAPACITY 4U
#define TASK_STACK_WORDS 1024U
#define IPC_WAIT_TICKS pdMS_TO_TICKS(100U)
#define REPORT_WAIT_TICKS pdMS_TO_TICKS(1000U)

typedef enum {
    SCENARIO_NORMAL,
    SCENARIO_BACKPRESSURE,
    SCENARIO_TIMEOUT
} Scenario;

typedef struct {
    unsigned int sent;
    unsigned int received;
    unsigned int invalid;
    unsigned int backpressure;
    unsigned int receive_timeouts;
    unsigned int result_count;
    SampleOutput results[RESULT_CAPACITY];
} Statistics;

typedef struct {
    Scenario scenario;
    QueueHandle_t queue;
    SemaphoreHandle_t mutex;
    SemaphoreHandle_t start_gate;
    TaskHandle_t reporter;
    TaskHandle_t producer;
    TaskHandle_t consumer;
    Statistics statistics;
    atomic_int task_error;
    int mutex_failure_verified;
    int notification_timeout_verified;
    int completed;
    int exit_code;
} Runtime;

static Runtime runtime;

static int record_sent(void)
{
    if (xSemaphoreTake(runtime.mutex, IPC_WAIT_TICKS) != pdTRUE) {
        return 0;
    }
    ++runtime.statistics.sent;
    configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
    return 1;
}

static int record_received(SampleStatus status, const SampleOutput *output,
                           TickType_t wait_ticks)
{
    if (xSemaphoreTake(runtime.mutex, wait_ticks) != pdTRUE) {
        return 0;
    }
    if (status == SAMPLE_BAD_VALUE) {
        ++runtime.statistics.invalid;
    } else if (status == SAMPLE_OK && output != NULL &&
               runtime.statistics.result_count < RESULT_CAPACITY) {
        runtime.statistics.results[runtime.statistics.result_count++] = *output;
    } else {
        configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
        return 0;
    }
    ++runtime.statistics.received;
    configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
    return 1;
}

static int record_receive_timeout(void)
{
    if (xSemaphoreTake(runtime.mutex, IPC_WAIT_TICKS) != pdTRUE) {
        return 0;
    }
    ++runtime.statistics.receive_timeouts;
    configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
    return 1;
}

static void producer_task(void *argument)
{
    static const uint16_t normal_values[] = {2600U, 2000U, 1400U};
    SampleInput input;
    unsigned int i;
    (void)argument;
    if (xSemaphoreTake(runtime.start_gate, IPC_WAIT_TICKS) != pdTRUE) {
        atomic_store(&runtime.task_error, 6);
        xTaskNotifyGive(runtime.reporter);
        vTaskDelete(NULL);
    }
    if (runtime.scenario == SCENARIO_TIMEOUT) {
        vTaskDelay(pdMS_TO_TICKS(30U));
    } else if (runtime.scenario == SCENARIO_NORMAL) {
        for (i = 0U; i < 3U; ++i) {
            input.sequence = i + 1U;
            input.value = normal_values[i];
            if (xQueueSend(runtime.queue, &input, IPC_WAIT_TICKS) != pdTRUE ||
                !record_sent()) {
                atomic_store(&runtime.task_error, 1);
                break;
            }
        }
    }
    if (runtime.scenario != SCENARIO_TIMEOUT) {
        input.sequence = 0U; /* In-band end marker, never counted as a sample. */
        input.value = 0U;
        if (xQueueSend(runtime.queue, &input, IPC_WAIT_TICKS) != pdTRUE) {
            atomic_store(&runtime.task_error, 1);
        }
    }
    xTaskNotifyGive(runtime.reporter);
    vTaskDelete(NULL);
}

static void consumer_task(void *argument)
{
    SampleProcessor processor;
    SampleInput input;
    SampleOutput output;
    SampleStatus status;
    (void)argument;
    if (xSemaphoreTake(runtime.start_gate, IPC_WAIT_TICKS) != pdTRUE) {
        atomic_store(&runtime.task_error, 7);
        xTaskNotifyGive(runtime.reporter);
        vTaskDelete(NULL);
    }
    if (sample_processor_init(&processor, 1500U, 2500U) != SAMPLE_OK) {
        atomic_store(&runtime.task_error, 2);
    } else {
        for (;;) {
            if (xQueueReceive(runtime.queue, &input,
                              runtime.scenario == SCENARIO_TIMEOUT ?
                                  pdMS_TO_TICKS(20U) : IPC_WAIT_TICKS) != pdTRUE) {
                if (!record_receive_timeout() ||
                    runtime.scenario != SCENARIO_TIMEOUT) {
                    atomic_store(&runtime.task_error, 3);
                }
                break;
            }
            if (input.sequence == 0U) {
                break;
            }
            status = sample_processor_accept(&processor, &input, &output);
            if (status != SAMPLE_OK && status != SAMPLE_BAD_VALUE) {
                atomic_store(&runtime.task_error, 4);
                break;
            }
            if (!record_received(status, status == SAMPLE_OK ? &output : NULL,
                                 IPC_WAIT_TICKS)) {
                atomic_store(&runtime.task_error, 5);
                break;
            }
            if (status == SAMPLE_OK) {
                xTaskNotifyGive(runtime.reporter);
            }
        }
    }
    xTaskNotifyGive(runtime.reporter);
    vTaskDelete(NULL);
}

static int result_is(const Statistics *s, unsigned int index,
                     uint32_t sequence, uint16_t value, SampleLevel level)
{
    return index < s->result_count &&
           s->results[index].sequence == sequence &&
           s->results[index].value == value &&
           s->results[index].level == level;
}

static int verify_statistics(const Statistics *s)
{
    if (runtime.scenario == SCENARIO_NORMAL) {
        return s->sent == 3U && s->received == 3U &&
               s->invalid == 0U && s->backpressure == 0U &&
               s->receive_timeouts == 0U && s->result_count == 3U &&
               result_is(s, 0U, 1U, 2600U, SAMPLE_LEVEL_HIGH) &&
               result_is(s, 1U, 2U, 2000U, SAMPLE_LEVEL_HIGH) &&
               result_is(s, 2U, 3U, 1400U, SAMPLE_LEVEL_LOW);
    }
    if (runtime.scenario == SCENARIO_BACKPRESSURE) {
        return s->sent == 4U && s->received == 4U &&
               s->invalid == 1U && s->backpressure == 1U &&
               s->receive_timeouts == 0U && s->result_count == 3U &&
               result_is(s, 0U, 1U, 1000U, SAMPLE_LEVEL_LOW) &&
               result_is(s, 1U, 2U, 2500U, SAMPLE_LEVEL_HIGH) &&
               result_is(s, 2U, 4U, 1500U, SAMPLE_LEVEL_LOW);
    }
    return s->sent == 0U && s->received == 0U &&
           s->invalid == 0U && s->backpressure == 0U &&
           s->receive_timeouts == 1U && s->result_count == 0U;
}

static const char *scenario_name(void)
{
    if (runtime.scenario == SCENARIO_NORMAL) {
        return "normal";
    }
    if (runtime.scenario == SCENARIO_BACKPRESSURE) {
        return "backpressure";
    }
    return "timeout";
}

static void reporter_task(void *argument)
{
    Statistics snapshot;
    SampleOutput dummy = {0U, 0U, SAMPLE_LEVEL_UNKNOWN};
    unsigned int expected_notifications =
        runtime.scenario == SCENARIO_TIMEOUT ? 2U : 5U;
    unsigned int notifications = 0U;
    unsigned int task_count = 0U;
    unsigned int attempts;
    uint32_t taken;
    int model_ok;
    int task_error;
    int ok = 1;
    (void)argument;

    if (ulTaskNotifyTake(pdTRUE, 0U) != 0U ||
        xSemaphoreTake(runtime.mutex, 0U) != pdTRUE) {
        ok = 0;
    } else {
        unsigned int previous = runtime.statistics.received;
        if (record_received(SAMPLE_OK, &dummy, 0U) != 0 ||
            runtime.statistics.received != previous) {
            ok = 0;
        } else {
            runtime.mutex_failure_verified = 1;
        }
        configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
    }
    if (ok) {
        if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(3U)) != 0U) {
            ok = 0;
        } else {
            runtime.notification_timeout_verified = 1;
        }
        if (xSemaphoreGive(runtime.start_gate) != pdTRUE ||
            xSemaphoreGive(runtime.start_gate) != pdTRUE) {
            ok = 0;
        }
    }
    while (ok && notifications < expected_notifications) {
        taken = ulTaskNotifyTake(pdTRUE, REPORT_WAIT_TICKS);
        if (taken == 0U ||
            taken > expected_notifications - notifications) {
            ok = 0;
            break;
        }
        notifications += taken;
    }
    /* Let both completed worker tasks execute vTaskDelete(NULL). */
    if (ok) {
        for (attempts = 0U; attempts < 50U; ++attempts) {
            task_count = uxTaskGetNumberOfTasks();
            if (task_count == 2U) { /* Reporter plus Idle. */
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1U));
        }
        ok = task_count == 2U;
    }
    if (xSemaphoreTake(runtime.mutex, IPC_WAIT_TICKS) != pdTRUE) {
        ok = 0;
        memset(&snapshot, 0, sizeof snapshot);
    } else {
        snapshot = runtime.statistics;
        configASSERT(xSemaphoreGive(runtime.mutex) == pdTRUE);
    }
    task_error = atomic_load(&runtime.task_error);
    model_ok = verify_statistics(&snapshot);
    if (task_error != 0 ||
        notifications != expected_notifications ||
        runtime.mutex_failure_verified != 1 ||
        runtime.notification_timeout_verified != 1 ||
        !model_ok) {
        ok = 0;
    }
    runtime.exit_code = ok ? 0 : 1;
    runtime.completed = 1;
    printf("scenario=%s kernel_tasks=3 sent=%u received=%u results=%u invalid=%u "
           "backpressure=%u receive_timeouts=%u notifications=%u mutex_denial=%d "
           "notify_timeout=%d tasks_remaining=%u task_error=%d model_ok=%d result=%s\n",
           scenario_name(), snapshot.sent, snapshot.received,
           snapshot.result_count, snapshot.invalid, snapshot.backpressure,
           snapshot.receive_timeouts, notifications,
           runtime.mutex_failure_verified,
           runtime.notification_timeout_verified, task_count, task_error, model_ok,
           ok ? "PASS" : "FAIL");
    fflush(stdout);
    if (ok) {
        vQueueDelete(runtime.queue);
        vSemaphoreDelete(runtime.mutex);
        vSemaphoreDelete(runtime.start_gate);
        runtime.queue = NULL;
        runtime.mutex = NULL;
        runtime.start_gate = NULL;
    }
    /* The official Host ports use operating-system threads. End this one-shot
     * process after both workers have deleted themselves. */
    exit(runtime.exit_code);
}

static int preload_backpressure(void)
{
    static const uint16_t values[] = {1000U, 2500U, 4096U, 1500U};
    SampleInput input;
    unsigned int i;
    for (i = 0U; i < SAMPLE_QUEUE_LENGTH; ++i) {
        input.sequence = i + 1U;
        input.value = values[i];
        if (xQueueSend(runtime.queue, &input, 0U) != pdTRUE) {
            return 0;
        }
        ++runtime.statistics.sent;
    }
    input.sequence = 5U;
    input.value = 3000U;
    if (xQueueSend(runtime.queue, &input, 0U) != errQUEUE_FULL) {
        return 0;
    }
    ++runtime.statistics.backpressure;
    return 1;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: rtos_demo normal|backpressure|timeout\n");
        return 2;
    }
    if (strcmp(argv[1], "normal") == 0) {
        runtime.scenario = SCENARIO_NORMAL;
    } else if (strcmp(argv[1], "backpressure") == 0) {
        runtime.scenario = SCENARIO_BACKPRESSURE;
    } else if (strcmp(argv[1], "timeout") == 0) {
        runtime.scenario = SCENARIO_TIMEOUT;
    } else {
        return 2;
    }
    atomic_init(&runtime.task_error, 0);
    runtime.queue = xQueueCreate(SAMPLE_QUEUE_LENGTH, sizeof(SampleInput));
    runtime.mutex = xSemaphoreCreateMutex();
    runtime.start_gate = xSemaphoreCreateCounting(2U, 0U);
    if (runtime.queue == NULL || runtime.mutex == NULL ||
        runtime.start_gate == NULL ||
        (runtime.scenario == SCENARIO_BACKPRESSURE &&
         !preload_backpressure()) ||
        xTaskCreate(reporter_task, "reporter", TASK_STACK_WORDS, NULL, 3U,
                    &runtime.reporter) != pdPASS ||
        xTaskCreate(consumer_task, "consumer", TASK_STACK_WORDS, NULL, 2U,
                    &runtime.consumer) != pdPASS ||
        xTaskCreate(producer_task, "producer", TASK_STACK_WORDS, NULL, 1U,
                    &runtime.producer) != pdPASS) {
        fprintf(stderr, "RTOS resource or task creation failed\n");
        if (runtime.producer != NULL) {
            vTaskDelete(runtime.producer);
        }
        if (runtime.consumer != NULL) {
            vTaskDelete(runtime.consumer);
        }
        if (runtime.reporter != NULL) {
            vTaskDelete(runtime.reporter);
        }
        if (runtime.mutex != NULL) {
            vSemaphoreDelete(runtime.mutex);
        }
        if (runtime.start_gate != NULL) {
            vSemaphoreDelete(runtime.start_gate);
        }
        if (runtime.queue != NULL) {
            vQueueDelete(runtime.queue);
        }
        return 1;
    }
    vTaskStartScheduler();
    if (!runtime.completed) {
        fprintf(stderr, "FreeRTOS scheduler did not complete the scenario\n");
        return 1;
    }
    return runtime.exit_code;
}

void vApplicationMallocFailedHook(void)
{
    fputs("FreeRTOS heap allocation failed\n", stderr);
    abort();
}

void vApplicationStackOverflowHook(TaskHandle_t task, char *name)
{
    (void)task;
    fprintf(stderr, "FreeRTOS stack overflow: %s\n", name);
    abort();
}
