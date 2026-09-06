#include <stdio.h>
#include "hysteresis_moving_average.h"

struct app_avg_state init_moving_average() {
    struct app_avg_state new_state = {
        .samples = {},
        .current_element = 0,
        .lower_threshold = LOWER_THRESHOLD,
        .upper_threshold = UPPER_THRESHOLD,
        .current_state = DEFAULT_STATE,
    };
    return new_state;
}

void update_moving_average(struct app_avg_state *current_state, int new_sample) {
    current_state->samples[current_state->current_element] = new_sample;
    current_state->current_element = (current_state->current_element + 1) % WINDOW_SIZE;
    float element_sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        element_sum += current_state->samples[i];
    }
    float avg = element_sum / WINDOW_SIZE;
    if (current_state->current_state == false) {
        if (avg > current_state->upper_threshold) {
            printf("HMA: switching state to %d (avg: %f)\n", true, avg);
            current_state->current_state = true;
        }
    } else {
        if (avg < current_state->lower_threshold) {
            printf("HMA: switching state to %d (avg: %f)\n", false, avg);
            current_state->current_state = false;
        }
    }
}