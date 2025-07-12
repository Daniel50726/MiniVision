/**
 * @file ov7670.h
 * @brief Definiciones, estructuras y funciones para controlar la cámara OV7670 en plataformas embebidas.
 *
 * SPDX-FileCopyrightText: 2020 P Burgess for Adafruit Industries  
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "pico/stdlib.h"

/**
 * @def OV7670_XCLK_HZ
 * @brief Frecuencia típica del pin XCLK para la cámara OV7670 (15.625 MHz).
 */
#define OV7670_XCLK_HZ 15625000

/**
 * @typedef OV7670_pin
 * @brief Representa el número de pin físico usado para conectar la cámara.
 */
typedef int OV7670_pin;

/**
 * @typedef OV7670_arch
 * @brief Configuración específica de arquitectura (uso interno).
 */
typedef int OV7670_arch;

/**
 * @enum OV7670_status
 * @brief Códigos de estado devueltos por algunas funciones OV7670.
 */
typedef enum {
  OV7670_STATUS_OK = 0,         ///< Éxito
  OV7670_STATUS_ERR_MALLOC,     ///< Error de memoria (falló malloc)
  OV7670_STATUS_ERR_PERIPHERAL, ///< Periférico (ej. timer) no encontrado
} OV7670_status;

/**
 * @enum OV7670_colorspace
 * @brief Espacios de color soportados por la OV7670.
 */
typedef enum {
  OV7670_COLOR_RGB = 0, ///< RGB565 big-endian
  OV7670_COLOR_YUV,     ///< YUV/YCbCr 4:2:2 big-endian
} OV7670_colorspace;

/**
 * @enum OV7670_size
 * @brief Tamaños de frame soportados (división respecto a VGA).
 */
typedef enum {
  OV7670_SIZE_DIV1 = 0, ///< 640 x 480
  OV7670_SIZE_DIV2,     ///< 320 x 240
  OV7670_SIZE_DIV4,     ///< 160 x 120
  OV7670_SIZE_DIV8,     ///< 80 x 60
  OV7670_SIZE_DIV16,    ///< 40 x 30
} OV7670_size;

/**
 * @enum OV7670_pattern
 * @brief Patrones de prueba soportados.
 */
typedef enum {
  OV7670_TEST_PATTERN_NONE = 0,       ///< Desactiva patrón de prueba
  OV7670_TEST_PATTERN_SHIFTING_1,     ///< Patrón de "1 desplazando"
  OV7670_TEST_PATTERN_COLOR_BAR,      ///< Barras de color
  OV7670_TEST_PATTERN_COLOR_BAR_FADE, ///< Barras de color con fundido a blanco
} OV7670_pattern;

/**
 * @enum OV7670_night_mode
 * @brief Modos nocturnos para mejorar imagen en baja luz.
 */
typedef enum {
  OV7670_NIGHT_MODE_OFF = 0, ///< Modo nocturno desactivado
  OV7670_NIGHT_MODE_2,       ///< 1/2 frame rate
  OV7670_NIGHT_MODE_4,       ///< 1/4 frame rate
  OV7670_NIGHT_MODE_8,       ///< 1/8 frame rate
} OV7670_night_mode;

/**
 * @struct OV7670_pins
 * @brief Define la conexión física entre el microcontrolador y la cámara OV7670.
 *
 * Algunos pines pueden ser fijos según la arquitectura.
 * Consulta la documentación y ejemplos para asignación de valores.
 */
typedef struct {
  OV7670_pin enable;  ///< Pin de encendido/PWDN (-1 = GND fijo)
  OV7670_pin reset;   ///< Pin de reset (-1 = 3.3V fijo)
  OV7670_pin xclk;    ///< Clock externo para la cámara (entrada)
  OV7670_pin pclk;    ///< Clock de píxel (salida de cámara)
  OV7670_pin vsync;   ///< VSYNC (denominado también DEN1)
  OV7670_pin hsync;   ///< HSYNC (denominado también DEN2)
  OV7670_pin data[8]; ///< Pines de datos paralelos (D0-D7)
  OV7670_pin sda;     ///< Línea de datos I2C
  OV7670_pin scl;     ///< Línea de reloj I2C
} OV7670_pins;

/**
 * @struct OV7670_command
 * @brief Par (dirección, valor) para comandos SCCB/I2C de la cámara.
 */
