#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdlib.h>

/* Host simulation only. These values are not a Cortex-M board configuration. */
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configCPU_CLOCK_HZ                      1000000UL
#define configTICK_RATE_HZ                      1000U
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                256U
#define configMAX_TASK_NAME_LEN                 16
#define configTOTAL_HEAP_SIZE                   ( 64U * 1024U )
#define configUSE_16_BIT_TICKS                  0
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0
#define configUSE_MUTEXES                       1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_TASK_NOTIFICATIONS            1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES   1
#define configUSE_TIMERS                        0
#define configQUEUE_REGISTRY_SIZE               0

#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskEndScheduler               1

#define configASSERT( condition ) do { if( !( condition ) ) { abort(); } } while( 0 )

#endif
