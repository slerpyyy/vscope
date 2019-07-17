all: vscope

vscope: vscope.c
	gcc vscope.c -o vscope -lm -Ofast -Wall -Wextra -Werror

install: vscope
	cp vscope /usr/local/bin/vscope

clean:
	rm -f vscope