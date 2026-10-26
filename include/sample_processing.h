#ifndef SAMPLE_PROCESSING_H
#define SAMPLE_PROCESSING_H

#include <stdint.h>

#define SAMPLE_ADC_MAX UINT16_C(4095)

typedef enum {
    SAMPLE_LEVEL_UNKNOWN = 0,
    SAMPLE_LEVEL_LOW,
    SAMPLE_LEVEL_HIGH
} SampleLevel;

typedef struct {
    uint32_t sequence;
    uint16_t value;
} SampleInput;

typedef struct {
    uint32_t sequence;
    uint16_t value;
    SampleLevel level;
} SampleOutput;

typedef struct {
    uint32_t next_sequence;
    uint16_t low_threshold;
    uint16_t high_threshold;
    SampleLevel level;
} SampleProcessor;

typedef enum {
    SAMPLE_OK = 0,
    SAMPLE_BAD_ARGUMENT,
    SAMPLE_BAD_SEQUENCE,
    SAMPLE_BAD_VALUE
} SampleStatus;

SampleStatus sample_processor_init(SampleProcessor *processor,
                                   uint16_t low_threshold,
                                   uint16_t high_threshold);
/* Invalid values consume their ordered sequence but do not change the level. */
SampleStatus sample_processor_accept(SampleProcessor *processor,
                                     const SampleInput *input,
                                     SampleOutput *output);

#endif
