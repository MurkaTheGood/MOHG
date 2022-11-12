#ifndef __SSD1306_H
#define __SSD1306_H

#include <stdint.h>

// Address of the display on I2C bus
#define __SSD1306_ADDRESS	0x3C

// resolution of the display
#define __SSD1306_WIDTH 	128
#define __SSD1306_HEIGHT 	32

// colors
#define SSD1306_WHITE 1
#define SSD1306_BLACK 0

// commands
#define __SSD1306_CMD__Display_On							0xAF
#define __SSD1306_CMD__Display_Off							0xAE
#define __SSD1306_CMD__Contrast_Set							0x81
#define __SSD1306_CMD__Display_All_On_Resume				0xA4
#define __SSD1306_CMD__Display_All_On						0xA5
#define __SSD1306_CMD__Display_Normal						0xA4
#define __SSD1306_CMD__Display_Inverse						0xA3

#define __SSD1306_CMD__Page_Addressing_Column_Lower_Set		0x00
#define __SSD1306_CMD__Page_Addressing_Column_Higher_Set	0x10
#define __SSD1306_CMD__Page_Addressing_Page_Start_Set		0xB0
#define __SSD1306_CMD__Page_Address_Set						0x22
#define __SSD1306_CMD__Memory_Addressing_Set				0x20
#define __SSD1306_CMD__Column_Address_Set					0x21

#define __SSD1306_CMD__Display_Start_Line_Set				0x40
#define __SSD1306_CMD__Display_Offset_Set					0xD3
#define __SSD1306_CMD__Segment_Remap_Set					0xA0
#define __SSD1306_CMD__Multiplex_Radio_Set					0xA8
#define __SSD1306_CMD__Com_Output_Scan_Inc					0xC0
#define __SSD1306_CMD__Com_Output_Scan_Dec					0xC8
#define __SSD1306_CMD__Com_Pins_Set							0xDA

#define __SSD1306_CMD__Display_Clock_Div_Ratio_Set			0xD5
#define __SSD1306_CMD__Display_Oscillator_Frequency_Set		0xD5
#define __SSD1306_CMD__Precharge_Period_Set					0xD9
#define __SSD1306_CMD__VCOMH_Deselect_Level_Set				0xDB
#define __SSD1306_CMD__Nop									0xE3

#define __SSD1306_CMD__Charge_Pump_Set						0x8D


/*
 * FUNCTIONS
 */

// prepare the display for work
void SSD1306_setup(void);

// send a single command without arguments to display
// if issue_start is 1 then the START and STOP conditions will be sent
void SSD1306_send_command(uint8_t command, int issue_start);

// send framebuffer to display
void SSD1306_render(void);


/*
 * GRAPHICS FUNCTIONS
 */

// fill the screen
void SSD1306_graphics_fill(int color);

// set the pixel
void SSD1306_graphics_set(uint8_t x, uint8_t y, int color);

// draws the horizontal line
void SSD1306_graphics_hline(uint8_t x1, uint8_t x2, uint8_t y, int color);

// draws the vertical line
void SSD1306_graphics_vline(uint8_t y1, uint8_t y2, uint8_t x, int color);

// draws the rectangle
void SSD1306_graphics_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color);

// draws the filled rectangle
void SSD1306_graphics_filled_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color);

// draws the bitmap
void SSD1306_graphics_bitmap(const uint8_t *bmp, uint8_t w, uint8_t h, uint8_t x, uint8_t y);

// draws text using specified symbol resolver
void SSD1306_graphics_text(
    const char *str,
    uint16_t x,
    uint16_t y,
    uint8_t*(*resolver)(char, uint16_t*, uint16_t*));

#endif