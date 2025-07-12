/**
 * @file LCD.c
 * @brief Funciones para el manejo de la pantalla LCD basada en el controlador SSD1283A.
 */

#include "LCD.h"
#include <stdio.h>

#define PIN_DC   16  /**< Pin para Data/Command */
#define PIN_CS   17  /**< Pin para Chip Select */
#define PIN_SCK  18  /**< Pin para SPI Clock */
#define PIN_MOSI 19  /**< Pin para SPI MOSI */
#define PIN_RST  20  /**< Pin para Reset */
#define PIN_VCC  15  /**< Pin para VCC */
#define PIN_LED  22  /**< Pin para retroiluminación LED */

/**
 * @brief Selecciona el chip LCD (activo bajo).
 * @param host Puntero a la estructura SSD1283A_host
 */
static inline void cs_select(SSD1283A_host *host) {
    asm volatile("nop \n nop \n nop");
    gpio_put(host->pins->cs, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

/**
 * @brief Deselecciona el chip LCD.
 * @param host Puntero a la estructura SSD1283A_host
 */
static inline void cs_deselect(SSD1283A_host *host) {
    asm volatile("nop \n nop \n nop");
    gpio_put(host->pins->cs, 1);
    asm volatile("nop \n nop \n nop");
}

/**
 * @brief Configura el pin DC para indicar comando.
 * @param host Puntero a la estructura SSD1283A_host
 */
static inline void dc_command(SSD1283A_host *host) {
    asm volatile("nop \n nop \n nop");
    gpio_put(host->pins->dc, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

/**
 * @brief Configura el pin DC para indicar datos.
 * @param host Puntero a la estructura SSD1283A_host
 */
static inline void dc_data(SSD1283A_host *host) {
    asm volatile("nop \n nop \n nop");
    gpio_put(host->pins->dc, 1);
    asm volatile("nop \n nop \n nop");
}

/**
 * @brief Inicializa la estructura y los pines para la pantalla LCD.
 * @param lcd      Puntero a la estructura LCD
 * @param platform Puntero a la configuración de plataforma
 * @return Estado de inicialización (SSD1283A_STATUS_OK en éxito)
 */
SSD1283A_status lcd_init(struct LCD *lcd, struct lcd_platform_config *platform) {
    *lcd = (struct LCD){ 0 };

    gpio_init(PIN_VCC);
    gpio_set_dir(PIN_VCC, GPIO_OUT);
    gpio_put(PIN_VCC, 1); // Encender VCC

    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_put(PIN_RST, 1); // Mantener RST alto

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_put(PIN_LED, 1); // Encender LED de retroiluminación
    
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);  
    gpio_set_function(PIN_DC, GPIO_FUNC_SIO);

    static SSD1283A_pins pins = {
        .cs = PIN_CS,
        .dc = PIN_DC,
        .rst = PIN_RST,
        .led = PIN_LED,
    };

    lcd->driver_host = (SSD1283A_host){
        .pins = &pins,
        .platform = platform,
    };

    // Inicializa el controlador SSD1283A
    SSD1283A_status status = SSD1283A_begin(&lcd->driver_host);
    if (status != SSD1283A_STATUS_OK) {
        return status; // Error al inicializar el controlador
    }
    
    return SSD1283A_STATUS_OK; // Inicialización exitosa
}

/**
 * @brief Envía un comando al controlador SSD1283A por SPI.
 * @param host   Puntero a la estructura SSD1283A_host
 * @param pcfg   Puntero a la configuración de plataforma
 * @param command Comando a enviar
 */
void SSD1283A_write_command(SSD1283A_host *host, struct lcd_platform_config *pcfg, uint8_t command){
    cs_select(host);
    dc_command(host);
    pcfg->spi_write_blocking(pcfg->spi_handle, &command, 1);
    cs_deselect(host);
}

/**
 * @brief Envía un dato al controlador SSD1283A por SPI.
 * @param host   Puntero a la estructura SSD1283A_host
 * @param pcfg   Puntero a la configuración de plataforma
 * @param data   Dato a enviar
 */
void SSD1283A_write_data(SSD1283A_host *host, struct lcd_platform_config *pcfg, uint8_t data)
{
    cs_select(host);
    dc_data(host);
    pcfg->spi_write_blocking(pcfg->spi_handle, &data, 1);
    cs_deselect(host);
}

/**
 * @brief Escribe un valor de 16 bits en un registro del controlador SSD1283A.
 * @param host   Puntero a la estructura SSD1283A_host
 * @param pcfg   Puntero a la configuración de plataforma
 * @param reg    Dirección del registro
 * @param value  Valor de 16 bits a escribir
 */
void SSD1283A_write_register(SSD1283A_host *host, struct lcd_platform_config *pcfg, uint8_t reg, uint16_t value)
{
    cs_select(host);
    dc_command(host);
    pcfg->spi_write_blocking(pcfg->spi_handle, &reg, 1);
    dc_data(host);
    uint8_t val_buf[2] = { value >> 8, value & 0xFF };
    pcfg->spi_write_blocking(pcfg->spi_handle, val_buf, 2);
    cs_deselect(host);
}

/**
 * @brief Escribe un valor de color de 16 bits (RGB565) al LCD.
 * @param host   Puntero a la estructura SSD1283A_host
 * @param pcfg   Puntero a la configuración de plataforma
 * @param color  Valor de color (16 bits)
 */
void SSD1283A_write_color_16bit(SSD1283A_host *host, struct lcd_platform_config *pcfg, uint16_t color) {
    uint8_t buf[2] = { color >> 8, color & 0xFF };

    cs_select(host);
    dc_data(host);
    pcfg->spi_write_blocking(pcfg->spi_handle, buf, 2);
    cs_deselect(host);
}

/**
 * @brief Llena toda la pantalla LCD con un color específico.
 * @param lcd   Puntero a la estructura LCD
 * @param color Valor de color (RGB565)
 */
void lcd_fill_screen(struct LCD *lcd, uint16_t color) {
    uint16_t x_start = 0, x_end   = 135-1;
    uint16_t y_start = 0, y_end   = 135-1;

    struct lcd_platform_config *platform = (struct lcd_platform_config *)lcd->driver_host.platform;

    SSD1283A_write_register(&lcd->driver_host, platform, SSD1283A_CMD_HORIZONTAL_RAM_ADDR, (x_end << 8) | x_start);
    SSD1283A_write_register(&lcd->driver_host, platform, SSD1283A_CMD_VERTICAL_RAM_ADDR, (y_end << 8) | y_start);

    SSD1283A_write_command(&lcd->driver_host, platform, SSD1283A_CMD_SET_GDDRAM_XY);
    SSD1283A_write_data(&lcd->driver_host, platform, x_start); // X address
    SSD1283A_write_data(&lcd->driver_host, platform, y_start); // Y address

    SSD1283A_write_command(&lcd->driver_host, platform, SSD1283A_CMD_RAM_WRITE);

    for (uint32_t i = 0; i < (x_end - x_start) * (y_end - y_start); i++) {
        SSD1283A_write_color_16bit(&lcd->driver_host, platform, color);
    }
}

/**
 * @brief Muestra una imagen en la pantalla LCD a partir de un arreglo de colores.
 * @param lcd    Puntero a la estructura LCD
 * @param width  Ancho de la imagen
 * @param height Alto de la imagen
 * @param color  Arreglo de colores (RGB565)
 */
void lcd_show_image(struct LCD *lcd, uint16_t width, uint16_t height, uint16_t *color) {
    uint16_t x_start = 30, x_end   = width-1;
    uint16_t y_start = 30, y_end   = height-1;

    struct lcd_platform_config *platform = (struct lcd_platform_config *)lcd->driver_host.platform;

    SSD1283A_write_register(&lcd->driver_host, platform, SSD1283A_CMD_HORIZONTAL_RAM_ADDR, (x_end << 8) | x_start);
    SSD1283A_write_register(&lcd->driver_host, platform, SSD1283A_CMD_VERTICAL_RAM_ADDR, (y_end << 8) | y_start);

    SSD1283A_write_command(&lcd->driver_host, platform, SSD1283A_CMD_SET_GDDRAM_XY);
    SSD1283A_write_data(&lcd->driver_host, platform, x_start); // X address
    SSD1283A_write_data(&lcd->driver_host, platform, y_start); // Y address

    SSD1283A_write_command(&lcd->driver_host, platform, SSD1283A_CMD_RAM_WRITE);

    for (uint32_t i = 0; i < (x_end - x_start) * (y_end - y_start); i++) {
        SSD1283A_write_color_16bit(&lcd->driver_host, platform, color[i]);
    }
}
