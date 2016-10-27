#include "keyboard.h"

int kbd_test_scan(unsigned short ass) {
	return kbd_scan_loop();
}
int kbd_test_leds(unsigned short n, unsigned short *leds) {
	return kbd_leds_loop(n, leds);
}
int kbd_test_timed_scan(unsigned short n) {
    return kbd_timed_scan_loop(n);
}
