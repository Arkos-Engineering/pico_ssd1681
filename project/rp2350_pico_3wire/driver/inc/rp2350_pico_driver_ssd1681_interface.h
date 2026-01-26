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
 * @file      rp2350_pico_driver_ssd1681_interface.h
 * @brief     rp2350 pico driver ssd1681 interface header file (3-WIRE SPI)
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

#ifndef RP2350_PICO_DRIVER_SSD1681_INTERFACE_H
#define RP2350_PICO_DRIVER_SSD1681_INTERFACE_H

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @defgroup rp2350_pico_ssd1681_pin_config RP2350 Pico Pin Configuration (3-Wire SPI)
 * @brief    Pin assignments for SSD1681 e-paper display using 3-wire SPI
 * @{
 */

/* Default pin assignments for RP2350 Pico - 3-WIRE SPI MODE */
#define SSD1681_SPI_PORT        spi0        /**< SPI peripheral (spi0 or spi1) */
#define SSD1681_PIN_MOSI        19          /**< SPI MOSI (Data In/Out) */
#define SSD1681_PIN_SCK         18          /**< SPI Clock */
#define SSD1681_PIN_CS          17          /**< Chip Select (active low) */
/* NOTE: No D/C pin in 3-wire mode - D/C is sent as 9th bit */
#define SSD1681_PIN_RST         21          /**< Reset (active low) */
#define SSD1681_PIN_BUSY        22          /**< Busy status (1=busy, 0=ready) */

#define SSD1681_SPI_BAUDRATE    4000000     /**< SPI clock frequency (4 MHz) */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* RP2350_PICO_DRIVER_SSD1681_INTERFACE_H */
