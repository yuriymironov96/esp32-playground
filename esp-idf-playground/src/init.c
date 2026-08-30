#include <conf.h>
#include <stdbool.h>

static void init_button(gpio_num_t pin, bool use_interrupt)
{
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_DEF_INPUT,
        .pin_bit_mask = (1ULL << pin),
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE
    };
    if (use_interrupt) {
        io_conf.intr_type = GPIO_INTR_NEGEDGE;
    }
    gpio_config(&io_conf);
}

static void init_output(gpio_num_t pin)
{
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
}

void init_gpio()
{
    init_button(BUTTON_1_PIN, true);
    init_button(BUTTON_2_PIN, true);
    init_button(BUTTON_3_PIN, true);
    init_button(BUTTON_4_PIN, false);
    gpio_install_isr_service(0);

    for (int i = 0; i < MONITOR_PINS_SIZE; i++)
    {
        init_output(MONITOR_PINS[i]);
    }
}
