/**
 * @file format.h
 * @brief Definiciones y utilidades para el manejo de formatos de imagen de cámara.
 * @author Brian Starkey
 * @date 2022
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __FORMAT_H__
#define __FORMAT_H__

#include <stdint.h>

/**
 * @brief Macro para crear un código de formato de 4 caracteres.
 * @param a Primer carácter
 * @param b Segundo carácter
 * @param c Tercer carácter
 * @param d Cuarto carácter
 * @return Código de formato de 32 bits
 */
#define FORMAT_CODE(a, b, c, d) (((uint32_t)(a) << 0) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define FORMAT_YUYV   FORMAT_CODE('Y', 'U', 'Y', 'V')  /**< Formato YUYV */
#define FORMAT_RGB565 FORMAT_CODE('R', 'G', '1', '6')  /**< Formato RGB565 */
#define FORMAT_YUV422 FORMAT_CODE('Y', 'U', '1', '6')  /**< Formato YUV422 */

/**
 * @brief Obtiene el número de planos para un formato dado.
 * @param format Código de formato
 * @return Número de planos (ejemplo: 1 para RGB, 2 o más para YUV)
 */
uint8_t format_num_planes(uint32_t format);

/**
 * @brief Obtiene el número de bytes por pixel para un formato y plano dado.
 * @param format Código de formato
 * @param plane  Índice del plano
 * @return Bytes por pixel en el plano especificado
 */
uint8_t format_bytes_per_pixel(uint32_t format, uint8_t plane);

/**
 * @brief Obtiene el factor de submuestreo horizontal para un formato y plano dado.
 * @param format Código de formato
 * @param plane  Índice del plano
 * @return Submuestreo horizontal (1 = sin submuestreo, 2 = mitad)
 */
uint8_t format_hsub(uint32_t format, uint8_t plane);

/**
 * @brief Calcula el stride (longitud en bytes de una línea) para un formato, plano y ancho dados.
 * @param format Código de formato
 * @param plane  Índice del plano
 * @param width  Ancho en píxeles
 * @return Stride en bytes
 */
uint32_t format_stride(uint32_t format, uint8_t plane, uint16_t width);

/**
 * @brief Calcula el tamaño del plano en bytes para un formato, plano, ancho y alto dados.
 * @param format Código de formato
 * @param plane  Índice del plano
 * @param width  Ancho en píxeles
 * @param height Alto en píxeles
 * @return Tamaño del plano en bytes
 */
uint32_t format_plane_size(uint32_t format, uint8_t plane, uint16_t width, uint16_t height);

#endif /* __FORMAT_H__ */
