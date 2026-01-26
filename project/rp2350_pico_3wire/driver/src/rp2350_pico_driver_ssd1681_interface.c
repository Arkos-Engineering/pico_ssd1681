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
 * @brief     rp2350 pico driver ssd1681 interface source file (3-WIRE SPI)
 * @version   1.0.0
 * @author    LibDriver / OpenCode
 * @date      2026-01-26
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author          <th>Description
 * <tr><td>2026/01/26  <td>1.0      <td>OpenCode        <td>RP2350 Pico SDK 3-wire SPI implementation
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
 * @brief   D/C state tracker for 3-wire SPI mode
 * @details In 3-wire SPI, we don't have a physical D/C pin.
 *          Instead, we send the D/C bit as the 9th bit before each byte.
 *          0 = Command mode, 1 = Data mode
 */
static uint8_t g_dc_state = 0;

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   Initializes SPI in 9-bit mode for 3-wire communication
 */
uint8_t ssd1681_interface_spi_init(void)
{
    /* Initialize SPI at configured baudrate */
    uint32_t actual_baudrate = spi_init(SSD1681_SPI_PORT, SSD1681_SPI_BAUDRATE);
    
    /* Check if initialization was successful */
    if (actual_baudrate == 0) {
        return 1;
    }
    
    /* Reconfigure SPI for 9-bit frames (D/C bit + 8 data bits) */
    /* According to RP2350 datasheet, DSS field = data_bits - 1 */
    /* For 9-bit: DSS = 8 (0b1000) */
    spi_hw_t *spi_hw = spi_get_hw(SSD1681_SPI_PORT);
    
    /* Disable SPI before changing format */
    hw_clear_bits(&spi_hw->cr1, SPI_SSPCR1_SSE_BITS);
    
    /* Set 9-bit data size, SPI mode 0 (CPOL=0, CPHA=0), MSB first */
    /* SSPCR0 register format:
     * Bits 7:0  = DSS (Data Size Select) = 8 for 9-bit
     * Bits 5:4  = FRF (Frame Format) = 0 for SPI
     * Bit  6    = SPO (Clock Polarity) = 0
     * Bit  7    = SPH (Clock Phase) = 0
     */
    spi_hw->cr0 = (8 << SPI_SSPCR0_DSS_LSB) |  /* 9-bit data */
                  (0 << SPI_SSPCR0_FRF_LSB) |  /* SPI frame format */
                  (0 << SPI_SSPCR0_SPO_LSB) |  /* CPOL = 0 */
                  (0 << SPI_SSPCR0_SPH_LSB);   /* CPHA = 0 */
    
    /* Re-enable SPI */
    hw_set_bits(&spi_hw->cr1, SPI_SSPCR1_SSE_BITS);
    
    /* Configure GPIO pins for SPI function */
    gpio_set_function(SSD1681_PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(SSD1681_PIN_SCK, GPIO_FUNC_SPI);
    
    /* Configure CS pin as GPIO output (manual control) */
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
 * @brief     send a 9-bit frame via SPI (D/C bit + 8 data bits)
 * @param[in] data 8-bit data to send
 * @note      Uses hardware SPI with 9-bit frame format
 */
static void spi_write_9bit(uint8_t data)
{
    spi_hw_t *spi_hw = spi_get_hw(SSD1681_SPI_PORT);
    
    /* Build 9-bit frame: D/C bit (bit 8) + data (bits 7:0) */
    uint16_t frame = ((uint16_t)g_dc_state << 8) | data;
    
    /* Wait for TX FIFO to have space */
    while (!(spi_hw->sr & SPI_SSPSR_TNF_BITS)) {
        tight_loop_contents();
    }
    
    /* Write 9-bit frame to data register */
    spi_hw->dr = frame;
    
    /* Wait for transmission to complete */
    while (spi_hw->sr & SPI_SSPSR_BSY_BITS) {
        tight_loop_contents();
    }
}

/**
 * @brief     interface spi bus write
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      Sends each byte as a 9-bit frame (D/C + 8 data bits)
 */
uint8_t ssd1681_interface_spi_write_cmd(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) {
        return 1;
    }
    
    /* Assert CS (active low) */
    gpio_put(SSD1681_PIN_CS, 0);
    
    /* Send each byte as 9-bit frame */
    for (uint16_t i = 0; i < len; i++) {
        spi_write_9bit(buf[i]);
    }
    
    /* Deassert CS */
    gpio_put(SSD1681_PIN_CS, 1);
    
    return 0;
}

/**
 * @brief      interface spi bus read
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       3-wire SPI read is complex (requires bidirectional MOSI)
 *             Most e-paper operations don't require reading back data
 */
uint8_t ssd1681_interface_spi_read_cmd(uint8_t *buf, uint16_t len)
{
    if (buf == NULL || len == 0) {
        return 1;
    }
    
    /* 3-wire SPI read requires special handling:
     * 1. MOSI line must become bidirectional (input after command phase)
     * 2. Display drives the line during read phase
     * 
     * This is rarely used with e-paper displays, so we return an error.
     * If you need read functionality, implement bit-banging or use 4-wire mode.
     */
    
    return 1;  /* Not supported in basic 3-wire implementation */
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
 * @note   In 3-wire mode, there is no physical D/C pin.
 *         This function just initializes our state tracker.
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_init(void)
{
    /* Initialize D/C state to command mode */
    g_dc_state = 0;
    
    return 0;
}

/**
 * @brief  interface command && data gpio deinit
 * @return status code
 *         - 0 success
 *         - 1 gpio deinit failed
 * @note   No physical pin to deinitialize in 3-wire mode
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_deinit(void)
{
    /* Nothing to do - no physical pin */
    return 0;
}

/**
 * @brief     interface command && data gpio write
 * @param[in] value written value (0=command, 1=data)
 * @return    status code
 *            - 0 success
 *            - 1 gpio write failed
 * @note      Stores D/C state for next SPI transmission
 */
uint8_t ssd1681_interface_spi_cmd_data_gpio_write(uint8_t value)
{
    /* Store D/C state for next 9-bit frame */
    g_dc_state = value ? 1 : 0;
    
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
