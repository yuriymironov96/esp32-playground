#include <Arduino.h>
#include <Preferences.h>
#include <esp_timer.h>
#include <esp_task_wdt.h>

constexpr uint8_t FAN_PIN = 4;
constexpr uint8_t LED_PIN = 14;
constexpr bool FAN_ACTIVE_LOW = true;

constexpr uint64_t SECOND_US = 1000000ULL;

constexpr uint64_t PERIOD_US = 6 * SECOND_US;
constexpr uint64_t ON_TIME_US = 3 * SECOND_US;


constexpr uint64_t WDT_CHECK_US = SECOND_US;
constexpr uint64_t WDT_GRACE_US = 2 * SECOND_US;

Preferences prefs;

esp_timer_handle_t cycleTimer;
esp_timer_handle_t offTimer;
esp_timer_handle_t watchdogTimer;

uint64_t nextTransitionUs;
bool watchdogAdded = false;


void setFan(bool enabled)
{
    // set relay value to trigger fan
        digitalWrite(
        FAN_PIN,
        FAN_ACTIVE_LOW ? !enabled : enabled
    );

    // set status led value
    digitalWrite(LED_PIN, enabled);

    // remember current state in persistance 
    // in case of reboot
    prefs.putBool("fan", enabled);

    Serial.printf(
        "[%llu ms] Fan %s\n",
        esp_timer_get_time() / 1000,
        enabled ? "ON" : "OFF"
    );
}


void turnOff(void *)
{
    setFan(false);

    nextTransitionUs =
        esp_timer_get_time() +
        PERIOD_US -
        ON_TIME_US;
}


void startCycle(void *)
{
    setFan(true);

    nextTransitionUs =
        esp_timer_get_time() + ON_TIME_US;

    ESP_ERROR_CHECK(
        esp_timer_start_once(
            offTimer,
            ON_TIME_US
        )
    );
}


void checkWatchdog(void *)
{
    // register watchdog only once,
    // set current task as a watchdog
    if (!watchdogAdded)
    {
        ESP_ERROR_CHECK(
            esp_task_wdt_add(NULL)
        );

        watchdogAdded = true;
    }

    // feed watchdog only while fan timing is healthy
    if (esp_timer_get_time() <=
        nextTransitionUs + WDT_GRACE_US)
    {
        ESP_ERROR_CHECK(
            esp_task_wdt_reset()
        );
    }
}


void setup()
{
    Serial.begin(9600);

    pinMode(FAN_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    prefs.begin("fan", false);


    esp_timer_create_args_t args{};

    // create timers, swallow any erros
    // if we find those using ESP_ERROR_CHECK
    args.callback = startCycle;
    ESP_ERROR_CHECK(
        esp_timer_create(&args, &cycleTimer)
    );
    args.callback = turnOff;
    ESP_ERROR_CHECK(
        esp_timer_create(&args, &offTimer)
    );

    // create a watchdog
    args.callback = checkWatchdog;
    ESP_ERROR_CHECK(
        esp_timer_create(&args, &watchdogTimer)
    );


    // remember persisted state, falling back to true if no state was recoded
    bool wasOn =
        prefs.getBool("fan", true);
    setFan(wasOn);


    if (wasOn)
    {
        nextTransitionUs =
            esp_timer_get_time() + ON_TIME_US;

        ESP_ERROR_CHECK(
            esp_timer_start_once(
                offTimer,
                ON_TIME_US
            )
        );
    }
    else
    {
        nextTransitionUs =
            esp_timer_get_time() + PERIOD_US;
    }


    // start recurring timers, the general fan cycle and a watchdog
    ESP_ERROR_CHECK(
        esp_timer_start_periodic(
            cycleTimer,
            PERIOD_US
        )
    );
    ESP_ERROR_CHECK(
        esp_timer_start_periodic(
            watchdogTimer,
            WDT_CHECK_US
        )
    );
}


void loop()
{
}