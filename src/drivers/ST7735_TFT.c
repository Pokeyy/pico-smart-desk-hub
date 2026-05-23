#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "ST7735_TFT.h"
#include "fonts.h"

// To Do List:
// Understand timings still for each pin change of SPI
// Understand power commands maybe
// Make sure that screen works with ST7735.c and the other files
// Update CMake to work with new files
// If not, just copy and paste into og files

uint8_t col_start = 0, row_start = 0, x_start = 0, y_start = 0;

uint8_t tft_width, tft_height;

st7735_pin_config_t interface_pins = {
    .cs_pin = SPI_ST7735_CS,
    .rs_pin = SPI_ST7735_RS,
    .rst_pin = SPI_ST7735_RST,
    .spi_port = SPI_ST7735_PORT
};

void st7735_spi_init() {
    spi_init(spi0, 20*1000*1000);
    gpio_init(SPI_ST7735_CS);
    gpio_set_dir(SPI_ST7735_CS, GPIO_OUT);
    gpio_put(SPI_ST7735_CS, 1);                 // Chip select is active-low, so keep high when unselected

    gpio_init(SPI_ST7735_RS);
    gpio_set_dir(SPI_ST7735_RS, GPIO_OUT);
    gpio_put(SPI_ST7735_RS, 0);

    gpio_init(SPI_ST7735_RST);
    gpio_set_dir(SPI_ST7735_RST, GPIO_OUT);
    gpio_put(SPI_ST7735_RST, 0);

    gpio_init(SPI_ST7735_LEDA);
    gpio_set_dir(SPI_ST7735_LEDA, GPIO_OUT);
    gpio_put(SPI_ST7735_LEDA, 1);

    gpio_init(SPI_ST7735_SCK);
    gpio_set_function(SPI_ST7735_SCK, GPIO_FUNC_SPI);

    gpio_init(SPI_ST7735_MOSI);
    gpio_set_function(SPI_ST7735_MOSI, GPIO_FUNC_SPI);
}

void write_command(uint8_t cmd) {
    tft_rs_low();
    tft_cs_low();
    spi_write(cmd);
    tft_cs_high();
}


void write_data(uint8_t data) {
    tft_rs_high();
    tft_cs_low();
    spi_write(data);
    tft_cs_high();
}

// add multi-byte data func
void write_data_buffer(uint8_t* data, size_t len) {
    tft_rs_high();
    tft_cs_low();
    spi_write_blocking(SPI_ST7735_PORT, data, len);
    tft_cs_high();
}


void set_AddrArea(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    write_command(ST77XX_CASET);
    write_data(0); write_data(x0 + x_start);            // unsure about x_start, maybe for offset or some other func?
    write_data(0); write_data(x1 + x_start);

    write_command(ST77XX_RASET);
    write_data(0); write_data(y0 + y_start);            // unsure about y_start, maybe for offset or some other func?
    write_data(0); write_data(y1 + y_start);

    write_command(ST77XX_RAMWR);
}

void fill_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color) {
    uint8_t upper, lower;
    if( (x >= tft_width) || (y >= tft_height) )
        return;
    if( (x + width - 1) >= tft_width)
        width = tft_width - x;
    if( (y + height - 1) >= tft_height)
        height = tft_height - y;
    set_AddrArea(x, y, x+width-1, y+height-1);
    upper = color >> 8; lower = color;
    tft_rs_high();                                      // not using spi_write instantly to not spam loop pin changes
    tft_cs_low();
    for(y = height; y > 0; y--) {
        for (x = width; x > 0; x--) {
            spi_write(upper);
            spi_write(lower);
        }
    }
    tft_cs_high();
}

void fill_screen(uint16_t color) {
    fill_rectangle(0, 0, tft_width, tft_height, color);
}

void draw_pixel(uint8_t x, uint8_t y, uint16_t color) {
    if((x >= tft_width) || (y >= tft_height)) 
        return;
    set_AddrArea(x, y, x, y);
    write_data(color >> 8);
    write_data(color);
}

