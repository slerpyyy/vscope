#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>


// define global vars
bool g_col, g_gray, g_img, g_lv;
int16_t g_size, g_thld;
int64_t g_rdn;


void help()
{
	puts(
		"Visualizes STDIN by writing a 2D histogram to STDOUT.\n"
		"Useful for classifing unknown datasets.\n\n"

		"Options:\n"
		"  -h, --help         Shows this message and exits.\n"
		"  -s, --size=NUM     Changes the size of the heatmap. If this flag is not set,\n"
		"                     the map will be sized to fit the terminal window.\n"
		"  -b, --bytes=NUM    Only reads the first NUM bytes from STDIN.\n"
		"  -c, --color        Colors the heatmap using ANSI escape codes. This might\n"
		"                     not look as intendet, depending on your terminal.\n"
		"  -g, --gray         Enables a gray-scale version of --color. This flag\n"
		"                     cannot be set in combination with --color.\n"
		"  -p, --picture      Write pgm image data directly to STDOUT.\n"
		"  -t, --thold=[NUM]  Checks if the value is higher than the threshold and\n"
		"                     outputs either 00 or ff. If no NUM is defined, the\n"
		"                     threshold will be set to 0.\n"
	);
	exit(EXIT_SUCCESS);
}


void setup(int argc, char **argv)
{
	// auto detect size
	struct winsize dim;
	ioctl(STDOUT_FILENO, TIOCGWINSZ , &dim);
	g_size = fmin(dim.ws_row-4, dim.ws_col/2);
	if(g_size < 0 || g_size > 256) g_size = 256;

	// set default values
	g_rdn = -1;
	g_thld = -1;
	g_col = false;
	g_gray = false;
	g_img = false;
	g_lv = false;

	// parse cmd args
	for(int i=1; i<argc; i++)
	{
		if(strcmp(argv[i], "--help")==0 || strcmp(argv[i], "-h")==0)
		{
			printf("Usage: cat [FILE] | %s [OPTION]...\n\n", argv[0]);
			help();
		}

		else if(strcmp(argv[i], "--size")==0 || strcmp(argv[i], "-s")==0)
		{
			g_size = atoi(argv[++i]);
			if(g_size == 0)
			{
				fprintf(stderr, "Invalid argument after %s\n", argv[i-1]);
				exit(EXIT_FAILURE);
			}
			g_size = fmin(g_size, 256);
			g_size = fmax(g_size, 0);
		}

		else if(strcmp(argv[i], "--bytes")==0 || strcmp(argv[i], "-b")==0)
		{
			int len = strlen(argv[++i]);
			int64_t mul = 1;

			if(len > 1)
			{
				// fetch last char
				char post = argv[i][len-1];

				// post is not number literal
				if(post > 0x39 || post < 0x30)
				{
					// shorten string
					argv[i][len-1] = '\0';
					mul = 0;

					// find postfix
					if(post == 'k')mul = 1000;
					if(post == 'K')mul = 1024;
					if(post == 'm')mul = 1000000;
					if(post == 'M')mul = 1048576;
					if(post == 'g')mul = 1000000000;
					if(post == 'G')mul = 1073742000;
					if(post == 't')mul = 1000000000000;
					if(post == 'T')mul = 1099512000000;
					if(post == 'p')mul = 1000000000000000;
					if(post == 'P')mul = 1125900000000000;
				}
			}

			g_rdn = mul * fmax(atoi(argv[i]), 0);

			if(g_rdn == 0)
			{
				fprintf(stderr, "Invalid argument after %s\n", argv[i-1]);
				exit(EXIT_FAILURE);
			}
		}

		else if(strcmp(argv[i], "--live")==0 || strcmp(argv[i], "-l")==0)
		{
			if(g_rdn < 0)g_rdn = 1024;
			g_lv = true;
		}

		else if(strcmp(argv[i], "--color")==0 || strcmp(argv[i], "-c")==0)
		{
			g_col = true;
		}

		else if(strcmp(argv[i], "--gray")==0 || strcmp(argv[i], "-g")==0)
		{
			g_gray = true;
		}

		else if(strcmp(argv[i], "--picture")==0 || strcmp(argv[i], "-p")==0)
		{
			g_img = true;
		}

		else if(strcmp(argv[i], "--thold")==0 || strcmp(argv[i], "-t")==0)
		{
			g_thld = 0;
			if(i+1 < argc)
			{
				g_thld = fmax(atoi(argv[i+1]), 0);
			}
		}

		else
		{
			fprintf(stderr,
				"Unable to parse argument: %s\n"
				"Try '%s --help' for more information.\n",
			argv[i], argv[0]);

			exit(EXIT_FAILURE);
		}
	}

	// check for contradictions
	if(g_col && g_gray)
	{
		fprintf(stderr,
			"Cannot use both color themes at once\n"
			"Try '%s --help' for more information.\n",
		argv[0]);
		exit(EXIT_FAILURE);
	}

	if(g_img && g_lv)
	{
		fprintf(stderr,
			"Cannot output image data in live mode\n"
			"Try '%s --help' for more information.\n",
		argv[0]);
		exit(EXIT_FAILURE);
	}
}