typedef struct {
  uint8_t reg;   ///< Dirección de registro
  uint8_t value; ///< Valor a escribir
} OV7670_command;

/**
 * @struct OV7670_host
 * @brief Estructura compuesta para configuración de arquitectura y plataforma.
 */
typedef struct {
  OV7670_arch *arch; ///< Config. específica de arquitectura
  OV7670_pins *pins; ///< Conexión física de pines a la cámara
  void *platform;    ///< Datos específicos de la plataforma (ej: objeto C++ Arduino)
} OV7670_host;

/**
 * @def OV7670_ADDR
 * @brief Dirección I2C por defecto de la OV7670 (0x21).
 */
#define OV7670_ADDR 0x21

// --- Macros para acceso rápido a registros de la OV7670---
#define OV7670_ADDR 0x21                 /**< @brief Dirección I2C por defecto de la OV7670 */
#define OV7670_REG_GAIN 0x00             /**< @brief AGC gain bits 7:0 (9:8 en VREF) */
#define OV7670_REG_BLUE 0x01             /**< @brief AWB blue channel gain */
#define OV7670_REG_RED 0x02              /**< @brief AWB red channel gain */
#define OV7670_REG_VREF 0x03             /**< @brief Vertical frame control bits */
#define OV7670_REG_COM1 0x04             /**< @brief Common control 1 */
#define OV7670_COM1_R656 0x40            /**< @brief COM1 enable R656 format */
#define OV7670_REG_BAVE 0x05             /**< @brief U/B average level */
#define OV7670_REG_GbAVE 0x06            /**< @brief Y/Gb average level */
#define OV7670_REG_AECHH 0x07            /**< @brief Exposure value - AEC 15:10 bits */
#define OV7670_REG_RAVE 0x08             /**< @brief V/R average level */
#define OV7670_REG_COM2 0x09             /**< @brief Common control 2 */
#define OV7670_COM2_SSLEEP 0x10          /**< @brief COM2 soft sleep mode */
#define OV7670_REG_PID 0x0A              /**< @brief Product ID MSB (read-only) */
#define OV7670_REG_VER 0x0B              /**< @brief Product ID LSB (read-only) */
#define OV7670_REG_COM3 0x0C             /**< @brief Common control 3 */
#define OV7670_COM3_SWAP 0x40            /**< @brief COM3 output data MSB/LSB swap */
#define OV7670_COM3_SCALEEN 0x08         /**< @brief COM3 scale enable */
#define OV7670_COM3_DCWEN 0x04           /**< @brief COM3 DCW enable */
#define OV7670_REG_COM4 0x0D             /**< @brief Common control 4 */
#define OV7670_REG_COM5 0x0E             /**< @brief Common control 5 */
#define OV7670_REG_COM6 0x0F             /**< @brief Common control 6 */
#define OV7670_REG_AECH 0x10             /**< @brief Exposure value 9:2 */
#define OV7670_REG_CLKRC 0x11            /**< @brief Internal clock */
#define OV7670_CLK_EXT 0x40              /**< @brief CLKRC Use ext clock directly */
#define OV7670_CLK_SCALE 0x3F            /**< @brief CLKRC Int clock prescale mask */
#define OV7670_REG_COM7 0x12             /**< @brief Common control 7 */
#define OV7670_COM7_RESET 0x80           /**< @brief COM7 SCCB register reset */
#define OV7670_COM7_SIZE_MASK 0x38       /**< @brief COM7 output size mask */
#define OV7670_COM7_PIXEL_MASK 0x05      /**< @brief COM7 output pixel format mask */
#define OV7670_COM7_SIZE_VGA 0x00        /**< @brief COM7 output size VGA */
#define OV7670_COM7_SIZE_CIF 0x20        /**< @brief COM7 output size CIF */
#define OV7670_COM7_SIZE_QVGA 0x10       /**< @brief COM7 output size QVGA */
#define OV7670_COM7_SIZE_QCIF 0x08       /**< @brief COM7 output size QCIF */
#define OV7670_COM7_RGB 0x04             /**< @brief COM7 pixel format RGB */
#define OV7670_COM7_YUV 0x00             /**< @brief COM7 pixel format YUV */
#define OV7670_COM7_BAYER 0x01           /**< @brief COM7 pixel format Bayer RAW */
#define OV7670_COM7_PBAYER 0x05          /**< @brief COM7 pixel fmt proc Bayer RAW */
#define OV7670_COM7_COLORBAR 0x02        /**< @brief COM7 color bar enable */
#define OV7670_REG_COM8 0x13             /**< @brief Common control 8 */
#define OV7670_COM8_FASTAEC 0x80         /**< @brief COM8 Enable fast AGC/AEC algo */
#define OV7670_COM8_AECSTEP 0x40         /**< @brief COM8 AEC step size unlimited */
#define OV7670_COM8_BANDING 0x20         /**< @brief COM8 Banding filter enable */
#define OV7670_COM8_AGC 0x04             /**< @brief COM8 AGC (auto gain) enable */
#define OV7670_COM8_AWB 0x02             /**< @brief COM8 AWB (auto white balance) */
#define OV7670_COM8_AEC 0x01             /**< @brief COM8 AEC (auto exposure) enable */
#define OV7670_REG_COM9 0x14             /**< @brief Common control 9 - max AGC value */
#define OV7670_REG_COM10 0x15            /**< @brief Common control 10 */
#define OV7670_COM10_HSYNC 0x40          /**< @brief COM10 HREF changes to HSYNC */
#define OV7670_COM10_PCLK_HB 0x20        /**< @brief COM10 Suppress PCLK on hblank */
#define OV7670_COM10_HREF_REV 0x08       /**< @brief COM10 HREF reverse */
#define OV7670_COM10_VS_EDGE 0x04        /**< @brief COM10 VSYNC chg on PCLK rising */
#define OV7670_COM10_VS_NEG 0x02         /**< @brief COM10 VSYNC negative */
#define OV7670_COM10_HS_NEG 0x01         /**< @brief COM10 HSYNC negative */
#define OV7670_REG_HSTART 0x17           /**< @brief Horiz frame start high bits */
#define OV7670_REG_HSTOP 0x18            /**< @brief Horiz frame end high bits */
#define OV7670_REG_VSTART 0x19           /**< @brief Vert frame start high bits */
#define OV7670_REG_VSTOP 0x1A            /**< @brief Vert frame end high bits */
#define OV7670_REG_PSHFT 0x1B            /**< @brief Pixel delay select */
#define OV7670_REG_MIDH 0x1C             /**< @brief Manufacturer ID high byte */
#define OV7670_REG_MIDL 0x1D             /**< @brief Manufacturer ID low byte */
#define OV7670_REG_MVFP 0x1E             /**< @brief Mirror / vert-flip enable */
#define OV7670_MVFP_MIRROR 0x20          /**< @brief MVFP Mirror image */
#define OV7670_MVFP_VFLIP 0x10           /**< @brief MVFP Vertical flip */
#define OV7670_REG_LAEC 0x1F             /**< @brief Reserved */
#define OV7670_REG_ADCCTR0 0x20          /**< @brief ADC control */
#define OV7670_REG_ADCCTR1 0x21          /**< @brief Reserved */
#define OV7670_REG_ADCCTR2 0x22          /**< @brief Reserved */
#define OV7670_REG_ADCCTR3 0x23          /**< @brief Reserved */
#define OV7670_REG_AEW 0x24              /**< @brief AGC/AEC upper limit */
#define OV7670_REG_AEB 0x25              /**< @brief AGC/AEC lower limit */
#define OV7670_REG_VPT 0x26              /**< @brief AGC/AEC fast mode op region */
#define OV7670_REG_BBIAS 0x27            /**< @brief B channel signal output bias */
#define OV7670_REG_GbBIAS 0x28           /**< @brief Gb channel signal output bias */
#define OV7670_REG_EXHCH 0x2A            /**< @brief Dummy pixel insert MSB */
#define OV7670_REG_EXHCL 0x2B            /**< @brief Dummy pixel insert LSB */
#define OV7670_REG_RBIAS 0x2C            /**< @brief R channel signal output bias */
#define OV7670_REG_ADVFL 0x2D            /**< @brief Insert dummy lines MSB */
#define OV7670_REG_ADVFH 0x2E            /**< @brief Insert dummy lines LSB */
#define OV7670_REG_YAVE 0x2F             /**< @brief Y/G channel average value */
#define OV7670_REG_HSYST 0x30            /**< @brief HSYNC rising edge delay */
#define OV7670_REG_HSYEN 0x31            /**< @brief HSYNC falling edge delay */
#define OV7670_REG_HREF 0x32             /**< @brief HREF control */
#define OV7670_REG_CHLF 0x33             /**< @brief Array current control */
#define OV7670_REG_ARBLM 0x34            /**< @brief Array ref control - reserved */
#define OV7670_REG_ADC 0x37              /**< @brief ADC control - reserved */
#define OV7670_REG_ACOM 0x38             /**< @brief ADC & analog common - reserved */
#define OV7670_REG_OFON 0x39             /**< @brief ADC offset control - reserved */
#define OV7670_REG_TSLB 0x3A             /**< @brief Line buffer test option */
#define OV7670_TSLB_NEG 0x20             /**< @brief TSLB Negative image enable */
#define OV7670_TSLB_YLAST 0x04           /**< @brief TSLB UYVY or VYUY, see COM13 */
#define OV7670_TSLB_AOW 0x01             /**< @brief TSLB Auto output window */
#define OV7670_REG_COM11 0x3B            /**< @brief Common control 11 */
#define OV7670_COM11_NIGHT 0x80          /**< @brief COM11 Night mode */
#define OV7670_COM11_NMFR 0x60           /**< @brief COM11 Night mode frame rate mask */
#define OV7670_COM11_HZAUTO 0x10         /**< @brief COM11 Auto detect 50/60 Hz */
#define OV7670_COM11_BAND 0x08           /**< @brief COM11 Banding filter val select */
#define OV7670_COM11_EXP 0x02            /**< @brief COM11 Exposure timing control */
#define OV7670_REG_COM12 0x3C            /**< @brief Common control 12 */
#define OV7670_COM12_HREF 0x80           /**< @brief COM12 Always has HREF */
#define OV7670_REG_COM13 0x3D            /**< @brief Common control 13 */
#define OV7670_COM13_GAMMA 0x80          /**< @brief COM13 Gamma enable */
#define OV7670_COM13_UVSAT 0x40          /**< @brief COM13 UV saturation auto adj */
#define OV7670_COM13_UVSWAP 0x01         /**< @brief COM13 UV swap, use w TSLB[3] */
#define OV7670_REG_COM14 0x3E            /**< @brief Common control 14 */
#define OV7670_COM14_DCWEN 0x10          /**< @brief COM14 DCW & scaling PCLK enable */
#define OV7670_REG_EDGE 0x3F             /**< @brief Edge enhancement adjustment */
#define OV7670_REG_COM15 0x40            /**< @brief Common control 15 */
#define OV7670_COM15_RMASK 0xC0          /**< @brief COM15 Output range mask */
#define OV7670_COM15_R10F0 0x00          /**< @brief COM15 Output range 10 to F0 */
#define OV7670_COM15_R01FE 0x80          /**< @brief COM15 Output range 01 to FE */
#define OV7670_COM15_R00FF 0xC0          /**< @brief COM15 Output range 00 to FF */
#define OV7670_COM15_RGBMASK 0x30        /**< @brief COM15 RGB 555/565 option mask */
#define OV7670_COM15_RGB 0x00            /**< @brief COM15 Normal RGB out */
#define OV7670_COM15_RGB565 0x10         /**< @brief COM15 RGB 565 output */
#define OV7670_COM15_RGB555 0x30         /**< @brief COM15 RGB 555 output */
#define OV7670_REG_COM16 0x41            /**< @brief Common control 16 */
#define OV7670_COM16_AWBGAIN 0x08        /**< @brief COM16 AWB gain enable */
#define OV7670_REG_COM17 0x42            /**< @brief Common control 17 */
#define OV7670_COM17_AECWIN 0xC0         /**< @brief COM17 AEC window must match COM4 */
#define OV7670_COM17_CBAR 0x08           /**< @brief COM17 DSP Color bar enable */
#define OV7670_REG_AWBC1 0x43            /**< @brief Reserved */
#define OV7670_REG_AWBC2 0x44            /**< @brief Reserved */
#define OV7670_REG_AWBC3 0x45            /**< @brief Reserved */
#define OV7670_REG_AWBC4 0x46            /**< @brief Reserved */
#define OV7670_REG_AWBC5 0x47            /**< @brief Reserved */
#define OV7670_REG_AWBC6 0x48            /**< @brief Reserved */
#define OV7670_REG_REG4B 0x4B            /**< @brief UV average enable */
#define OV7670_REG_DNSTH 0x4C            /**< @brief De-noise strength */
#define OV7670_REG_MTX1 0x4F             /**< @brief Matrix coefficient 1 */
#define OV7670_REG_MTX2 0x50             /**< @brief Matrix coefficient 2 */
#define OV7670_REG_MTX3 0x51             /**< @brief Matrix coefficient 3 */
#define OV7670_REG_MTX4 0x52             /**< @brief Matrix coefficient 4 */
#define OV7670_REG_MTX5 0x53             /**< @brief Matrix coefficient 5 */
#define OV7670_REG_MTX6 0x54             /**< @brief Matrix coefficient 6 */
#define OV7670_REG_BRIGHT 0x55           /**< @brief Brightness control */
#define OV7670_REG_CONTRAS 0x56          /**< @brief Contrast control */
#define OV7670_REG_CONTRAS_CENTER 0x57   /**< @brief Contrast center */
#define OV7670_REG_MTXS 0x58             /**< @brief Matrix coefficient sign */
#define OV7670_REG_LCC1 0x62             /**< @brief Lens correction option 1 */
#define OV7670_REG_LCC2 0x63             /**< @brief Lens correction option 2 */
#define OV7670_REG_LCC3 0x64             /**< @brief Lens correction option 3 */
#define OV7670_REG_LCC4 0x65             /**< @brief Lens correction option 4 */
#define OV7670_REG_LCC5 0x66             /**< @brief Lens correction option 5 */
#define OV7670_REG_MANU 0x67             /**< @brief Manual U value */
#define OV7670_REG_MANV 0x68             /**< @brief Manual V value */
#define OV7670_REG_GFIX 0x69             /**< @brief Fix gain control */
#define OV7670_REG_GGAIN 0x6A            /**< @brief G channel AWB gain */
#define OV7670_REG_DBLV 0x6B             /**< @brief PLL & regulator control */
#define OV7670_REG_AWBCTR3 0x6C          /**< @brief AWB control 3 */
#define OV7670_REG_AWBCTR2 0x6D          /**< @brief AWB control 2 */
#define OV7670_REG_AWBCTR1 0x6E          /**< @brief AWB control 1 */
#define OV7670_REG_AWBCTR0 0x6F          /**< @brief AWB control 0 */
#define OV7670_REG_SCALING_XSC 0x70      /**< @brief Test pattern X scaling */
#define OV7670_REG_SCALING_YSC 0x71      /**< @brief Test pattern Y scaling */
#define OV7670_REG_SCALING_DCWCTR 0x72   /**< @brief DCW control */
#define OV7670_REG_SCALING_PCLK_DIV 0x73 /**< @brief DSP scale control clock divide */
#define OV7670_REG_REG74 0x74            /**< @brief Digital gain control */
#define OV7670_REG_REG76 0x76            /**< @brief Pixel correction */
#define OV7670_REG_SLOP 0x7A             /**< @brief Gamma curve highest seg slope */
#define OV7670_REG_GAM_BASE 0x7B         /**< @brief Gamma register base (1 of 15) */
#define OV7670_GAM_LEN 15                /**< @brief Number of gamma registers */
#define OV7670_R76_BLKPCOR 0x80          /**< @brief REG76 black pixel corr enable */
#define OV7670_R76_WHTPCOR 0x40          /**< @brief REG76 white pixel corr enable */
#define OV7670_REG_RGB444 0x8C           /**< @brief RGB 444 control */
#define OV7670_R444_ENABLE 0x02          /**< @brief RGB444 enable */
#define OV7670_R444_RGBX 0x01            /**< @brief RGB444 word format */
#define OV7670_REG_DM_LNL 0x92           /**< @brief Dummy line LSB */
#define OV7670_REG_LCC6 0x94             /**< @brief Lens correction option 6 */
#define OV7670_REG_LCC7 0x95             /**< @brief Lens correction option 7 */
#define OV7670_REG_HAECC1 0x9F           /**< @brief Histogram-based AEC/AGC ctrl 1 */
#define OV7670_REG_HAECC2 0xA0           /**< @brief Histogram-based AEC/AGC ctrl 2 */
#define OV7670_REG_SCALING_PCLK_DELAY 0xA2 /**< @brief Scaling pixel clock delay */
#define OV7670_REG_BD50MAX 0xA5          /**< @brief 50 Hz banding step limit */
#define OV7670_REG_HAECC3 0xA6           /**< @brief Histogram-based AEC/AGC ctrl 3 */
#define OV7670_REG_HAECC4 0xA7           /**< @brief Histogram-based AEC/AGC ctrl 4 */
#define OV7670_REG_HAECC5 0xA8           /**< @brief Histogram-based AEC/AGC ctrl 5 */
#define OV7670_REG_HAECC6 0xA9             /**< Histogram-based AEC/AGC ctrl 6 */
#define OV7670_REG_HAECC7 0xAA             /**< Histogram-based AEC/AGC ctrl 7 */
#define OV7670_REG_BD60MAX 0xAB            /**< 60 Hz banding step limit */
#define OV7670_REG_ABLC1 0xB1              /**< ABLC enable */
#define OV7670_REG_THL_ST 0xB3             /**< ABLC target */
#define OV7670_REG_SATCTR 0xC9             /**< Saturation control */
#define OV7670_REG_LAST 0xFF   /**< Última dirección válida de registro para listas de comandos */

