/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      rp2350_pico_driver_ssd1681_interface.c
 * @brief     rp2350 pico driver ssd1681 interface source file
 * @version   1.0.0
 * @author    LibDriver / OpenCode
 * @date      2026-01-26
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author          <th>Description
 * <tr><td>2026/01/26  <td>1.0      <td>OpenCode        <td>RP2350 Pico SDK implementation
 * </table>
 */

#include "driver_ssd1681_interface.h"
#include "rp2350_pico_driver_ssd1681_interface.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdarg.h>
#include <stdio.h>

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   Initializes SPI0 at 4 MHz for 4-wire SPI communication
 */
uint8_t ssd1681_interface_spi_init(void)
{
    /* Initialize SPI at configured baudrate */
    uint32_t actual_baudrate = spi_init(SSD1681_SPI_PORT, SSD1681_SPI_BAUDRATE);
    
    /* Check if initialization was successful */
    if (actual_baudrate == 0) {
        return 1;
    }
    
    /* Set SPI format: 8 data bits, SPI mode 0 (CPOL=0, CPHA=0) */
    spi_set_format(SSD1681_SPI_PORT, 
                   8,           /* 8 bits per transfer */
                   SPI_CPOL_0,  /* Clock polarity 0 */
                   SPI_CPHA_0,  /* Clock phase 0 */
                   SPI_MSB_FIRST);
    
    /* Configure GPIO pins for SPI function */
    gpio_set_function(SSD1681_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SSD1681_PIN_SCK, GPIO_FUNC_SPI);
    
    /* Configure CS pin as GPIO output (we'll control it manually) */
    gpio_init(SSD1681_PIN_CS);
    gpio_set_dir(SSD1681_PIN_CS, GPIO_OUT);
    gpio_put(SSD1681_PIN_CS, 1);  /* CS idle high (inactive) */
    
    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   Deinitializes SPI peripheral
 */
uint8_t ssd1681_interface_spi_deinit(void)
{
    spi_deinit(SSD1681_SPI_PORT);
    
    return 0;
}

/**
 * @brief     interface spi bus write
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      Performs blocking SPI write with CS control
 */
uint8_t ssd1681_interface_spi_write_cmd(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) {
        return 1;
    }
    
    /* Assert CS (active low) */
    gpio_put(SSD1681_PIN_CS, 0);
    
    /* Write data via SPI */
    int bytes_written = spi_write_blocking(SSD1681_SPI_PORT, buf, len);
    
    /* Deassert CS */
    gpio_put(SSD1681_PIN_CS, 1);
    
    /* Check if all bytes were written */
    if (bytes_written != (int)len) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief      interface spi bus read
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       Performs blocking SPI read with CS control
 */
uint8_t ssd1681_interface_spi_read_cmd(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) {
        return 1;
    }
    
    /* Assert CS (active low) */
    gpio_put(SSD1681_PIN_CS, 0);
    
    /* Read data via SPI (write dummy bytes to generate clock) */
    int bytes_read = spi_read_blocking(SSD1681_SPI_PORT, 0x00, buf, len);
    
    /* Deassert CS */
    gpio_put(SSD1681_PIN_CS, 1);
    
    /* Check if all bytes were read */
    if (bytes_read != (int)len) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms time in milliseconds
 * @note      Uses Pico SDK sleep function
 */
void ssd1681_interface_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt format string
 * @note      Prints to stdout (USB or UART depending on SDK configuration)
 */
void ssd1681_interface_debug_print(const char *const fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

/**
 * @brief  interface command && data gpio init
 * @return status code
 *         - 0 success
 *         - 1 gpio init failed
 * @note   Initializes the D/C (Data/Command) select GPIO
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_init(void)
{
    gpio_init(SSD1681_PIN_DC);
    gpio_set_dir(SSD1681_PIN_DC, GPIO_OUT);
    gpio_put(SSD1681_PIN_DC, 0);  /* Default to command mode */
    
    return 0;
}

/**
 * @brief  interface command && data gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   Deinitializes the D/C GPIO
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_deinit(void)
{
    gpio_deinit(SSD1681_PIN_DC);
    
    return 0;
}

/**
 * @brief     interface command && data gpio write
 * @param[in] value written value (0=command, 1=data)
 * @return    status code
 *            - 0 success
 *            - 1 gpio write failed
 * @note      Controls D/C pin: 0=command mode, 1=data mode
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_write(uint8_t value)
{
    gpio_put(SSD1681_PIN_DC, value ? 1 : 0);
    
    return 0;
}

/**
 * @brief  interface reset gpio init
 * @return status code
 *         - 0 success
 *         - 1 gpio init failed
 * @note   Initializes the reset GPIO
 */
uint8_t ssd1681_interface_reset_gpio_init(void)
{
    gpio_init(SSD1681_PIN_RST);
    gpio_set_dir(SSD1681_PIN_RST, GPIO_OUT);
    gpio_put(SSD1681_PIN_RST, 1);  /* Inactive (not in reset) */
    
    return 0;
}

/**
 * @brief  interface reset gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   Deinitializes the reset GPIO
 */
uint8_t ssd1681_interface_reset_gpio_deinit(void)
{
    gpio_deinit(SSD1681_PIN_RST);
    
    return 0;
}

/**
 * @brief     interface reset gpio write
 * @param[in] value written value (0=reset active, 1=reset inactive)
 * @return    status code
 *            - 0 success
 *            - 1 gpio write failed
 * @note      Controls reset pin: 0=active (reset), 1=inactive (normal)
 */
uint8_t ssd1681_interface_reset_gpio_write(uint8_t value)
{
    gpio_put(SSD1681_PIN_RST, value ? 1 : 0);
    
    return 0;
}

/**
 * @brief  interface busy gpio init
 * @return status code
 *         - 0 success
 *         - 1 gpio init failed
 * @note   Initializes the busy status GPIO as input
 */
uint8_t ssd1681_interface_busy_gpio_init(void)
{
    gpio_init(SSD1681_PIN_BUSY);
    gpio_set_dir(SSD1681_PIN_BUSY, GPIO_IN);
    /* Enable pull-down to prevent floating when display not connected */
    gpio_pull_down(SSD1681_PIN_BUSY);
    
    return 0;
}

/**
 * @brief  interface busy gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   Deinitializes the busy GPIO
 */
uint8_t ssd1681_interface_busy_gpio_deinit(void)
{
    gpio_deinit(SSD1681_PIN_BUSY);
    
    return 0;
}

/**
 * @brief      interface busy gpio read
 * @param[out] *value pointer to a value buffer
 * @return     status code
 *             - 0 success
 *             - 1 gpio read failed
 * @note       Reads busy status: 1=busy, 0=ready
 */
uint8_t ssd1681_interface_busy_gpio_read(uint8_t *value)
{
    if (value == NULL) {
        return 1;
    }
    
    *value = gpio_get(SSD1681_PIN_BUSY) ? 1 : 0;
    
    return 0;
}
