enum Color {
	BLACK = 0,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHT_GRAY,
	DARK_GRAY,
	LIGHT_BLUE,
	LIGHT_GREEN,
	LIGHT_CYAN,
	LIGHT_RED,
	LIGHT_MAGENTA,
	LIGHT_BROWN,
	WHITE
};

union attribute {
	struct {
		unsigned char foreground: 4;
		unsigned char background: 4;
	} attribute;
	unsigned char bits;
};

char* const video_memory = (char*) 0xb8000;

void framebuffer_write_cell(int, char, union attribute); 
void main(void);

void framebuffer_write_cell(int offset, char c, union attribute attribute) {
	video_memory[offset * 2] = c;
	video_memory[(offset * 2) + 1] = (char) attribute.bits;
}

void main(void) {
	union attribute a;
	a.attribute.foreground = BLUE;
	a.attribute.background = WHITE;
	framebuffer_write_cell(2, 'X', a);
}

