/**
 * @file format.c
 * @brief Implementación de funciones auxiliares para el manejo de formatos de imagen de cámara.
 *
 * Proporciona utilidades para el cálculo de planos, bytes por pixel, stride y tamaño de cada plano.
 */

#include "camera/format.h"

/**
 * @brief Obtiene el número de planos de un formato de imagen.
 * @param format Código del formato (por ejemplo, FORMAT_YUYV, FORMAT_RGB565, FORMAT_YUV422).
 * @return Número de planos (1 o 3), o 0 si el formato no es reconocido.
 */
uint8_t format_num_planes(uint32_t format)
{
	switch (format) {
	case FORMAT_YUYV:
		/* Fallthrough */
	case FORMAT_RGB565:
		return 1;
	case FORMAT_YUV422:
		return 3;
	default:
		return 0;
	}
}

/**
 * @brief Obtiene la cantidad de bytes por pixel en un formato y plano dado.
 * @param format Código de formato.
 * @param plane  Índice del plano.
 * @return Número de bytes por pixel en ese plano.
 */
uint8_t format_bytes_per_pixel(uint32_t format, uint8_t plane)
{
	switch (format) {
	case FORMAT_YUYV:
		/* Fallthrough */
	case FORMAT_RGB565:
		return 2;
	case FORMAT_YUV422:
		return 1;
	default:
		return 0;
	}
}

/**
 * @brief Obtiene el factor de submuestreo horizontal de un formato y plano.
 * @param format Código de formato.
 * @param plane  Índice del plano.
 * @return Valor de submuestreo horizontal (1 o 2).
 */
uint8_t format_hsub(uint32_t format, uint8_t plane)
{
	switch (format) {
	case FORMAT_YUV422:
		return plane ? 2 : 1;
	default:
		return 1;
	}
}

/**
 * @brief Calcula el stride (número de bytes por fila) para un formato/plano/ancho dado.
 * @param format Código de formato.
 * @param plane  Índice del plano.
 * @param width  Ancho en píxeles.
 * @return Número de bytes por fila (stride).
 */
uint32_t format_stride(uint32_t format, uint8_t plane, uint16_t width)
{
	return format_bytes_per_pixel(format, plane) * width / format_hsub(format, plane);
}

/**
 * @brief Calcula el tamaño total (en bytes) de un plano de imagen.
 * @param format Código de formato.
 * @param plane  Índice del plano.
 * @param width  Ancho en píxeles.
 * @param height Alto en píxeles.
 * @return Tamaño en bytes del plano.
 */
uint32_t format_plane_size(uint32_t format, uint8_t plane, uint16_t width, uint16_t height)
{
	return format_stride(format, plane, width) * height;
}
