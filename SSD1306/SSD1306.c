#include "SSD1306.h"

#include <string.h>
#include <math.h>

#include "../I2C/I2C.h"

// the framebuffer of the display
uint8_t *SSD1306_framebuffer;
uint16_t SSD1306_framebuffer_size = 0;

// This sequence is sent when display is initializing
const char SETUP_SEQUENCE[] = {
	__SSD1306_CMD__Display_Off,
	__SSD1306_CMD__Display_Clock_Div_Ratio_Set,		0xF0,
	__SSD1306_CMD__Multiplex_Radio_Set,				__SSD1306_HEIGHT - 1,
	__SSD1306_CMD__Display_Offset_Set,				0x00,
	__SSD1306_CMD__Charge_Pump_Set,					0x14,
	__SSD1306_CMD__Display_Start_Line_Set | 0x00,
	__SSD1306_CMD__Memory_Addressing_Set,			0x00,
	__SSD1306_CMD__Segment_Remap_Set | 0x01,
	__SSD1306_CMD__Com_Output_Scan_Dec,
	__SSD1306_CMD__Com_Pins_Set,					__SSD1306_HEIGHT == 64 ? 0x12 : 0x02,
	__SSD1306_CMD__Contrast_Set,					0xFF,
	__SSD1306_CMD__Precharge_Period_Set,			0xF1,
	__SSD1306_CMD__VCOMH_Deselect_Level_Set,		0x20,
	__SSD1306_CMD__Display_All_On_Resume,
	__SSD1306_CMD__Display_Normal,
	0x2E, // Disable scrolling
	__SSD1306_CMD__Display_On,
};


// prepare the display for work
void SSD1306_setup(void) {
	// begin the I2C
	I2C_start();

	// SLA+W
	I2C_send_one(I2C_get_addr_byte(__SSD1306_ADDRESS, 1));

	// send the setup sequence
	for (int i = 0; i < sizeof(SETUP_SEQUENCE) / sizeof(SETUP_SEQUENCE[0]); i++) {
		SSD1306_send_command(SETUP_SEQUENCE[i], 0);
	}

	// stop the I2C
	I2C_stop();

	// create the buffer
	SSD1306_framebuffer_size = __SSD1306_WIDTH * __SSD1306_HEIGHT / 8;
	SSD1306_framebuffer = malloc(SSD1306_framebuffer_size);
}


// send a single command without arguments to display
// if issue_start is 1 then the START and STOP conditions will be sent
void SSD1306_send_command(uint8_t command, int issue_start) {
	if (issue_start) {
		// begin the I2C
		I2C_start();

		// SLA+W
		I2C_send_one(I2C_get_addr_byte(__SSD1306_ADDRESS, 1));
	}

	// identify the control byte
	I2C_send_one(1 << 7);

	// send the byte with command
	I2C_send_one(command);

	// stop the I2C
	if (issue_start) I2C_stop();
}


// send framebuffer to display
void SSD1306_render(void) {
	// start the I2C
	I2C_start();

	// SLA+W
	I2C_send_one(I2C_get_addr_byte(__SSD1306_ADDRESS, 1));

	// go to beginning
	SSD1306_send_command(__SSD1306_CMD__Column_Address_Set, 0);
	SSD1306_send_command(0x00, 0);
	SSD1306_send_command(__SSD1306_WIDTH - 1, 0);

	SSD1306_send_command(__SSD1306_CMD__Page_Address_Set, 0);
	SSD1306_send_command(0x00, 0);
	SSD1306_send_command(__SSD1306_HEIGHT / 8 - 1, 0);

	// stop the I2C
	I2C_stop();



	//I2C_send_one(SSD1306_framebuffer[i]);
	for (int i = 0; i < SSD1306_framebuffer_size / 16; i++) {
		// start the I2C
		I2C_start();

		// SLA+W
		I2C_send_one(I2C_get_addr_byte(__SSD1306_ADDRESS, 1));

		// we're gonna send only data today
		I2C_send_one(1 << 6);

		I2C_send(SSD1306_framebuffer + 16 * i, 16);

		// stop the I2C
		I2C_stop();
	}

}


