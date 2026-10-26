#include "sample_processing.h"

#include <stdint.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 0; \
} } while (0)
#define RUN(test) do { ++runs; if (!(test())) { ++failures; } } while (0)

static unsigned int runs;
static unsigned int failures;

static int test_invalid_configuration(void)
{
    SampleProcessor p;
    CHECK(sample_processor_init(NULL, 1500U, 2500U) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_init(&p, 1500U, 1500U) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_init(&p, 2500U, 1500U) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_init(&p, 0U, 4096U) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_init(&p, 0U, SAMPLE_ADC_MAX) == SAMPLE_OK);
    return 1;
}

static int test_unknown_and_inclusive_edges(void)
{
    SampleProcessor p;
    SampleInput input = {1U, 2000U};
    SampleOutput output = {0U, 0U, SAMPLE_LEVEL_LOW};
    CHECK(sample_processor_init(&p, 1500U, 2500U) == SAMPLE_OK);
    CHECK(p.level == SAMPLE_LEVEL_UNKNOWN);
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_UNKNOWN && output.sequence == 1U);
    input.sequence = 2U;
    input.value = 2500U;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_HIGH);
    input.sequence = 3U;
    input.value = 1501U;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_HIGH);
    input.sequence = 4U;
    input.value = 1500U;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_LOW && p.next_sequence == 5U);
    return 1;
}

static int test_invalid_value_consumes_order_only(void)
{
    SampleProcessor p;
    SampleInput input = {1U, 4096U};
    SampleOutput output = {77U, 88U, SAMPLE_LEVEL_HIGH};
    CHECK(sample_processor_init(&p, 1500U, 2500U) == SAMPLE_OK);
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_BAD_VALUE);
    CHECK(p.next_sequence == 2U && p.level == SAMPLE_LEVEL_UNKNOWN);
    CHECK(output.sequence == 77U && output.value == 88U);
    input.sequence = 2U;
    input.value = 1000U;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.sequence == 2U && output.level == SAMPLE_LEVEL_LOW);
    return 1;
}

static int test_sequence_failure_does_not_advance(void)
{
    SampleProcessor p;
    SampleInput input = {2U, 2500U};
    SampleOutput output = {0U, 0U, SAMPLE_LEVEL_UNKNOWN};
    CHECK(sample_processor_init(&p, 1500U, 2500U) == SAMPLE_OK);
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_BAD_SEQUENCE);
    CHECK(p.next_sequence == 1U && p.level == SAMPLE_LEVEL_UNKNOWN);
    input.sequence = 1U;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.sequence == 1U && output.level == SAMPLE_LEVEL_HIGH);
    return 1;
}

static int test_sequence_overflow_guard(void)
{
    SampleProcessor p;
    SampleInput input = {UINT32_MAX, 1000U};
    SampleOutput output = {0U, 0U, SAMPLE_LEVEL_UNKNOWN};
    CHECK(sample_processor_init(&p, 1500U, 2500U) == SAMPLE_OK);
    p.next_sequence = UINT32_MAX;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_BAD_SEQUENCE);
    CHECK(p.next_sequence == UINT32_MAX && p.level == SAMPLE_LEVEL_UNKNOWN);
    return 1;
}

static int test_null_arguments(void)
{
    SampleProcessor p;
    SampleInput input = {1U, 1000U};
    SampleOutput output;
    CHECK(sample_processor_init(&p, 1500U, 2500U) == SAMPLE_OK);
    CHECK(sample_processor_accept(NULL, &input, &output) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_accept(&p, NULL, &output) == SAMPLE_BAD_ARGUMENT);
    CHECK(sample_processor_accept(&p, &input, NULL) == SAMPLE_BAD_ARGUMENT);
    CHECK(p.next_sequence == 1U);
    return 1;
}

static int test_adc_extremes(void)
{
    SampleProcessor p;
    SampleInput input = {1U, 0U};
    SampleOutput output;
    CHECK(sample_processor_init(&p, 0U, SAMPLE_ADC_MAX) == SAMPLE_OK);
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_LOW);
    input.sequence = 2U;
    input.value = SAMPLE_ADC_MAX;
    CHECK(sample_processor_accept(&p, &input, &output) == SAMPLE_OK);
    CHECK(output.level == SAMPLE_LEVEL_HIGH);
    return 1;
}

int main(void)
{
    RUN(test_invalid_configuration);
    RUN(test_unknown_and_inclusive_edges);
    RUN(test_invalid_value_consumes_order_only);
    RUN(test_sequence_failure_does_not_advance);
    RUN(test_sequence_overflow_guard);
    RUN(test_null_arguments);
    RUN(test_adc_extremes);
    printf("sample_processing: %u tests, %u failures\n", runs, failures);
    return failures == 0U ? 0 : 1;
}
