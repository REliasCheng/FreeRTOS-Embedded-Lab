#include "sample_processing.h"

#include <limits.h>
#include <stddef.h>

SampleStatus sample_processor_init(SampleProcessor *processor,
                                   uint16_t low_threshold,
                                   uint16_t high_threshold)
{
    if (processor == NULL || low_threshold >= high_threshold ||
        high_threshold > SAMPLE_ADC_MAX) {
        return SAMPLE_BAD_ARGUMENT;
    }
    processor->next_sequence = 1U;
    processor->low_threshold = low_threshold;
    processor->high_threshold = high_threshold;
    processor->level = SAMPLE_LEVEL_UNKNOWN;
    return SAMPLE_OK;
}

SampleStatus sample_processor_accept(SampleProcessor *processor,
                                     const SampleInput *input,
                                     SampleOutput *output)
{
    SampleLevel next_level;
    if (processor == NULL || input == NULL || output == NULL) {
        return SAMPLE_BAD_ARGUMENT;
    }
    if (processor->next_sequence == UINT32_MAX ||
        input->sequence != processor->next_sequence) {
        return SAMPLE_BAD_SEQUENCE;
    }
    ++processor->next_sequence;
    if (input->value > SAMPLE_ADC_MAX) {
        return SAMPLE_BAD_VALUE;
    }
    next_level = processor->level;
    if (input->value >= processor->high_threshold) {
        next_level = SAMPLE_LEVEL_HIGH;
    } else if (input->value <= processor->low_threshold) {
        next_level = SAMPLE_LEVEL_LOW;
    }
    output->sequence = input->sequence;
    output->value = input->value;
    output->level = next_level;
    processor->level = next_level;
    return SAMPLE_OK;
}
