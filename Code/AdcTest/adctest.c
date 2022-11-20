/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    printf("ADC Example, measuring GPIO26\n");

    adc_init();

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);

    while (1) {
        // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
        const float conversion_factor = 3.3f / (1 << 12);
        // Select ADC input 0 (GPIO26)
        adc_select_input(0);
        uint16_t result = adc_read();
        printf("ADC 1: Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
        // Select ADC input 1 (GPIO27)
        adc_select_input(1);
        result = adc_read();
        printf("ADC 2: Raw value: 0x%03x, voltage: %f V\n", result, result * conversion_factor);
    }
}
