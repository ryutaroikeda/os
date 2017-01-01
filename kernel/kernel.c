void some_function() {
}

void main() {
	char* const video_memory = (char*) 0xb8000;
	*video_memory = 'X';
	some_function();
}