// ------------------ FUNCIONES ACCESIBLES EN C/C++ -------------------

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa la cámara OV7670 con los parámetros dados.
 * @param host       Puntero a la estructura de host/configuración
 * @param colorspace Espacio de color a utilizar
 * @param size       Tamaño de frame
 * @param fps        Frame rate deseado (Hz)
 * @return Estado de la operación (OV7670_STATUS_OK, etc.)
 */
OV7670_status OV7670_begin(OV7670_host *host, OV7670_colorspace colorspace,
                           OV7670_size size, float fps);

/**
 * @brief Configura el formato de color de la cámara.
 * @param platform   Puntero a datos de plataforma
 * @param colorspace Espacio de color a configurar
 * @return Estado de la operación
 */
OV7670_status OV7670_set_format(void *platform, OV7670_colorspace colorspace);

/**
 * @brief Configura el frame rate de la cámara.
 * @param platform Puntero a datos de plataforma
 * @param fps      Frame rate deseado
 * @return Frame rate resultante (puede diferir del solicitado)
 */
float OV7670_set_fps(void *platform, float fps);

/**
 * @brief Configura la resolución (tamaño de frame) de la cámara.
 * @param platform Puntero a datos de plataforma
 * @param size     Tamaño deseado (ver OV7670_size)
 */