void draw_bitmap(uint8_t x, uint8_t y, uint8_t icon, uint16_t font_color, uint16_t bg_color, uint8_t size) {
    if (size < 1) size = 1;
    if (x >= tft_width || y >= tft_height) return;

    const uint8_t *iconData = icons_weather[icon];
    
    for (uint8_t col = 0; col < 8; col++) {
        uint8_t line = iconData[col];
        for(uint8_t row = 0; row < 8; row++) {
            bool pixelOn = line & 0x01; // LSB-top
            line >>= 1;

            // rotate 90° clockwise
            int drawX = x + (7 - row) * size;
            int drawY = y + col * size;

            // Optional row debug:
            // printf("    Row %d: %d\n", row, pixelOn);

            if (pixelOn) {
                if (size == 1) draw_pixel(drawX, drawY, font_color);
                else fill_rectangle(drawX, drawY, size, size, font_color);
            } 
            else if (bg_color != font_color) {
                if (size == 1) draw_pixel(drawX, drawY, bg_color);
                else fill_rectangle(drawX, drawY, size, size, bg_color);
            }
        }
    }

}

void draw_string(uint8_t x, uint8_t y, const char* str, uint16_t font_color, uint16_t bg_color, uint8_t size) {
    if (size < 1) 
        size = 1;

    uint8_t x_start = x;

    while(str[0]) { // while loop until the character at the index is not NULL terminator / '0'
        if (str[0] == ' ') {
            x += 4;
        }
        else if (str[0] == '\n') {
            x = x_start;
            y += 8 * size; // 7 pixels for height + 1 pixel spacing
        }
        else {
            if (x + 5 * size < tft_width && y + 7 * size < tft_height)
                draw_char(x, y, str[0], font_color, bg_color, size);
            x += 6 * size; // 5 pixels for height + 1 pixel spacing
        }
     str++;   
    }
}

void draw_char(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (size < 1) size = 1;

    if (c < ' ' || c > '~') c = '?';
    if (x >= tft_width || y >= tft_height) return;

    // printf("Drawing char '%c' (0x%02X) at index %d\n", c, c, c - ' ');           Debugging

    const uint8_t *charData = font5x7[c - ' ']; // fetch character row data

    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = charData[col];
        // printf("  Col %d: 0x%02X\n", col, line);                                 Debugging

        for (uint8_t row = 0; row < 7; row++) {
            bool pixelOn = line & 0x01; // LSB-top
            line >>= 1;

            int drawX = x + col * size;  // column → X
            int drawY = y + row * size;  // row → Y

            // Optional row debug:
            // printf("    Row %d: %d\n", row, pixelOn);

            if (pixelOn) {
                if (size == 1) draw_pixel(drawX, drawY, color);
                else fill_rectangle(drawX, drawY, size, size, color);
            } 
            else if (bg != color) {
                if (size == 1) draw_pixel(drawX, drawY, bg);
                else fill_rectangle(drawX, drawY, size, size, bg);
            }
        }
    }
}

void set_rotation(uint8_t madctl) {
    write_command(ST77XX_MADCTL);
    write_data(madctl);

    if (madctl & 0x20) {        
        tft_width = 160, tft_height = 128;
    }
    else {
        tft_width = 128, tft_height = 160;
    }

}


void st7735_init_display() {
    sleep_ms(50);
    gpio_put(SPI_ST7735_RST, 1);
    sleep_ms(50);

    // Step 1, Reset it 
    write_command(ST77XX_SWRESET);
    sleep_ms(120);

    // Wake display up
    write_command(ST77XX_SLPOUT);
    sleep_ms(120);

    // Define Pixel Format (COLMOD)  - 12/16/18 bit
    write_command(ST77XX_COLMOD);
    write_data(0x05);                 // 16-bit

    // // MADCTL (define reading or writing each frame scan)
    // write_command(ST77XX_MADCTL);
    // // write_data(0xC0);                   // bits 0-4 just keep 0 for default, bits 5-7 indicate inversion, like if your first pixel starts opposite side
    //                                                      // (MX, MY, MV = 0) (was 0x00 before but adafruit uses 0xC0)
    // write_data(0xC0);

    // Normal Display Mode ON
    write_command(ST77XX_NORON);
    sleep_ms(10);

    // Turn Display On
    write_command(ST77XX_DISPON);
    sleep_ms(100);
}
