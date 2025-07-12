/**
 * @file LCD.h
 * @brief Definiciones y prototipos para el manejo de la pantalla LCD con controlador SSD1283A.
 */

#ifndef __LCD_H__
#define __LCD_H__

#include <stdint.h>

#include "hardware/spi.h"
#include "hardware/dma.h"
#include "SSD1283A.h"

#define CAMERA_MAX_N_PLANES 3  /**< Máximo número de planos admitidos para buffers DMA */

/**
 * @struct lcd_platform_config
 * @brief Configuración de la plataforma para la pantalla LCD (SPI y DMA).
 *
 * Incluye los handles y funciones para acceso a SPI y recursos DMA, necesarios para el envío de datos al controlador.
 */
struct lcd_platform_config {
    /**
     * @brief Función para envío bloqueante por SPI (puede usar DMA internamente).
     * @param spi  Puntero al objeto SPI
     * @param src  Puntero al buffer fuente
     * @param len  Número de bytes a enviar
     * @return Número de bytes enviados (negativo en error)
     */
    int8_t (*spi_write_blocking)(void *spi, const uint8_t *src, size_t len);

    spi_inst_t *spi_handle;   /**< Handle al periférico SPI */

    int8_t base_dma_channel;  /**< Canal DMA base (puede usarse uno por plano) */
};

/**
 * @struct lcd_config
 * @brief Configuración de la pantalla LCD dependiente de formato, resolución y DMA.
 */
struct lcd_config {
    uint32_t format;                                /**< Formato de imagen (por ejemplo, RGB565) */
    uint16_t width;                                 /**< Ancho en píxeles */
    uint16_t height;                                /**< Alto en píxeles */
    uint dma_transfers[CAMERA_MAX_N_PLANES];        /**< Transferencias DMA por plano */
    uint dma_offset[CAMERA_MAX_N_PLANES];           /**< Offset DMA por plano */
    dma_channel_config dma_cfgs[CAMERA_MAX_N_PLANES]; /**< Configuración DMA por plano */
};

/**
 * @struct LCD
 * @brief Estructura principal para el manejo de la pantalla LCD.
 */
struct LCD {
    SSD1283A_host driver_host;                      /**< Estructura host para el controlador SSD1283A */
    int dma_channels[CAMERA_MAX_N_PLANES];          /**< Canales DMA asignados */
    struct lcd_config config;                       /**< Configuración dependiente de formato y tamaño */
};

/**
 * @brief Inicializa la pantalla LCD y el controlador SSD1283A.
 * @param lcd      Puntero a la estructura LCD a inicializar
 * @param platform Puntero a la configuración de plataforma
 * @return Estado de la operación (SSD1283A_STATUS_OK si fue exitosa)
 */
SSD1283A_status lcd_init(struct LCD *lcd, struct lcd_platform_config *platform);

/**
 * @brief Llena toda la pantalla LCD con un color específico.
 * @param lcd   Puntero a la estructura LCD
 * @param color Valor de color (formato RGB565)
 */
void lcd_fill_screen(struct LCD *lcd, uint16_t color);

/**
 * @brief Muestra una imagen en la pantalla LCD a partir de un arreglo de colores.
 * @param lcd    Puntero a la estructura LCD
 * @param width  Ancho de la imagen
 * @param height Alto de la imagen
 * @param color  Arreglo de colores (RGB565) a mostrar
 */
void lcd_show_image(struct LCD *lcd, uint16_t width, uint16_t height, uint16_t *color);

#endif // __LCD_H__