void OV7670_set_size(void *platform, OV7670_size size);

/**
 * @brief Permite manipulación avanzada de los registros de resolución.
 * @param platform     Puntero a datos de plataforma
 * @param size         Tamaño (divisor VGA)
 * @param vstart       Inicio vertical
 * @param hstart       Inicio horizontal
 * @param edge_offset  Offset de borde
 * @param pclk_delay   Retardo de pixel clock
 */
void OV7670_frame_control(void *platform, uint8_t size, uint16_t vstart,
                         uint16_t hstart, uint8_t edge_offset, uint8_t pclk_delay);

/**
 * @brief Configura el modo nocturno (mejora imagen baja luz).
 * @param platform   Puntero a datos de plataforma
 * @param night      Modo nocturno a configurar
 */
void OV7670_night(void *platform, OV7670_night_mode night);

/**
 * @brief Invierte la imagen (horizontal y/o vertical).
 * @param platform   Puntero a datos de plataforma
 * @param flip_x     Invierte horizontalmente si es true
 * @param flip_y     Invierte verticalmente si es true
 */
void OV7670_flip(void *platform, bool flip_x, bool flip_y);

/**
 * @brief Selecciona un patrón de prueba en la cámara.
 * @param platform   Puntero a datos de plataforma
 * @param pattern    Patrón a mostrar
 */
void OV7670_test_pattern(void *platform, OV7670_pattern pattern);

/**
 * @brief Convierte la componente Y de imagen YUV en RAM a formato RGB565.
 * @param ptr  Puntero a los datos a convertir (in-place)
 * @param len  Longitud en número de píxeles
 */
void OV7670_Y2RGB565(uint16_t *ptr, uint32_t len);

#ifdef __cplusplus
}
#endif
