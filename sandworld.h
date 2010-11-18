/* Copyright (c) 2010, Michael Patraw
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of Michael Patraw may not be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Michael Patraw ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Michael Patraw BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SANDWORLD_H
#define SANDWORLD_H

#include <assert.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SW_WORLD_WIDTH	80
#define SW_WORLD_HEIGHT	24

#define SW_BLACK	COLOR_BLACK
#define SW_RED		COLOR_RED
#define SW_GREEN	COLOR_GREEN
#define SW_BLUE		COLOR_BLUE
#define SW_YELLOW	COLOR_YELLOW
#define SW_MAGENTA	COLOR_MAGENTA
#define SW_CYAN		COLOR_CYAN
#define SW_WHITE	COLOR_WHITE
#define SW_COLORS	8

#define SW_ROWS	LINES
#define SW_COLS	COLS

/* The desirable width and height. */
#define SW_WIDTH	80
#define SW_HEIGHT	24

#define SW_ERR_GENERAL		(-1)	/* General error */
#define SW_ERR_NONCOMP		(-2)	/* Non-compliancy */
#define SW_ERR_NOMEM		(-3)	/* No memory */
#define SW_ERR_OVERFLOW		(-4)	/* Buffer overflow */
#define SW_ERR_UNDERFLOW	(-5)	/* Buffer underflow */
#define SW_ERR_RANGE		(-6)	/* Range error */

#define SW_CMD_NONE		(-1)
#define SW_CMD_UP		KEY_UP
#define SW_CMD_DOWN		KEY_DOWN
#define SW_CMD_LEFT		KEY_LEFT
#define SW_CMD_RIGHT		KEY_RIGHT
#define SW_CMD_UP2		'k'		/* Another way to move left. */
#define SW_CMD_DOWN2		'j'		/* Another way to move down. */
#define SW_CMD_LEFT2		'h'		/* Another way to move up. */
#define SW_CMD_RIGHT2		'l'		/* Another way to move right. */
#define SW_CMD_ACTION		' '		/* Perform action. */
#define SW_CMD_ACTION2		'\n'
#define SW_CMD_QUIT		'q'		/* Quit anything. */
#define SW_CMD_QUIT2		27
#define SW_CMD_SELF		'w'		/* Info on self or selected. */
#define SW_CMD_RUCKSACK		'e'		/* Rucksack of self. */
#define SW_CMD_ATTACK		'a'		/* Attack something. */
#define SW_CMD_SWAP		's'		/* Swap with object. */
#define SW_CMD_INFO		'f'		/* Info on other objects. */
#define SW_CMD_SKILL		'z'
#define SW_CMD_TOOL		'x'		/* Use a tool on something. */
#define SW_CMD_CREATE		'c'
#define SW_CMD_HELP		'?'		/* HALP */

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

typedef short color_t;
typedef chtype attr_t;

#define SW_ATTR_NONE		0
#define SW_ATTR_BRIGHT		A_BOLD

/* Random seed...
 */
void sw_seed(unsigned long s);

/* Returns a number between `l` and `h` inclusive.
 */
int sw_randint(unsigned int l, unsigned int h);

/* Puts a colored character on the screen at x,y
 */
void sw_putch(int x, int y, int ch);

/* Waits for a command and returns it.
 */
int sw_getcmd(void);

/* Sets the foreground and background color, and the attribute.
 */
void sw_setfgbg(color_t fg, color_t bg, attr_t attr);

/* Just sets foreground colors (sets background color to black).
 */
void sw_setfg(color_t fg);

/* Outputs a string to the screen.
 */
void sw_putstr(int x, int y, const char *str, ...);
void sw_vputstr(int x, int y, const char *str, va_list args);

/* Clears a line from xstart to xend to black, including xend.
 */
void sw_clearlineto(int line, int xstart, int xend);

/* Draws a box using pipe characters.
 */
void sw_box(int x, int y, int w, int h);

/* Sets `dx` and `dy` according to the direction. x=0,y=-1 would be UP.
 */
void sw_getdelta(int dir, int *dx, int *dy);

void sw_addinfo(const char *str, ...);
void sw_infobox(int x, int y);
void sw_clearinfo(void);

void sw_addmenuheader(const char *str, ...);
void sw_addmenu(const char *str, ...);
int sw_menubox(int x, int y);
void sw_clearmenu(void);

/* Maybe use this. */
void sw_loopevent(int (*fp) (int cmd));
void sw_loopdraw(void (*fp) (void));
void sw_loop(void);

#endif
