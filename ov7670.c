/**
 * @file ov7670.c
 * @brief Implementación de funciones para configurar y controlar la cámara OV7670.
 * 
 * @copyright 2020 P Burgess for Adafruit Industries
 * @license MIT
 */

#include "camera/ov7670.h"

/**
 * @brief Función externa para imprimir cadenas de texto (debug).
 * 
 * Esta función debe ser implementada por el usuario.
 * @param str Cadena de texto a imprimir.
 */
extern void OV7670_print(char *str);

/**
 * @brief Función externa para leer un registro de la cámara OV7670 por I2C.
 * 
 * Esta función debe ser implementada por el usuario.
 * @param platform Puntero a la configuración de plataforma.
 * @param reg Dirección del registro a leer.
 * @return Valor leído del registro.
 */
extern int OV7670_read_register(void *platform, uint8_t reg);

/**
 * @brief Función externa para escribir un registro de la cámara OV7670 por I2C.
 * 
 * Esta función debe ser implementada por el usuario.
 * @param platform Puntero a la configuración de plataforma.
 * @param reg Dirección del registro a escribir.
 * @param value Valor a escribir en el registro.
 */
extern void OV7670_write_register(void *platform, uint8_t reg, uint8_t value);

/**
 * @brief Envía una lista de comandos terminada en 0xFF a la cámara OV7670.
 *
 * @param platform Puntero a datos de plataforma (ejemplo: objeto que referencia I2C).
 * @param cmd      Lista de comandos @ref OV7670_command, terminada en reg=0xFF.
 */
void OV7670_write_list(void *platform, OV7670_command *cmd) {
    for (int i = 0; cmd[i].reg <= OV7670_REG_LAST; i++) {
    #if 0 // DEBUG
        char buf[50];
        sprintf(buf, "Write reg %02X = %02X\n", cmd[i].reg, cmd[i].value);
        OV7670_print(buf);
    #endif
        OV7670_write_register(platform, cmd[i].reg, cmd[i].value);
        OV7670_delay_ms(1); // Required, else lockup on init
    }
}

/**
 * @brief Configuración predeterminada RGB para la cámara OV7670.
 */
static const OV7670_command OV7670_rgb[] = {
    {OV7670_REG_COM7, OV7670_COM7_RGB},
    {OV7670_REG_RGB444, 0},
    {OV7670_REG_COM15, OV7670_COM15_RGB565 | OV7670_COM15_R00FF},
    {0xFF, 0xFF}
};

/**
 * @brief Configuración predeterminada YUV para la cámara OV7670.
 */
static const OV7670_command OV7670_yuv[] = {
    {OV7670_REG_COM7, OV7670_COM7_YUV},
    {OV7670_REG_COM15, OV7670_COM15_R00FF},
    {0xFF, 0xFF}
};

/**
 * @brief Configuración inicial extensa de registros para la OV7670.
 */
