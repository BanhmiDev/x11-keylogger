all:
	gcc main.c -o x11-keylogger -l X11

clean:
	rm x11-keylogger
