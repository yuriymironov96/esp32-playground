#include <stdbool.h>
#define WINDOW_SIZE 10

#define LOWER_THRESHOLD 2000
#define UPPER_THRESHOLD 2500
#define DEFAULT_STATE false

struct app_avg_state {
    int samples[WINDOW_SIZE];
    int current_element;
    int lower_threshold;
    int upper_threshold;
    bool current_state;
};

struct app_avg_state init_moving_average();

void update_moving_average(struct app_avg_state *current_state, int new_sample);

