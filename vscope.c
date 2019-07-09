#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdbool.h>

// gcc vscope.c -o vscope -lm -Wall; cat vscope.c | ./vscope

int main(int argc, char *argv[])
{
	//setting up

	struct winsize dim;
    ioctl(STDOUT_FILENO, TIOCGWINSZ , &dim);

	int size = fmin(dim.ws_row, dim.ws_col/2) - 2;
	if(size < 0 || size > 256) size = 256;

	int read = -1;
	int thold = -1;
	bool color = false;
	bool img = false;

	for(int i=1; i<argc; i++)
	{
		if(strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0)
		{
			printf(
				//--------------------------------------------------------------------------------
				"Usage: cat [FILE] | %s [OPTION]...\n"
				"\n"
				"Visualizes STDIN by writing a 2D histogram to STDOUT.\n"
				"Useful for classifing unknown datasets.\n"
				"\n"
				"Options:\n"
				"  -h, --help         Shows this message.\n"
				"  -s, --size=NUM     Changes the size of the heatmap. If this flag is not set,\n"
				"                     the map will be sized to fit the terminal window.\n"
				"  -b, --bytes=NUM    Only reads the first NUM bytes from STDIN.\n"
				"  -c, --color        Colors the heatmap using ANSI escape codes. This might not\n"
				"                     look as intendet, depending on the system this is running on.\n"
				"  -i, --image        Write pgm image data directly to STDOUT.\n"
				"  -t, --thold=[NUM]  Checks if the value is higher than the threshold and\n"
				"                     outputs either ff or 00. If no NUM is defined, the\n"
				"                     threshold will be set to 0.\n"
				//--------------------------------------------------------------------------------
			, argv[0]);
			exit(EXIT_SUCCESS);
		}

		if(strcmp(argv[i], "--size")==0 || strcmp(argv[i], "-s")==0)
		{
			size = atoi(argv[++i]);
			if(size == 0)
			{
				fprintf(stderr, "Invalid argument after %s\n", argv[i-1]);
				exit(EXIT_FAILURE);
			}
			size = fmin(size, 256);
			size = fmax(size, 0);
		}

		if(strcmp(argv[i], "--bytes")==0 || strcmp(argv[i], "-b")==0)
		{
			read = fmax(atoi(argv[++i]), 0);
			if(read == 0)
			{
				fprintf(stderr, "Invalid argument after %s\n", argv[i-1]);
				exit(EXIT_FAILURE);
			}
		}

		if(strcmp(argv[i], "--color")==0 || strcmp(argv[i], "-c")==0)
		{
			color = true;
		}

		if(strcmp(argv[i], "--image")==0 || strcmp(argv[i], "-i")==0)
		{
			img = true;
		}

		if(strcmp(argv[i], "--thold")==0 || strcmp(argv[i], "-t")==0)
		{
			thold = 0;
			if(i+1 < argc)
			{
				thold = fmax(atoi(argv[i+1]), 0);
			}
		}
	}

	// reading and analysing stdin

	int map[size][size];
	memset(map, 0x00, sizeof(map[0][0])*size*size);

	int this, last = getchar();

	while((this = getchar()) != EOF)
	{
		if(read == 0) break;
		if(read >  0) read--;

		int x = (int)floor(size * last / 256.0);
		int y = (int)floor(size * this / 256.0);
		map[x][y]++;

		last = this;
	}

	// printing out table

	if(img) printf("P5\n%d %d\n%d\n", size, size, 0xff);
	
	for(int y=0; y<size; y++)
	{
		for(int x=0; x<size; x++)
		{
			int val = map[x][y];

			if(thold != -1) val = 0xff * (val > thold);
			else val = val < 0xff ? val : 0xff;

			if(img)
			{
				putchar(val);
				continue;
			}

			if(color)
			{
				int r = val;
				int b = 255-val;
				printf("\x1b[48;2;%d;%d;%dm", r, 0, b);

				/*int fg = 255 * (val < 128);
				int bg = val;
				printf("\x1b[38;2;%d;%d;%dm", fg, fg, fg);
				printf("\x1b[48;2;%d;%d;%dm", bg, bg, bg);*/
			}

			if(val < 1)
			{
				printf("--");
				continue;
			}

			printf("%02x", val);
		}

		if(color) printf("\x1b[0m");
		if(!img) putchar('\n');
	}

	return 0;
}