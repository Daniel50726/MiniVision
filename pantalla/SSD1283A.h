/**
 * @file SSD1283A.h
 * @brief Definiciones, estructuras y prototipos para el controlador de pantalla LCD SSD1283A.
 */

#ifndef __SSD1283A_H__
#define __SSD1283A_H__

#include <stdint.h>
#include "pico/stdlib.h"

/**
 * @typedef SSD1283A_pin
 * @brief Define un número de pin usado por el controlador SSD1283A.
 */
typedef uint8_t SSD1283A_pin;

/**
 * @enum SSD1283A_status
 * @brief Códigos de estado devueltos por funciones de la librería SSD1283A.
 */
typedef enum {
  SSD1283A_STATUS_OK = 0,         ///< Operación exitosa
  SSD1283A_STATUS_ERR_MALLOC,     ///< Error de asignación de memoria
  SSD1283A_STATUS_ERR_PERIPHERAL, ///< Periférico no encontrado
} SSD1283A_status;

/**
 * @struct SSD1283A_pins
 * @brief Estructura que agrupa los pines físicos usados por el display SSD1283A.
 */
typedef struct {
    SSD1283A_pin cs;  /**< Pin Chip Select */
    SSD1283A_pin dc;  /**< Pin Data/Command */
    SSD1283A_pin rst; /**< Pin Reset */
    SSD1283A_pin led; /**< Pin Backlight (retroiluminación) */
} SSD1283A_pins;

/**
 * @struct SSD1283A_command
 * @brief Par (registro, valor) para inicialización o configuración del display.
 */
typedef struct {
  uint16_t reg;   /**< Dirección del registro */
  uint16_t value; /**< Valor a escribir en el registro */
} SSD1283A_command;

/**
 * @struct SSD1283A_host
 * @brief Describe una instancia del controlador SSD1283A con los pines físicos y datos de plataforma.
 */
typedef struct {
  SSD1283A_pins *pins; /**< Estructura de pines conectados al display */
  void *platform;      /**< Datos de plataforma específicos (ejemplo: config SPI) */
} SSD1283A_host;

/**
 * @name Direcciones de comandos del SSD1283A
 * @brief Macros con las direcciones de registro/comando del controlador SSD1283A.
 * @{
 */
#define SSD1283A_CMD_OSCILLATION_START     0x00  /**< @brief Oscillation start */
#define SSD1283A_CMD_DRIVER_OUTPUT_CONTROL 0x01  /**< @brief Driver output control */
#define SSD1283A_CMD_LCD_DRIVE_AC_CONTROL  0x02  /**< @brief LCD drive AC control */
#define SSD1283A_CMD_ENTRY_MODE            0x03  /**< @brief Entry mode */
#define SSD1283A_CMD_COMPARE_REGISTER1     0x04  /**< @brief Compare register 1 */
#define SSD1283A_CMD_COMPARE_REGISTER2     0x05  /**< @brief Compare register 2 */
#define SSD1283A_CMD_DISPLAY_CONTROL       0x07  /**< @brief Display control */
#define SSD1283A_CMD_FRAME_CYCLE_CONTROL   0x0B  /**< @brief Frame cycle control */
#define SSD1283A_CMD_POWER_CONTROL1        0x10  /**< @brief Power control 1 */
#define SSD1283A_CMD_POWER_CONTROL2        0x11  /**< @brief Power control 2 */
#define SSD1283A_CMD_POWER_CONTROL3        0x12  /**< @brief Power control 3 */
#define SSD1283A_CMD_POWER_CONTROL4        0x13  /**< @brief Power control 4 */
#define SSD1283A_CMD_POWER_CONTROL5        0x1E  /**< @brief Power control 5 */
#define SSD1283A_CMD_POWER_CONTROL6        0x1F  /**< @brief Power control 6 */

#define SSD1283A_CMD_HORIZONTAL_PORCH      0x16  /**< @brief Horizontal porch */
#define SSD1283A_CMD_VERTICAL_PORCH        0x17  /**< @brief Vertical porch */

#define SSD1283A_CMD_RAM_WRITE_MASK1       0x23  /**< @brief RAM write mask 1 */
#define SSD1283A_CMD_RAM_WRITE_MASK2       0x24  /**< @brief RAM write mask 2 */

#define SSD1283A_CMD_GAMMA_CONTROL1        0x30  /**< @brief Gamma control 1 */
#define SSD1283A_CMD_GAMMA_CONTROL2        0x31  /**< @brief Gamma control 2 */
#define SSD1283A_CMD_GAMMA_CONTROL3        0x32  /**< @brief Gamma control 3 */
#define SSD1283A_CMD_GAMMA_CONTROL4        0x33  /**< @brief Gamma control 4 */
#define SSD1283A_CMD_GAMMA_CONTROL5        0x34  /**< @brief Gamma control 5 */
#define SSD1283A_CMD_GAMMA_CONTROL6        0x35  /**< @brief Gamma control 6 */
#define SSD1283A_CMD_GAMMA_CONTROL7        0x36  /**< @brief Gamma control 7 */
#define SSD1283A_CMD_GAMMA_CONTROL8        0x37  /**< @brief Gamma control 8 */
#define SSD1283A_CMD_GAMMA_CONTROL9        0x38  /**< @brief Gamma control 9 */
#define SSD1283A_CMD_GAMMA_CONTROL10       0x39  /**< @brief Gamma control 10 */

#define SSD1283A_CMD_GATE_SCAN_POS         0x40  /**< @brief Gate scan position */
#define SSD1283A_CMD_VERT_SCROLL_CONTROL   0x41  /**< @brief Vertical scroll control */
#define SSD1283A_CMD_FIRST_OUTPUT_POS      0x42  /**< @brief First output position */
#define SSD1283A_CMD_SECOND_OUTPUT_POS     0x43  /**< @brief Second output position */
#define SSD1283A_CMD_HORIZONTAL_RAM_ADDR   0x44  /**< @brief Horizontal RAM address set */
#define SSD1283A_CMD_VERTICAL_RAM_ADDR     0x45  /**< @brief Vertical RAM address set */

#define SSD1283A_CMD_RAM_WRITE             0x22  /**< @brief Iniciar escritura de datos en RAM */
#define SSD1283A_CMD_SET_GDDRAM_XY         0x21  /**< @brief Dirección de escritura actual en RAM */
/** @} */

/**
 * @brief Inicializa el display SSD1283A.
 * @param host Puntero a la estructura SSD1283A_host con la configuración de hardware.
 * @return Estado de la operación (SSD1283A_STATUS_OK si fue exitosa)
 */
SSD1283A_status SSD1283A_begin(SSD1283A_host *host);

/**
 * @brief Envía una lista de comandos (y retardos) al display SSD1283A.
 * @param host     Puntero a la estructura SSD1283A_host
 * @param platform Puntero a datos de plataforma (por ejemplo, handle SPI)
 * @param cmd      Puntero al arreglo de comandos a enviar
 */
void SSD1283A_write_list(SSD1283A_host *host, void *platform, const SSD1283A_command *cmd);

#endif // __SSD1283A_H__