static const OV7670_command OV7670_init[] = {
    {OV7670_REG_TSLB, OV7670_TSLB_YLAST}, // No auto window
    //{OV7670_REG_COM10, OV7670_COM10_VS_NEG}, // -VSYNC (req by SAMD PCC)
    {OV7670_REG_SLOP, 0x20},
    {OV7670_REG_GAM_BASE, 0x1C},
    {OV7670_REG_GAM_BASE + 1, 0x28},
    {OV7670_REG_GAM_BASE + 2, 0x3C},
    {OV7670_REG_GAM_BASE + 3, 0x55},
    {OV7670_REG_GAM_BASE + 4, 0x68},
    {OV7670_REG_GAM_BASE + 5, 0x76},
    {OV7670_REG_GAM_BASE + 6, 0x80},
    {OV7670_REG_GAM_BASE + 7, 0x88},
    {OV7670_REG_GAM_BASE + 8, 0x8F},
    {OV7670_REG_GAM_BASE + 9, 0x96},
    {OV7670_REG_GAM_BASE + 10, 0xA3},
    {OV7670_REG_GAM_BASE + 11, 0xAF},
    {OV7670_REG_GAM_BASE + 12, 0xC4},
    {OV7670_REG_GAM_BASE + 13, 0xD7},
    {OV7670_REG_GAM_BASE + 14, 0xE8},
    {OV7670_REG_COM8, OV7670_COM8_FASTAEC | OV7670_COM8_AECSTEP | OV7670_COM8_BANDING},
    {OV7670_REG_GAIN, 0x00},
    {OV7670_REG_COM2, 0x00},
    {OV7670_REG_COM4, 0x00},
    {OV7670_REG_COM9, 0x20}, // Max AGC value
    {OV7670_REG_COM11, (1 << 3)}, // 50Hz
    //{0x9D, 99}, // Banding filter for 50 Hz at 15.625 MHz
    {0x9D, 89}, // Banding filter for 50 Hz at 13.888 MHz
    {OV7670_REG_BD50MAX, 0x05},
    {OV7670_REG_BD60MAX, 0x07},
    {OV7670_REG_AEW, 0x75},
    {OV7670_REG_AEB, 0x63},
    {OV7670_REG_VPT, 0xA5},
    {OV7670_REG_HAECC1, 0x78},
    {OV7670_REG_HAECC2, 0x68},
    {0xA1, 0x03},              // Reserved register?
    {OV7670_REG_HAECC3, 0xDF}, // Histogram-based AEC/AGC setup
    {OV7670_REG_HAECC4, 0xDF},
    {OV7670_REG_HAECC5, 0xF0},
    {OV7670_REG_HAECC6, 0x90},
    {OV7670_REG_HAECC7, 0x94},
    {OV7670_REG_COM8, OV7670_COM8_FASTAEC | OV7670_COM8_AECSTEP |
        OV7670_COM8_BANDING | OV7670_COM8_AGC |
        OV7670_COM8_AEC | OV7670_COM8_AWB },
    {OV7670_REG_COM5, 0x61},
    {OV7670_REG_COM6, 0x4B},
    {0x16, 0x02},            // Reserved register?
    {OV7670_REG_MVFP, 0x07}, // 0x07,
    {OV7670_REG_ADCCTR1, 0x02},
    {OV7670_REG_ADCCTR2, 0x91},
    {0x29, 0x07}, // Reserved register?
    {OV7670_REG_CHLF, 0x0B},
    {0x35, 0x0B}, // Reserved register?
    {OV7670_REG_ADC, 0x1D},
    {OV7670_REG_ACOM, 0x71},
    {OV7670_REG_OFON, 0x2A},
    {OV7670_REG_COM12, 0x78},
    {0x4D, 0x40}, // Reserved register?
    {0x4E, 0x20}, // Reserved register?
    {OV7670_REG_GFIX, 0x5D},
    {OV7670_REG_REG74, 0x19},
    {0x8D, 0x4F}, // Reserved register?
    {0x8E, 0x00}, // Reserved register?
    {0x8F, 0x00}, // Reserved register?
    {0x90, 0x00}, // Reserved register?
    {0x91, 0x00}, // Reserved register?
    {OV7670_REG_DM_LNL, 0x00},
    {0x96, 0x00}, // Reserved register?
    {0x9A, 0x80}, // Reserved register?
    {0xB0, 0x84}, // Reserved register?
    {OV7670_REG_ABLC1, 0x0C},
    {0xB2, 0x0E}, // Reserved register?
    {OV7670_REG_THL_ST, 0x82},
    {0xB8, 0x0A}, // Reserved register?
    {OV7670_REG_AWBC1, 0x14},
    {OV7670_REG_AWBC2, 0xF0},
    {OV7670_REG_AWBC3, 0x34},
    {OV7670_REG_AWBC4, 0x58},
    {OV7670_REG_AWBC5, 0x28},
    {OV7670_REG_AWBC6, 0x3A},
    {0x59, 0x88}, // Reserved register?
    {0x5A, 0x88}, // Reserved register?
    {0x5B, 0x44}, // Reserved register?
    {0x5C, 0x67}, // Reserved register?
    {0x5D, 0x49}, // Reserved register?
    {0x5E, 0x0E}, // Reserved register?
    {OV7670_REG_LCC3, 0x04},
    {OV7670_REG_LCC4, 0x20},
    {OV7670_REG_LCC5, 0x05},
    {OV7670_REG_LCC6, 0x04},
    {OV7670_REG_LCC7, 0x08},
    {OV7670_REG_AWBCTR3, 0x0A},
    {OV7670_REG_AWBCTR2, 0x55},
    //{OV7670_REG_MTX1, 0x80},
    //{OV7670_REG_MTX2, 0x80},
    //{OV7670_REG_MTX3, 0x00},
    //{OV7670_REG_MTX4, 0x22},
    //{OV7670_REG_MTX5, 0x5E},
    //{OV7670_REG_MTX6, 0x80}, // 0x40?
    {OV7670_REG_AWBCTR1, 0x11},
    //{OV7670_REG_AWBCTR0, 0x9F}, // Or use 0x9E for advance AWB
    {OV7670_REG_AWBCTR0, 0x9E}, // Or use 0x9E for advance AWB
    {OV7670_REG_BRIGHT, 0x00},
    {OV7670_REG_CONTRAS, 0x40},
    {OV7670_REG_CONTRAS_CENTER, 0x80}, // 0x40?
    {OV7670_REG_LAST + 1, 0x00},       // End-of-data marker
};

