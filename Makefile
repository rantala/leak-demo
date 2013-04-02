main: main.c
	$(CC) -pthread $(CFLAGS) $< -o $@ -lX11 -lrt