// fill the screen
void SSD1306_graphics_fill(int color) {
	memset(SSD1306_framebuffer, color ? 0xFF : 0x00, SSD1306_framebuffer_size);
}

// set the pixel
void SSD1306_graphics_set(uint8_t x, uint8_t y, int color) {
	// check OOB
	if (x >= __SSD1306_WIDTH) return;
	if (y >= __SSD1306_HEIGHT) return;

	int16_t line = y / 8;
	int16_t bit = y % 8;

	uint16_t offset = __SSD1306_WIDTH * line + x;

	if (color)
		SSD1306_framebuffer[offset] |= 1 << bit;
	else
		SSD1306_framebuffer[offset] &= ~(1 << bit);
}

// draws the horizontal line
void SSD1306_graphics_hline(uint8_t x1, uint8_t x2, uint8_t y, int color) {
	for (; x1 != x2; x1 += (x1 > x2 ? -1 : 1))
		SSD1306_graphics_set(x1, y, color);
	if (x2 >= __SSD1306_WIDTH) return;

	SSD1306_graphics_set(x2, y, color);
}

// draws the vertical line
void SSD1306_graphics_vline(uint8_t y1, uint8_t y2, uint8_t x, int color) {
	for (; y1 != y2; y1 += (y1 > y2 ? -1 : 1))
		SSD1306_graphics_set(x, y1, color);
	if (y2 >= __SSD1306_HEIGHT) return;

	SSD1306_graphics_set(x, y2, color);
}

// draws the rectangle
void SSD1306_graphics_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
	SSD1306_graphics_hline(x1, x2, y1, color);
	SSD1306_graphics_hline(x1, x2, y2, color);

	SSD1306_graphics_vline(y1, y2, x1, color);
	SSD1306_graphics_vline(y1, y2, x2, color);
}

// draws the filled rectangle
void SSD1306_graphics_filled_rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, int color) {
	if (x1 > x2) {
		uint8_t c = x2;
		x2 = x1;
		x1 = c;
	}
	if (y1 > y2) {
		uint8_t c = y2;
		y2 = y1;
		y1 = c;
	}

	for (uint8_t x = x1; x <= x2; x++)
		for (uint8_t y = y1; y <= y2; y++)
			SSD1306_graphics_set(x, y, color);
}

// draws the bitmap
void SSD1306_graphics_bitmap(const uint8_t *bmp, uint8_t w, uint8_t h, uint8_t x, uint8_t y) {
	for (uint8_t bx = 0; bx < w; bx++) {
		// position of BMP pixel on screen
		uint8_t fx = x + bx;

		// out of bounds
		if (fx >= __SSD1306_WIDTH) break;

		for (uint8_t by = 0; by < h; by++) {
			uint8_t fy = y + by;

			// out of bounds
			if (fy >= __SSD1306_HEIGHT) break;

			// get pixel value
			uint8_t b_line = by / 8;
			uint8_t b_bit = by % 8;

			uint8_t b_val = bmp[b_line * w + bx] & (1 << b_bit);

			SSD1306_graphics_set(
				fx,
				fy,
				b_val);

		}
	}
}

// draws the text using specified bmp resolver
void SSD1306_graphics_text(
	const char *str,
	uint16_t x,
	uint16_t y,
	uint8_t*(*resolver)(char, uint16_t*, uint16_t*)) {

	// for newlines
	uint16_t original_x = x;

	for (int i = 0; str[i]; ++i) {
		// check if newline
		if (str[i] == '\n') {
			// symbols are 8 pixels tall, so we move cursor 9 pixels down
			y += 9;
			x = original_x;
			continue;
		}

		// get the bitmap for the symbol
		uint16_t w, h;
		uint8_t* bmp = (*resolver)(str[i], &w, &h);

		// draw the symbol
		SSD1306_graphics_bitmap(bmp, w, h, x, y);

		// move the cursor
		x += w + 1;
	}
}