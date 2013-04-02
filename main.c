#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

static void *tf(void *ptr) { return ptr; }

static void
leak_pixmap(void)
{
	Display *display = XOpenDisplay(NULL);
	if (!display) {
		fprintf(stderr, " - Unable to open X11 display.\n");
		return;
	}
	int screen = DefaultScreen(display);
	Window win = XCreateSimpleWindow(display, RootWindow(display, screen), 0, 0, 10, 10, 0, 0, 0);
	XCreatePixmap(display, win, 10, 10, 8);
}

static void
leak(int round)
{
	pthread_t thr;

	printf("- Leaking a file descriptor.\n");
	if (open("/dev/zero", O_RDONLY) == -1)
		fprintf(stderr,
			" - Unable to open /dev/zero: %s",
			strerror(errno));

	printf("- Leaking a socket.\n");
	socket(AF_INET, SOCK_STREAM, 0);

	printf("- Leaking 64 bytes with malloc().\n");
	if (malloc(64) == NULL)
		fprintf(stderr,
			" - malloc() failed: %s\n",
			strerror(errno));

	printf("- Leaking one page (%d bytes) with mmap().\n", getpagesize());
	mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

	printf("- Creating joinable thread but not joining it.\n");
	if (pthread_create(&thr, NULL, tf, NULL) != 0)
		fprintf(stderr,
			" - pthread_create() failed.\n");

	printf("- Leaking X11 pixmap.\n");
	leak_pixmap();

	//mq_open("");
	//flock(...);
}

int main(void)
{
	int round = 1;
	while (1) {
		printf("Round %d:\n", round);
		leak(round);
		sleep(10);
		++round;
	}
	return 0;
}