/**
 * @brief Inicializa la cámara OV7670.
 *
 * @param host       Puntero a estructura de configuración.
 * @param colorspace Espacio de color a usar.
 * @param size       Tamaño de imagen.
 * @param fps        Frame rate deseado.
 * @return Estado de la operación.
 */
OV7670_status OV7670_begin(OV7670_host *host, OV7670_colorspace colorspace,
                           OV7670_size size, float fps) {
    OV7670_status status;

    // I2C must already be set up and running (@ 100 KHz) in calling code

    // Do device-specific (but platform-agnostic) setup. e.g. on SAMD this
    // function will fiddle registers to start a timer for XCLK output and
    // enable the parallel capture peripheral.
    /*
    status = OV7670_arch_begin(host);
    if (status != OV7670_STATUS_OK) {
        return status;
    }
    */

    // Unsure of camera startup time from beginning of input clock.
    // Let's guess it's similar to tS:REG (300 ms) from datasheet.
    OV7670_delay_ms(300);

    // ENABLE AND/OR RESET CAMERA --------------------------------------------
    if (host->pins->enable >= 0) { // Enable pin defined?
        OV7670_pin_output(host->pins->enable);
        OV7670_pin_write(host->pins->enable, 0); // PWDN low (enable)
        OV7670_delay_ms(300);
    }

    if (host->pins->reset >= 0) { // Hard reset pin defined?
        OV7670_pin_output(host->pins->reset);
        OV7670_pin_write(host->pins->reset, 0);
        OV7670_delay_ms(1);
        OV7670_pin_write(host->pins->reset, 1);
    } else { // Soft reset, doesn't seem reliable, might just need more delay?
        OV7670_write_register(host->platform, OV7670_REG_COM7, OV7670_COM7_RESET);
    }
    OV7670_delay_ms(1000); // Datasheet: tS:RESET = 1 ms

    //(void)OV7670_set_fps(host->platform, fps); // Timing
    OV7670_write_register(host->platform, OV7670_REG_CLKRC, 1); // CLK * 4
    OV7670_write_register(host->platform, OV7670_REG_DBLV, 1 << 6); // CLK / 4
    OV7670_set_format(host->platform, colorspace);
    OV7670_write_list(host->platform, OV7670_init); // Other config
    OV7670_set_size(host->platform, size);          // Frame size

    OV7670_delay_ms(300); // tS:REG = 300 ms (settling time = 10 frames)

    return OV7670_STATUS_OK;
}

/**
 * @brief Configura el formato de color de la cámara.
 * 
 * @param platform   Puntero a la plataforma.
 * @param colorspace Espacio de color.
 * @return Estado de la operación.
 */
OV7670_status OV7670_set_format(void *platform, OV7670_colorspace colorspace) {
    if (colorspace == OV7670_COLOR_RGB) {
        OV7670_write_list(platform, OV7670_rgb);
    } else {
        OV7670_write_list(platform, OV7670_yuv);
    }
    return OV7670_STATUS_OK;
}

/**
 * @brief Configura el frame rate de la cámara.
 * 
 * @param platform Puntero a datos de plataforma.
 * @param fps      Frame rate deseado.
 * @return         Frame rate real resultante.
 */
