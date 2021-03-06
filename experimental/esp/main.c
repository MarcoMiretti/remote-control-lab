/*
 * Copyright 2021 ACES.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
/** \file main.c */
/* standard */
#include <string.h>

/* third party libs */
#include <FreeRTOS.h>
#include <task.h>
#include <espressif/esp_common.h>
#include <espressif/user_interface.h>
#include <esp/uart.h>
#include <pwm.h>

/* third party local libs */
#include <log.h>

/* configuration includes */
#include <pinout_configuration.h>

/* Global variable: this variable is global for
  communicate between threads */
static uint16_t valor_adc;

/* Global variable: between 0 and 1 */
static float pwm_duty;
static float pwm_duty_percentage;
static float adc_voltage;
uint8_t SYSTEM_LOG_LEVEL = LOG_INFO;



/* Asignación de parametros para PWM */
#define DEFAULT_DRIVER_PWM_PIN              14      /**< \brief Default driver pin */
#define DEFAULT_DRIVER_PWM_FREQUENCY_HZ     100     /**< \brief Default PWM frequency */
#define DRIVER_PWM_COUNT                    1       /**< \brief Quantity of PWMs to use */
#define DRIVER_PWM_REVERSE                  false   /**< \brief PWM Reverse option */
#define SYSTEM_VOLTAGE                      3.3     /**< \brief Default system voltage */
#define ADC_RESOLUTION                      1023    /**< \brief ADC resolution excluding 0 */
#define PWM_DUTY_OFF                        0x0000  /**< \brief Minimum signal value */
#define PWM_DUTY_MAX                        0xFFFF  /**< \brief Maximum signal value */

/**
 * \brief   PWM configuration structure.
 */
typedef struct pwm_config_t {
    uint16_t frequency_hz;  /**< \brief Frequency of the PWM signal in Hertz */
    uint8_t pin;            /**< \brief Pin where the PWM will be initialized */
} pwm_config_t;


/**
 * \brief   Global PWM driver configuration.
 */
pwm_config_t pwm_config = {
    .pin = DEFAULT_DRIVER_PWM_PIN,
    .frequency_hz = DEFAULT_DRIVER_PWM_FREQUENCY_HZ,
};

/**
 * \brief   adc_read.
 */
void adc_read(void *pvParameters) {
    for (;;) {
        valor_adc = sdk_system_adc_read();
        adc_voltage = valor_adc*(SYSTEM_VOLTAGE/ADC_RESOLUTION);
        log_debug("ADC Voltage: %f", adc_voltage);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

/**
 * \brief   uart_publisher.
 */
void uart_publisher(void *pvParameters) {
    for (;;) {
        pwm_duty_percentage = pwm_duty*(100.0/(PWM_DUTY_MAX-1.0));
        printf("{\"pwm\": %f, \"adc\": %f}\n", pwm_duty_percentage, adc_voltage);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


/**
 * \brief   PWM_writer.
 */
void PWM_writer(void *pvParameters) {
    log_trace("Set PWM in pin %d", pwm_config.pin);
    pwm_init(DRIVER_PWM_COUNT, &pwm_config.pin, DRIVER_PWM_REVERSE);

    log_trace("Set PWM frequency to %d Hz", pwm_config.frequency_hz);
    pwm_set_freq(pwm_config.frequency_hz);

    log_trace("Set PWM default duty");
    pwm_set_duty(PWM_DUTY_OFF);

    log_trace("Start PWM");
    pwm_start();
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    for (;;) {
        log_trace("Set PWM default duty");
        pwm_duty = PWM_DUTY_MAX/2;
        pwm_set_duty((uint16_t) (pwm_duty));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        log_trace("Set PWM default duty again");
        pwm_duty = PWM_DUTY_OFF;
        pwm_set_duty(pwm_duty);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}


/**
 * \brief Program's entrypoint.
 */
void user_init(void) {
    uart_set_baud(0, 115200);
    log_set_level(SYSTEM_LOG_LEVEL);
    log_info("SDK version:%s ", sdk_system_get_sdk_version());

    /* turn off LED */
    gpio_enable(ONBOARD_LED_PIN, GPIO_OUTPUT);
    gpio_write(ONBOARD_LED_PIN, true);

    /* initialize tasks */
    xTaskCreate(&adc_read, "adc read", 256, NULL, 2, NULL);
    xTaskCreate(&uart_publisher, "uart publisher", 256, NULL, 2, NULL);
    xTaskCreate(&PWM_writer, "PWM_writer", 256, NULL, 2, NULL);
}