void generate(uint8_t *map)
{
	// prepare vars
	int this, last = getchar();
	int64_t cnt = 0;

	// loop over input data
	while(1)
	{
		// read byte
		this = getchar();

		// check for EOF
		if(this == EOF)
		{
			g_lv = false;
			break;
		}

		// count down bytes
		if(g_rdn > 0)
		{
			if(cnt >= g_rdn)break;
			cnt++;
		}

		// increment counter
		int x = (int)floor(g_size * last / 256.0);
		int y = (int)floor(g_size * this / 256.0);
		int i = x + g_size * y;
		if(map[i] < 0xff)map[i]++;

		// update vars
		last = this;
	}
}


void draw(uint8_t *map)
{
	// print pgm header
	if(g_img) printf("P5\n%d %d\n%d\n", g_size, g_size, 0xff);

	for(int y=0; y<g_size; y++)
	{
		for(int x=0; x<g_size; x++)
		{
			// read value from heatmap
			int val = map[x + g_size * y];

			// clamp value and step on threshold
			if(g_thld != -1) val = 0xff * (val > g_thld);

			// output raw byte in image mode
			if(g_img)
			{
				putchar(val);
				continue;
			}

			// calculate and output gay color codes
			if(g_col)
			{
				int x = val + val - 255;
				uint8_t r = fmax(x, 0);
				uint8_t g = 255 - fabs(x);
				uint8_t b = fmax(-x, 0);
				printf("\e[48;2;%d;%d;%dm", r, g, b);
			}

			// output gray-scale color codes
			else if(g_gray)
			{
				uint8_t bg = val;
				uint8_t fg = 255 * (bg < 128);
				printf("\e[38;2;%d;%d;%dm", fg, fg, fg);
				printf("\e[48;2;%d;%d;%dm", bg, bg, bg);
			}

			// output hex value
			if(val < 1) printf("--");
			else printf("%02x", val);
		}

		// reset color code
		if(g_col || g_gray) printf("\e[0m");

		// add linebreak
		if(!g_img) putchar('\n');
	}

	// final linebreak for spacing
	if(!g_img) putchar('\n');

	if(g_lv) printf("\e[%dA", g_size+1);
}


int main(int argc, char *argv[])
{
	// setup global vars
	setup(argc, argv);

	// init heatmap
	int memsize = sizeof(uint8_t)*g_size*g_size;
	uint8_t *map = (uint8_t *)malloc(memsize);

	// clear map
	memset(map, 0x00, memsize);

	_repeat:

	// create and print heatmap
	generate(map);
	draw(map);

	if(g_lv) goto _repeat;

	// free heatmap
	free(map);

	// clean exit
	exit(EXIT_SUCCESS);
	return 0;
}
