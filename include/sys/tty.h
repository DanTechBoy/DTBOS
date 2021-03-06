/**
 * SPDX-FileCopyrightText: 2020 Anuradha Weeraman <anuradha@weeraman.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stddef.h>

struct fb_info {
	uint8_t  type;
	uint8_t  bpp;
	size_t  *addr; /* 64-bit field in the specification */
	size_t   width;
	size_t   height;
	size_t   pitch;
};

void init_console(struct fb_info framebuffer);
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void disable_cursor();
void update_cursor();
uint16_t get_cursor_position(void);
void printc(uint8_t ch);
void draw_pixel(int x, int y, size_t color);
void clear_screen(void);
void identity_map_framebuffer();
struct fb_info *get_fb_info();

#endif
