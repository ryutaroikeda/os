void _start() {
	char* const video_memory = (char*) 0xb8000;
	*video_memory = 'X';
}
