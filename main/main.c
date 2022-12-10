#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/timers.h"
#include "driver/adc.h"


#define led1 22
#define ledR 33
#define ledG 25
#define ledB 26

uint8_t led_level =0;
static const char* tag = "Main";
TimerHandle_t xTimers;
int timerId = 1;
int intervalo = 50;
int adc_val = 0;


esp_err_t init_led(void);
esp_err_t blink_led(void);
esp_err_t set_timer(void);
esp_err_t set_adc(void);
void vTimerCallback( TimerHandle_t pxTimer);



void app_main(void)
{
    init_led();
    set_timer();

}

esp_err_t init_led(void){
    gpio_reset_pin(led1);
    gpio_set_direction(led1, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledB);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);


    return ESP_OK;
}

esp_err_t blink_led(void){
    led_level = !led_level;
    gpio_set_level(led1,led_level);
    return 0;
}

void vTimerCallback( TimerHandle_t pxTimer ){

    blink_led();
    adc_val = adc1_get_raw(ADC1_CHANNEL_4);

    int adc_case = adc_val / 1000; //max 4094
    ESP_LOGI(tag, "ADC VAL: %i", adc_val);

    switch(adc_val){
        case 0:
            gpio_set_level(ledR, 0);
            gpio_set_level(ledG, 0);
            gpio_set_level(ledB, 0);
            break;

        case 1:
            gpio_set_level(ledR, 1);
            gpio_set_level(ledG, 0);
            gpio_set_level(ledB, 0);
            break;

        case 2:
            gpio_set_level(ledR, 1);
            gpio_set_level(ledG, 1);
            gpio_set_level(ledB, 0);
            break;

        case 3 :
        case 4:
            gpio_set_level(ledR, 1);
            gpio_set_level(ledG, 1);
            gpio_set_level(ledB, 1);
            break;
        
        default:
            break;

    }

}


esp_err_t set_timer(void){
    ESP_LOGI(tag, "Timer init configuration");
    xTimers = xTimerCreate(    "Timer",       // Just a text name, not used by the kernel.
                                    (pdMS_TO_TICKS(intervalo)), //( 100 * x ),   // The timer period in ticks.
                                    pdTRUE,        // The timers will auto-reload themselves when they expire.
                                    ( void * ) timerId,  // Assign each timer a unique id equal to its array index.
                                    vTimerCallback // Each timer calls the same callback when it expires.
                                );
    
    if( xTimers == NULL ){
        // The timer was not created.
        ESP_LOGI(tag, "The timer was not created.");
    }
    else{
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if( xTimerStart( xTimers, 0 ) != pdPASS ){
            // The timer could not be set into the Active state.
            ESP_LOGI(tag, "The timer could not be set into the Active state");
        }
    }
    return ESP_OK;
}

esp_err_t set_adc(void)
{
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_11db);
    adc1_config_width(ADC_WIDTH_BIT_12);
    return ESP_OK;
}