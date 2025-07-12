/**
 * @file camera.h
 * @brief Interfaz para control y captura de imágenes con la cámara OV7670 utilizando PIO y DMA.
 * @author Brian Starkey
 * @date 2022
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdint.h>
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "camera/ov7670.h"

#define CAMERA_WIDTH_DIV8  80   /**< Ancho de la imagen dividido por 8 */
#define CAMERA_HEIGHT_DIV8 60   /**< Alto de la imagen dividido por 8 */
#define CAMERA_MAX_N_PLANES 3   /**< Máximo número de planos de color */

/**
 * @struct camera_buffer
 * @brief Estructura que almacena un frame de la cámara.
 *
 * Puede ser asignada dinámicamente usando camera_buffer_alloc/camera_buffer_free,
 * o de forma estática inicializando manualmente los campos.
 */
struct camera_buffer {
    uint32_t format;                           /**< Formato de imagen (definido en format.h) */
    uint16_t width;                            /**< Ancho en píxeles */
    uint16_t height;                           /**< Alto en píxeles */
    uint32_t strides[CAMERA_MAX_N_PLANES];     /**< Stride en bytes para cada plano */
    uint32_t sizes[CAMERA_MAX_N_PLANES];       /**< Tamaño en bytes de cada plano */
    uint8_t *data[CAMERA_MAX_N_PLANES];        /**< Punteros a los datos de cada plano */
};

/**
 * @brief Callback para notificar la finalización de la captura de un frame.
 * @param buf Puntero al buffer de la imagen capturada
 * @param p   Puntero a datos de usuario
 */
typedef void (*camera_frame_cb)(struct camera_buffer *buf, void *p);

/**
 * @struct camera_config
 * @brief Configuración dependiente de formato/ancho/alto para PIO y DMA.
 */
struct camera_config {
    uint32_t format;                              /**< Formato de la imagen */
    uint16_t width;                               /**< Ancho en píxeles */
    uint16_t height;                              /**< Alto en píxeles */
    uint dma_transfers[CAMERA_MAX_N_PLANES];      /**< Transferencias DMA por plano */
    uint dma_offset[CAMERA_MAX_N_PLANES];         /**< Offset DMA por plano */
    dma_channel_config dma_cfgs[CAMERA_MAX_N_PLANES]; /**< Configuración DMA por plano */
    pio_sm_config sm_cfgs[4];                     /**< Configuración de las state machines PIO */
};

/**
 * @struct camera
 * @brief Contexto y estado de una instancia de cámara.
 */
struct camera {
    OV7670_host driver_host;                         /**< Interfaz del driver bajo nivel OV7670 */
    uint frame_offset;                               /**< Offset para el frame actual */
    uint shift_byte_offset;                          /**< Offset de desplazamiento de byte */
    int dma_channels[CAMERA_MAX_N_PLANES];           /**< Canales DMA utilizados */
    struct camera_config config;                     /**< Configuración dinámica actual */
    struct camera_buffer *volatile pending;          /**< Frame en progreso */
    camera_frame_cb volatile pending_cb;             /**< Callback de frame pendiente */
    void *volatile cb_data;                          /**< Datos de usuario para el callback */
};

/**
 * @struct camera_platform_config
 * @brief Abstracción de la plataforma para inicialización de una cámara.
 */
struct camera_platform_config {
    /**
     * @brief Función para escritura I2C (debe comportarse como el SDK de la RP2040)
     * @param i2c_handle   Handle del bus I2C
     * @param addr         Dirección I2C del dispositivo
     * @param src          Puntero a los datos a escribir
     * @param len          Longitud en bytes
     * @return Número de bytes escritos
     */
    int (*i2c_write_blocking)(void *i2c_handle, uint8_t addr, const uint8_t *src, size_t len);

    /**
     * @brief Función para lectura I2C (debe comportarse como el SDK de la RP2040)
     * @param i2c_handle   Handle del bus I2C
     * @param addr         Dirección I2C del dispositivo
     * @param src          Puntero al buffer de destino
     * @param len          Longitud en bytes
     * @return Número de bytes leídos
     */
    int (*i2c_read_blocking)(void *i2c_handle, uint8_t addr, uint8_t *src, size_t len);

    void *i2c_handle;      /**< Handle del bus I2C */

    // Configuración de PIO y pines
    PIO pio;               /**< PIO utilizado */
    uint xclk_pin;         /**< Pin para la señal XCLK (debe ser 21, 23, 24 o 25) */
    uint xclk_divider;     /**< Divisor de frecuencia para XCLK */
    uint base_pin;         /**< Pin base para conexión de datos */
    int base_dma_channel;  /**< Canal DMA base; -1 para asignación dinámica */
};

/**
 * @brief Inicializa la cámara.
 * @param camera Puntero a la estructura de cámara
 * @param params Puntero a la configuración de plataforma (debe mantenerse válida)
 * @return 0 en caso de éxito, otro valor en caso de error
 */
int camera_init(struct camera *camera, struct camera_platform_config *params);

/**
 * @brief Termina la operación de la cámara (no implementada).
 * @param camera Puntero a la estructura de cámara
 */
void camera_term(struct camera *camera);

/**
 * @brief Configura la cámara para un formato/ancho/alto específico.
 * @param camera Puntero a la estructura de cámara
 * @param format Formato deseado
 * @param width  Ancho deseado en píxeles
 * @param height Alto deseado en píxeles
 * @return 0 en caso de éxito, otro valor en caso de error
 */
int camera_configure(struct camera *camera, uint32_t format, uint16_t width, uint16_t height);

/**
 * @brief Captura un frame de forma bloqueante en el buffer proporcionado.
 * @param camera            Puntero a la estructura de cámara
 * @param into              Puntero al buffer donde almacenar el frame
 * @param allow_reconfigure Permite reconfiguración si es necesario
 * @return 0 en caso de éxito, otro valor en caso de error
 */
int camera_capture_blocking(struct camera *camera, struct camera_buffer *into, bool allow_reconfigure);

/**
 * @brief Captura un frame de forma no bloqueante usando callback.
 * @param camera            Puntero a la estructura de cámara
 * @param into              Puntero al buffer donde almacenar el frame
 * @param allow_reconfigure Permite reconfiguración si es necesario
 * @param complete_cb       Callback a ejecutar al finalizar la captura
 * @param cb_data           Datos de usuario para el callback
 * @return 0 en caso de éxito, otro valor en caso de error
 */
int camera_capture_with_cb(struct camera *camera, struct camera_buffer *into, bool allow_reconfigure,
                           camera_frame_cb complete_cb, void *cb_data);

/**
 * @brief Asigna un buffer de cámara dinámicamente usando malloc.
 * @param format Formato de imagen
 * @param width  Ancho en píxeles
 * @param height Alto en píxeles
 * @return Puntero al buffer asignado, o NULL si falla
 */
struct camera_buffer *camera_buffer_alloc(uint32_t format, uint16_t width, uint16_t height);

/**
 * @brief Libera un buffer previamente asignado por camera_buffer_alloc.
 * @param buf Puntero al buffer a liberar
 */
void camera_buffer_free(struct camera_buffer *buf);

#endif /* __CAMERA_H__ */
