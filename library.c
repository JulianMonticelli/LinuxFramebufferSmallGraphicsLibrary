#include "library.h"
#include "iso_font.h"


int fd; // file descriptor
int fd1 = 1; // idk why this was in skeleton code


static const int WIDTH = 640;
static const int HEIGHT = 480;

color_t * address;
int size;

void init_graphics() {
	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1) {
		fprintf(stderr, "Error opening /dev/fb0\n");
		exit(fd);
	}
	
	struct fb_var_screeninfo var_screeninfo;
	ioctl(fd, FBIOGET_VSCREENINFO, &var_screeninfo);
	struct fb_fix_screeninfo fix_screeninfo;
	ioctl(fd, FBIOGET_FSCREENINFO, &fix_screeninfo);
	

	size = ((int)var_screeninfo.yres_virtual) * 
	((int)fix_screeninfo.line_length); //
	address = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	if (address == (void *)-1) {
		fprintf(stderr, "Error mapping memory\n");
		exit(1);
	}
	
	struct termios terminal;
	
	ioctl(fd1, TCGETS, &terminal);
	
	terminal.c_lflag &= ~ICANON;
	terminal.c_lflag &= ~ECHO;
	
	ioctl(fd1, TCSETS, &terminal);
	clear_screen();
}


void exit_graphics() {
	struct termios terminal;
	
	ioctl(fd1, TCGETS, &terminal);
	
	terminal.c_lflag |= ICANON;
	terminal.c_lflag |= ECHO;
	
	ioctl(fd1, TCSETS, &terminal);
	if (munmap(address, size) == -1)
	{
		fprintf(stderr, "Error unmapping memory\n"); // If there was an error unmapping the memory
		exit(1);
	}
	if (!close(fd))
	{
		exit(0);
	}
	else
	{
		fprintf(stderr, "Error closing /dev/fb0\n");
		exit(1);
	}
}


void clear_screen() {
	write(fd1, "\033[2J", 4); // why? 
	//printf("\033[2J");
}


char getkey() {
	// Get key from input
	char c = 0;
	struct timeval t_v = { 0L, 0L }; // Set block time to 0
	fd_set fds; // File descriptor set
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	int ret_value = select(1, &fds, NULL, NULL, &t_v);
	if(ret_value) {
		read(0, &c, sizeof(c)); // read from buffer
		return c;
	}
	else {
		return c; // should return non-printable char
	}
}


void sleep_ms(long ms) { // Should be finished
	// POSIX only, of course :)
	struct timespec ts; // Struct for time we will ellapse in the wait
	ts.tv_sec = ms / 1000; // If we wait more that 1000 ms, we have a second
	ts.tv_nsec = (ms % 1000) * 1000000; // For our MS, mod 1000 and multiply by 1,000,000 to get ms from ns
	nanosleep(&ts, NULL); // Sleep for timespec
}


void draw_pixel(int x, int y, color_t color) {
	if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
		return; // We don't need to draw to what isn't on screen
	}
	address[(y*WIDTH) + x] = color;
}

void draw_rect(int x1, int y1, int width, int height, color_t color) {
	int x, y;
	for (x = x1; x <= x1+width; x++) {
		if(x < 0 || x >= WIDTH) {
			continue;
		}
		if(y1 >= 0 && y1 < HEIGHT) {
			draw_pixel(x, y1, color);
		}
		if(y1+height >= 0 && y1+height < HEIGHT) {
			draw_pixel(x, y1+height, color);
		}
	}
	for (y = y1; y <= y1+height; y++) {
		if(y < 0 || y >= HEIGHT) {
			continue;
		}
		if(x1 >= 0 && x1 < WIDTH) {
			draw_pixel(x1, y, color);
		}
		if(x1+width >= 0 && x1+width < WIDTH) {
			draw_pixel(x1+width, y, color);
		}
	}
}

void draw_rect_filled(int x1, int y1, int width, int height, color_t 
color) {
	int x, y;
	
	for (x = x1; x <= x1+width; x++) {
		if(x < 0 || x >= WIDTH) {
			continue;
		}
		for(y = y1; y <= y1+height; y++) {
			if(y >= 0 &&  y < HEIGHT) { // draw top line
				draw_pixel(x, y, color);
			}
		}
	}
}


void draw_text(int x, int y, const char * text, color_t color) {
	int i = 0; // Character iteration
	// Loop for each iteration
	// No bound check because we
	// May want to write offscreen
	// In rare circumstances
	while(text[i] != '\0') {
		int c = (int)text[i];
		int c_dep, c_wid;
		for(c_dep = 0; c_dep < 16; c_dep++) {
			unsigned char byte = iso_font[c*16+c_dep];
			for(c_wid = 0; c_wid < 8; c_wid++) {
				//int sh_amt = c_wid;
				if((byte >> c_wid) & 0x01) {
					draw_pixel( (x+(i*8)+c_wid), 
					(y+c_dep), color);
				}
			}
		}
		i++; // Increment i
	}
}
