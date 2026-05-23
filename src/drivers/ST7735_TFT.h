
#ifndef ST7735_TFT_H
#define ST7735_TFT_H

#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// To Do List:
// Adapt for all screen tab types
// and insert adafruit commands for each one ig

// ST7735 Defines Part1 (Red / Green Tab)
#define ST77XX_SWRESET              0x01


#define ST77XX_SLPOUT               0x11


#define ST7735_FRMCTR1              0xB1 // Frame Rate Control
#define ST7735_FRMCTR2              0xB2
#define ST7735_FRMCTR3              0xB3

#define ST7735_INVCTR               0xB4

#define ST7735_PWCTR1               0xC0
#define ST7735_PWCTR2               0xC1
#define ST7735_PWCTR3               0xC2
#define ST7735_PWCTR4               0xC3
#define ST7735_PWCTR5               0xC4

#define ST7735_VMCTR1               0xC5

#define ST77XX_INVOFF               0x20  
#define ST77XX_MADCTL               0x36

#define ST77XX_COLMOD               0x3A            // Define color format (12-bit/16-bit/18-bit/none)

// ST7735 Defines Part2 (Green Tab) (check back for resolutions or smth)
#define ST77XX_CASET                0x2A            // set address of each column (pixel column?)
#define ST77XX_RASET                0x2B            // set address of each row (pixel row?)
#define ST77XX_RAMWR                0x2C


//ST7735 Defines Part3 (Red / Green Tab)
#define ST7735_GMCTRP1              0xE0            // Gamma Polarity (+)
#define ST7735_GMCTRN1              0xE1            // Gamma Polarity (-)
#define ST77XX_NORON                0x13            // Normal Display Mode (On or Off)
#define ST77XX_DISPON               0x29            // Display On

// Color Code Definitions

#define ST7735_BLACK                0x0000
#define ST7735_WHITE                0xFFFF
#define ST7735_RED                  0xF800
#define ST7735_GREEN                0x07E0
#define ST7735_BLUE                 0x001F
#define ST7735_YELLOW               0xFFE0
#define ST7735_CYAN                 0x07FF
#define ST7735_MAGENTA              0xF81F
#define ST7735_GRAY_LIGHT           0xC618  // ~192/255 brightness
#define ST7735_GRAY_MEDIUM          0x8410  // ~128/255 brightness
#define ST7735_GRAY_DARK            0x4208  // ~64/255 brightness

// Screen Rotation Definitions
#define ROTATION_0                  0x00
#define ROTATION_90                 0x60
#define ROTATION_180                0xC0
#define ROTATION_270                0xA0

// Icon Definitions
#define ICON_WEATHER_SUN            0x00
#define ICON_WEATHER_CLOUD          0x01
#define ICON_WEATHER_RAIN           0x02



// Pin are ports (defined in makefile typically?)

// SPI Port
#ifndef SPI_ST7735_PORT
    #define SPI_ST7735_PORT spi0
#endif

// Chip-Select Output Pin
#ifndef SPI_ST7735_CS
    #define SPI_ST7735_CS                   19
#endif

// ST7735 DC/RS Output Pin
#ifndef SPI_ST7735_RS
    #define SPI_ST7735_RS                   18
#endif

// Reset Output Pin
#ifndef SPI_ST7735_RST
    #define SPI_ST7735_RST                  17
#endif

#ifndef SPI_ST7735_LEDA
    #define SPI_ST7735_LEDA                 20
#endif

#ifndef SPI_ST7735_MOSI                                // unsure if needs to be defined
    #define SPI_ST7735_MOSI                 3
#endif

#ifndef SPI_ST7735_SCK                                 // unsure if needs to be defined
    #define SPI_ST7735_SCK                  2
#endif

#ifndef ST7735_SW_SCREENS
    #define ST7735_SW_SCREENS               14
#endif

extern uint8_t disp_width, disp_height;

typedef struct {
    uint cs_pin;
    uint rs_pin;
    uint rst_pin;
    spi_inst_t *spi_port;
} st7735_pin_config_t;

/********************** FUNCTION PROTOTYPES ***************/
// Some functions taken from: https://github.com/bablokb/pico-st7735/ 

static inline void tft_cs_low() {                       // Timing found at Pg25 (unsure about exact # of NOP uses)
    asm volatile("nop \n nop \n nop"); \
    gpio_put(SPI_ST7735_CS, 0);
    asm volatile("nop \n nop \n nop"); \
    asm volatile("nop \n nop \n nop"); \
}

static inline void tft_cs_high() {
    asm volatile("nop \n nop \n nop"); \
    gpio_put(SPI_ST7735_CS, 1);
    asm volatile("nop \n nop \n nop"); \
}

static inline void tft_rs_low() {
    asm volatile("nop \n nop \n nop"); \
    gpio_put(SPI_ST7735_RS, 0);
    asm volatile("nop \n nop \n nop"); \
}

static inline void tft_rs_high() {
    asm volatile("nop \n nop \n nop"); \
    gpio_put(SPI_ST7735_RS, 1);
    asm volatile("nop \n nop \n nop"); \
}

static inline void spi_write(uint8_t data) {
    spi_write_blocking(SPI_ST7735_PORT, &data, 1);
}

// For SPI:
void write_command(uint8_t cmd);
void write_data(uint8_t data);
// Declarations / Prototyping
void st7735_spi_init();


/*
    * @brief Initializes the ST7735 Display
    * 
*/
void st7735_init_display();


/*
    * @brief Sets rotation of the screen by checking MV bit for axis
    * @param value for madctl data
*/
void set_rotation(uint8_t madctl);


/*
    * @brief Set Area on screen that can be drawn to
    * @param Coordinates of point 0 and point 1
    * 
*/
void set_AddrArea(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);


/*
    * @brief Create and fill a rectangle 
    * @param (x,y) position for top left of rectangle, then how wide and tall it is along w/ color to fill
    * 
*/
void fill_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color);


/*
    * @brief Fills entire screen with one color
    * @param Color formatted in two bytes in R-G-B (5-6-5)
    * 
*/
void fill_screen(uint16_t color);

/*
    * @brief draws a specified string in a 5x7 ascii font
    * @param (x,y) position for top left of character, the character itself, char color, bg color, and font size
*/
void draw_string(uint8_t x, uint8_t y, const char* str, uint16_t font_color, uint16_t bg_color, uint8_t size);


void draw_bitmap(uint8_t x, uint8_t y, uint8_t icon, uint16_t font_color, uint16_t bg_color, uint8_t size);


/*
    * @brief draws a specified char in a 5x7 ascii font
    * @param (x,y) position for top left of character, the character itself, char color, bg color, and font size
*/
void draw_char(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

// int draw_char(uint8_t x, uint8_t y, char c, uint16_t color, uint16_t bg, uint8_t size);

void change_framerate();

/*
    * @brief Draw a singular pixel
    * @param (x,y) position for top left of rectangle, then how wide and tall it is along w/ color to fill
    * 
*/
void draw_pixel(uint8_t x, uint8_t y, uint16_t color);


void draw_line();

#if defined ST7735_GREEN_TAB
//
//
#endif

// RED_TAB


// BLACK_TAB 


// BLUE_TAB



#endif 