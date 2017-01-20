#ifndef _framebuffer_h_
#define _framebuffer_h_

enum framebuffer_color {
	FRAMEBUFFER_BLACK = 0,
	FRAMEBUFFER_BLUE,
	FRAMEBUFFER_GREEN,
	FRAMEBUFFER_CYAN,
	FRAMEBUFFER_RED,
	FRAMEBUFFER_MAGENTA,
	FRAMEBUFFER_BROWN,
	FRAMEBUFFER_LIGHT_GRAY,
	FRAMEBUFFER_DARK_GRAY,
	FRAMEBUFFER_LIGHT_BLUE,
	FRAMEBUFFER_LIGHT_GREEN,
	FRAMEBUFFER_LIGHT_CYAN,
	FRAMEBUFFER_LIGHT_RED,
	FRAMEBUFFER_LIGHT_MAGENTA,
	FRAMEBUFFER_LIGHT_BROWN,
	FRAMEBUFFER_WHITE
};

struct framebuffer_attribute {
	enum framebuffer_color foreground;
	enum framebuffer_color background;
};

struct framebuffer_offset {
	int row;
	int column;
};

void framebuffer_clear_screen(void);

void framebuffer_write(const char*, int);

void framebuffer_set_cursor(struct framebuffer_offset);

/**
 * @return the offset of the cursor
 */
struct framebuffer_offset framebuffer_get_cursor(void);

void framebuffer_scroll(int row);

#endif