float OV7670_set_fps(void *platform, float fps) {
    static const uint8_t pll_ratio[] = {1, 4, 6, 8};
    const uint8_t num_plls = sizeof pll_ratio / sizeof pll_ratio[0];

    fps = (fps > 30) ? 30 : fps;               // Max 30 FPS
    float pclk_target = fps * 4000000.0 / 5.0; // Ideal PCLK Hz for target FPS
    uint32_t pclk_min = OV7670_XCLK_HZ / 32;   // Min PCLK determines min FPS
    if (pclk_target < (float)pclk_min) {
        if (platform) {
            OV7670_write_register(platform, OV7670_REG_DBLV, 0);   // 1:1 PLL
            OV7670_write_register(platform, OV7670_REG_CLKRC, 31); // 1/32 div
        }
        return (float)(pclk_min * 5 / 4000000); // Return min frame rate
    }

    uint8_t best_pll = 0;
    uint8_t best_div = 1;
    float best_delta = 30.0;

    for (uint8_t p = 0; p < num_plls; p++) {
        uint32_t xclk_pll = OV7670_XCLK_HZ * pll_ratio[p];
        uint8_t first_div = p ? 2 : 1;
        for (uint8_t div = first_div; div <= 32; div++) {
            uint32_t pclk_result = xclk_pll / div;
            if (pclk_result > pclk_target) {
                continue;
            }
            float fps_result = (float)pclk_result * 5.0 / 4000000.0;
            float delta = fps - fps_result;
            if (delta < best_delta) {
                best_delta = delta;
                best_pll = p;
                best_div = div;
            }
        }
    }

    if (platform) {
        if (pll_ratio[best_pll] == best_div) {
            OV7670_write_register(platform, OV7670_REG_DBLV, 0);
            OV7670_write_register(platform, OV7670_REG_CLKRC, 0x40);
        } else {
            OV7670_write_register(platform, OV7670_REG_DBLV, best_pll << 6);
            OV7670_write_register(platform, OV7670_REG_CLKRC, best_div - 1);
        }
    }

    return fps - best_delta;
}

/**
 * @brief Control avanzado de ventana y divisores de PCLK.
 * 
 * @param platform     Puntero a datos de plataforma.
 * @param size         Tamaño (divisor VGA).
 * @param vstart       Inicio vertical.
 * @param hstart       Inicio horizontal.
 * @param edge_offset  Offset de borde.
 * @param pclk_delay   Retardo de pixel clock.
 */
void OV7670_frame_control(void *platform, uint8_t size, uint8_t vstart,
                          uint16_t hstart, uint8_t edge_offset,
                          uint8_t pclk_delay) {
    uint8_t value;

    value = (size > OV7670_SIZE_DIV1) ? OV7670_COM3_DCWEN : 0;
    if (size == OV7670_SIZE_DIV16)
        value |= OV7670_COM3_SCALEEN;
    OV7670_write_register(platform, OV7670_REG_COM3, value);

    value = (size > OV7670_SIZE_DIV1) ? (0x18 + size) : 0;
    OV7670_write_register(platform, OV7670_REG_COM14, value);

    value = (size <= OV7670_SIZE_DIV8) ? size : OV7670_SIZE_DIV8;
    OV7670_write_register(platform, OV7670_REG_SCALING_DCWCTR, value * 0x11);

    value = (size > OV7670_SIZE_DIV1) ? (0xF0 + size) : 0x08;
    OV7670_write_register(platform, OV7670_REG_SCALING_PCLK_DIV, value);

    value = (size == OV7670_SIZE_DIV16) ? 0x40 : 0x20;
    uint8_t xsc = OV7670_read_register(platform, OV7670_REG_SCALING_XSC);
    uint8_t ysc = OV7670_read_register(platform, OV7670_REG_SCALING_YSC);
    xsc = (xsc & 0x80) | value;
    ysc = (ysc & 0x80) | value;
    OV7670_write_register(platform, OV7670_REG_SCALING_XSC, xsc);
    OV7670_write_register(platform, OV7670_REG_SCALING_YSC, ysc);

    uint16_t vstop = vstart + 480;
    uint16_t hstop = (hstart + 640) % 784;
    OV7670_write_register(platform, OV7670_REG_HSTART, hstart >> 3);
    OV7670_write_register(platform, OV7670_REG_HSTOP, hstop >> 3);
    OV7670_write_register(platform, OV7670_REG_HREF,
        (edge_offset << 6) | ((hstop & 0b111) << 3) |
        (hstart & 0b111));
    OV7670_write_register(platform, OV7670_REG_VSTART, vstart >> 2);
    OV7670_write_register(platform, OV7670_REG_VSTOP, vstop >> 2);
    OV7670_write_register(platform, OV7670_REG_VREF,
        ((vstop & 0b11) << 2) | (vstart & 0b11));

    OV7670_write_register(platform, OV7670_REG_SCALING_PCLK_DELAY, pclk_delay);
}

