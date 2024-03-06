
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"


#define SELECT_SWITCH 13                                   // Change this to the GPIO pin connected to your switch
#define CONFIRM_SWITCH 4   

/************Led***********************/

#define TEA_LED 26              // GPIO pin for Tea LED
#define COFFEE_LED 25           // GPIO pin for Coffee LED
#define MILK_LED 14             // GPIO pin for Milk LED
#define BOOST_LED 12            // GPIO pin for Boost LED


#define DEVICE_LED 2            // GPIO pin for DEVICE_INDICATOR
#define DEVICE_LED_DELAY 200    // DEVICE_LED_DELAY 


/************count and Interval***********************/

#define TEA_BLINK_COUNT 5
#define TEA_BLINK_INTERVAL 1000

#define COFFEE_BLINK_COUNT 10
#define COFFEE_BLINK_INTERVAL 750

#define MILK_BLINK_COUNT 15
#define MILK_BLINK_INTERVAL 500

#define BOOST_BLINK_COUNT 20
#define BOOST_BLINK_INTERVAL 250


TaskHandle_t indicator_led_task_Handle, machine_function_Handle;

static void configure_led(void)
{
    ESP_LOGI("MACHINE :","Example configured to blink GPIO LED!");
    gpio_reset_pin(TEA_LED);
    gpio_set_direction(TEA_LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(COFFEE_LED);
    gpio_set_direction(COFFEE_LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MILK_LED);
    gpio_set_direction(MILK_LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(BOOST_LED);
    gpio_set_direction(BOOST_LED, GPIO_MODE_OUTPUT);

    gpio_reset_pin(DEVICE_LED);
    gpio_set_direction(DEVICE_LED, GPIO_MODE_OUTPUT);
}

static void configure_switch(void)
{
    ESP_LOGI("MACHINE:", "Example configured to read switch state!");
    gpio_reset_pin(SELECT_SWITCH);
    gpio_set_direction(SELECT_SWITCH, GPIO_MODE_INPUT);
    gpio_set_pull_mode(SELECT_SWITCH, GPIO_PULLUP_ONLY);

    gpio_reset_pin(CONFIRM_SWITCH);
    gpio_set_direction(CONFIRM_SWITCH, GPIO_MODE_INPUT);
    gpio_set_pull_mode(CONFIRM_SWITCH, GPIO_PULLUP_ONLY);
}

void machine_function_indicator(int led_gpio, int blink_count, int blink_interval)
{
    for (int i = 0; i < blink_count; i++)
    {
        gpio_set_level(led_gpio, 1);
        vTaskDelay(blink_interval / 2 / portTICK_PERIOD_MS);
        gpio_set_level(led_gpio, 0);
        vTaskDelay(blink_interval / 2 / portTICK_PERIOD_MS);
    }
}

static void indicator_led_task(void *pvParameter)
{
    while (1)
    {
        gpio_set_level(DEVICE_LED, 0);
        vTaskDelay(DEVICE_LED_DELAY / portTICK_PERIOD_MS);
        gpio_set_level(DEVICE_LED, 1);
        vTaskDelay(DEVICE_LED_DELAY / portTICK_PERIOD_MS);
    }
}

static void machine_function(void *pvParameter)
{
        uint8_t count = 0;

        bool tea_led_state = 0;
        bool coffee_led_state = 0;
        bool milk_led_state = 0;
        bool boost_led_state = 0;


        while(1)
        {
                int switch_state = gpio_get_level(SELECT_SWITCH);
                // ESP_LOGI("BUTTON:", "Select Switch state: %d", switch_state);
                if (switch_state == 0) 
                {
                    while(gpio_get_level(SELECT_SWITCH) == 0);
                    count++;
                    ESP_LOGI("BUTTON:", "Switch state: %d", switch_state);
                    switch(count)
                    {
                        case 1:
                            gpio_set_level(TEA_LED, 1);
                            gpio_set_level(COFFEE_LED, 0);
                            gpio_set_level(MILK_LED, 0);
                            gpio_set_level(BOOST_LED, 0);

                            tea_led_state = 1;
                            coffee_led_state = 0;
                            milk_led_state = 0;
                            boost_led_state = 0;

                            break;
                        case 2:
                            gpio_set_level(TEA_LED, 0);
                            gpio_set_level(COFFEE_LED, 1);
                            gpio_set_level(MILK_LED, 0);
                            gpio_set_level(BOOST_LED, 0);


                            tea_led_state = 0;
                            coffee_led_state = 1;
                            milk_led_state = 0;
                            boost_led_state = 0;

                            break;
                        case 3:
                            gpio_set_level(TEA_LED, 0);
                            gpio_set_level(COFFEE_LED, 0);
                            gpio_set_level(MILK_LED, 1);
                            gpio_set_level(BOOST_LED, 0);


                            tea_led_state = 0;
                            coffee_led_state = 0;
                            milk_led_state = 1;
                            boost_led_state = 0;

                            break;
                        case 4:
                            gpio_set_level(TEA_LED, 0);
                            gpio_set_level(COFFEE_LED, 0);
                            gpio_set_level(MILK_LED, 0);
                            gpio_set_level(BOOST_LED, 1);


                            tea_led_state = 0;
                            coffee_led_state = 0;
                            milk_led_state = 0;
                            boost_led_state = 1;

                            break;
                        case 5:
                            count = 0;
                            break;

                    }

                }

                int c_switch_state = gpio_get_level(CONFIRM_SWITCH);
                if (c_switch_state == 0) 
                {

                    // ESP_LOGI("BUTTON:", "CONFIRM_SWITCH_state: %d", c_switch_state);
                    while(gpio_get_level(CONFIRM_SWITCH) == 0); // Wait for the button to be released

                    if (tea_led_state == 1)
                    {
                        ESP_LOGI("Function:", "TEA executed");
                        machine_function_indicator(TEA_LED, TEA_BLINK_COUNT, TEA_BLINK_INTERVAL);
                    }
                    else if (coffee_led_state == 1)
                    {
                        ESP_LOGI("Function:", "COFFEE executed");
                        machine_function_indicator(COFFEE_LED, COFFEE_BLINK_COUNT, COFFEE_BLINK_INTERVAL);
                    }
                    else if (milk_led_state == 1)
                    {
                        ESP_LOGI("Function:", "MILK executed");
                        machine_function_indicator(MILK_LED, MILK_BLINK_COUNT, MILK_BLINK_INTERVAL);
                    }
                    else if (boost_led_state == 1)
                    {
                        ESP_LOGI("Function:", "BOOST executed");
                        machine_function_indicator(BOOST_LED, BOOST_BLINK_COUNT, BOOST_BLINK_INTERVAL);
                    }
                    else
                    {
                         ESP_LOGI("Function:", "failure");
                    }

                    // Reset all LEDs to OFF after confirming the selection
                    gpio_set_level(TEA_LED, 0);
                    gpio_set_level(COFFEE_LED, 0);
                    gpio_set_level(MILK_LED, 0);
                    gpio_set_level(BOOST_LED, 0);
                }
                vTaskDelay(100 / portTICK_PERIOD_MS);
        }
}
void app_main(void)
{
        configure_led();
        configure_switch();
        /*************************************RTOS task********************************************/
        //                      task function          taskname           ram size              priority  task handle               cpu
        xTaskCreatePinnedToCore(indicator_led_task , "indicator_led_task" , 10000   , NULL    , 1      , &indicator_led_task_Handle ,0);
        xTaskCreatePinnedToCore(machine_function   , "machine_function"   , 10000   , NULL    , 1      , &machine_function_Handle   ,1);

        while (1) 
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
}
