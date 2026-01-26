/**
 * SSD1681 E-Paper Display Driver for Raspberry Pi Pico
 * Direct Pico SDK implementation - no HAL abstraction
 * 
 * Copyright (c) 2026 OpenCode
 * SPDX-License-Identifier: MIT
 */

#ifndef PICO_SSD1681_H
#define PICO_SSD1681_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI mode configuration
 */
typedef enum {
    SSD1681_SPI_4WIRE = 0,  /**< 4-wire SPI with separate D/C pin */
    SSD1681_SPI_3WIRE = 1,  /**< 3-wire SPI with 9-bit frames (no D/C pin) */
} ssd1681_spi_mode_t;

/**
 * @brief Pin configuration structure
 */
typedef struct {
    ssd1681_spi_mode_t spi_mode;  /**< SPI mode (3-wire or 4-wire) */
    uint8_t spi_port;             /**< SPI port: 0 or 1 */
    uint8_t pin_mosi;             /**< MOSI pin */
    uint8_t pin_sck;              /**< SCK pin */
    uint8_t pin_cs;               /**< CS pin */
    uint8_t pin_dc;               /**< D/C pin (unused in 3-wire mode) */
    uint8_t pin_rst;              /**< Reset pin */
    uint8_t pin_busy;             /**< Busy status pin */
    uint32_t spi_baudrate;        /**< SPI clock frequency in Hz */
} ssd1681_config_t;

/**
 * @brief Color selection
 */
typedef enum {
    SSD1681_COLOR_BLACK = 0,
    SSD1681_COLOR_RED = 1,
} ssd1681_color_t;

/**
 * @brief Font size
 */
typedef enum {
    SSD1681_FONT_12 = 12,
    SSD1681_FONT_16 = 16,
    SSD1681_FONT_24 = 24,
} ssd1681_font_t;

/**
 * @brief Initialize the display
 * @param config Pin configuration
 * @return 0 on success, non-zero on error
 */
int ssd1681_init(const ssd1681_config_t *config);

/**
 * @brief Deinitialize the display
 */
void ssd1681_deinit(void);

/**
 * @brief Clear the display
 * @param color Color plane to clear
 * @return 0 on success
 */
int ssd1681_clear(ssd1681_color_t color);

/**
 * @brief Write a single point
 * @param color Color plane
 * @param x X coordinate (0-199)
 * @param y Y coordinate (0-199)
 * @param data 1=on, 0=off
 * @return 0 on success
 */
int ssd1681_write_point(ssd1681_color_t color, uint8_t x, uint8_t y, uint8_t data);

/**
 * @brief Read a single point
 * @param color Color plane
 * @param x X coordinate
 * @param y Y coordinate
 * @param data Output: pixel value
 * @return 0 on success
 */
int ssd1681_read_point(ssd1681_color_t color, uint8_t x, uint8_t y, uint8_t *data);

/**
 * @brief Draw a string
 * @param color Color plane
 * @param x X coordinate
 * @param y Y coordinate
 * @param str String to draw
 * @param len String length
 * @param data 1=visible, 0=invisible
 * @param font Font size
 * @return 0 on success
 */
int ssd1681_draw_string(ssd1681_color_t color, uint8_t x, uint8_t y, 
                        const char *str, uint16_t len, uint8_t data, 
                        ssd1681_font_t font);

/**
 * @brief Fill a rectangle
 * @param color Color plane
 * @param left Left X coordinate
 * @param top Top Y coordinate
 * @param right Right X coordinate
 * @param bottom Bottom Y coordinate
 * @param data Fill value (1=filled, 0=empty)
 * @return 0 on success
 */
int ssd1681_fill_rect(ssd1681_color_t color, uint8_t left, uint8_t top,
                      uint8_t right, uint8_t bottom, uint8_t data);

/**
 * @brief Draw an image
 * @param color Color plane
 * @param left Left X coordinate
 * @param top Top Y coordinate
 * @param right Right X coordinate
 * @param bottom Bottom Y coordinate
 * @param img Image buffer (1 bit per pixel, row-major)
 * @return 0 on success
 */
int ssd1681_draw_picture(ssd1681_color_t color, uint8_t left, uint8_t top,
                         uint8_t right, uint8_t bottom, const uint8_t *img);

/**
 * @brief Update the display (refresh)
 * @return 0 on success
 */
int ssd1681_update(void);

/**
 * @brief Get default configuration for 4-wire SPI
 * @param config Output configuration
 */
void ssd1681_get_default_config_4wire(ssd1681_config_t *config);

/**
 * @brief Get default configuration for 3-wire SPI
 * @param config Output configuration
 */
void ssd1681_get_default_config_3wire(ssd1681_config_t *config);

#ifdef __cplusplus
}
#endif

#endif /* pico_ssd1681.h */