/**
 * @brief Configura la resolución (tamaño de frame) de la cámara.
 * 
 * @param platform Puntero a la plataforma.
 * @param size     Tamaño deseado.
 */
void OV7670_set_size(void *platform, OV7670_size size) {
    static struct {
        uint8_t vstart;
        uint8_t hstart;
        uint8_t edge_offset;
        uint8_t pclk_delay;
    } window[] = {
        {9, 162, 2, 2},  // SIZE_DIV1  640x480 VGA
        {10, 174, 4, 2}, // SIZE_DIV2  320x240 QVGA
        {11, 186, 2, 2}, // SIZE_DIV4  160x120 QQVGA
        {12, 210, 0, 2}, // SIZE_DIV8  80x60   ...
        {15, 252, 3, 2}, // SIZE_DIV16 40x30
    };

    OV7670_frame_control(platform, size, window[size].vstart, window[size].hstart,
        window[size].edge_offset, window[size].pclk_delay);
}

/**
 * @brief Selecciona el modo nocturno de la cámara.
 * 
 * @param platform   Puntero a datos de plataforma.
 * @param night      Modo nocturno a configurar.
 */
void OV7670_night(void *platform, OV7670_night_mode night) {
    static const uint8_t night_bits[] = {0b00000000, 0b10100000, 0b11000000, 0b11100000};
    uint8_t com11 = OV7670_read_register(platform, OV7670_REG_COM11);
    com11 &= 0b00011111;
    com11 |= night_bits[night];
    OV7670_write_register(platform, OV7670_REG_COM11, com11);
}

/**
 * @brief Invierte la imagen de salida (horizontal y/o vertical).
 * 
 * @param platform Puntero a plataforma.
 * @param flip_x   Invierte horizontalmente si es true.
 * @param flip_y   Invierte verticalmente si es true.
 */
void OV7670_flip(void *platform, bool flip_x, bool flip_y) {
    uint8_t mvfp = OV7670_read_register(platform, OV7670_REG_MVFP);
    if (flip_x) {
        mvfp |= OV7670_MVFP_MIRROR;
    } else {
        mvfp &= ~OV7670_MVFP_MIRROR;
    }
    if (flip_y) {
        mvfp |= OV7670_MVFP_VFLIP;
    } else {
        mvfp &= ~OV7670_MVFP_VFLIP;
    }
    OV7670_write_register(platform, OV7670_REG_MVFP, mvfp);
}

/**
 * @brief Selecciona el patrón de prueba de la cámara OV7670.
 * 
 * @param platform Puntero a datos de plataforma.
 * @param pattern  Patrón a configurar.
 */
void OV7670_test_pattern(void *platform, OV7670_pattern pattern) {
    uint8_t xsc = OV7670_read_register(platform, OV7670_REG_SCALING_XSC);
    uint8_t ysc = OV7670_read_register(platform, OV7670_REG_SCALING_YSC);
    if (pattern & 1) {
        xsc |= 0x80;
    } else {
        xsc &= ~0x80;
    }
    if (pattern & 2) {
        ysc |= 0x80;
    } else {
        ysc &= ~0x80;
    }
    OV7670_write_register(platform, OV7670_REG_SCALING_XSC, xsc);
    OV7670_write_register(platform, OV7670_REG_SCALING_YSC, ysc);
}

/**
 * @brief Convierte el componente Y (luminancia) de YUV a RGB565 (big endian).
 * 
 * @param ptr  Puntero a los datos (in-place).
 * @param len  Longitud en número de píxeles.
 */
void OV7670_Y2RGB565(uint16_t *ptr, uint32_t len) {
    while (len--) {
        uint8_t y = *ptr & 0xFF;
        uint16_t rgb = ((y >> 3) * 0x801) | ((y & 0xFC) << 3);
        *ptr++ = __builtin_bswap16(rgb);
    }
}
