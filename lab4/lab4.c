#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "test4.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char *argv[])
{
	/* Initialize service */
	sef_startup();

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test_packet <decimal no.- number of packets>\"\n"
			"\t service run %s -args \"test_async <decimal no. - idle time, in seconds>\"\n"
			"\t service run %s -args \"test_config <void>\"\n"
			"\t service run %s -args \"test_gesture <decimal no. - minimum length of movement\"\n",
			argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	unsigned short counter;
	unsigned short idle_time;
	short length;
	if (strncmp(argv[1], "test_packet", strlen("test_packet")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_packet()\n");
			return 1;
		}
		counter = parse_ulong(argv[2], 10);
		if (counter == ULONG_MAX)
			return 1;
		printf("mouse::test_packet(%lu)\n", counter);
		return test_packet(counter);
	}
	else if (strncmp(argv[1], "test_async", strlen("test_async")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_async()\n");
			return 1;
		}
		idle_time = parse_ulong(argv[2], 10);
		if (idle_time == ULONG_MAX)
			return 1;
		printf("mouse::test_async(%lu)\n", idle_time);
		return test_async(idle_time);
	}
	else if (strncmp(argv[1], "test_config", strlen("test_config")) == 0) {
		if (argc != 2) {
			printf("mouse: wrong no of arguments for test_config()\n");
			return 1;
		}
		printf("mouse::test_config()\n");
		return test_config();
	}
	else if (strncmp(argv[1], "test_gesture", strlen("test_gesture")) == 0) {
		if (argc != 3) {
			printf("mouse: wrong no. of arguments for test_gesture()\n");
			return 1;
		}
		length = parse_ulong(argv[2], 10);
		if (length == ULONG_MAX)
			return 1;
		printf("mouse::test_gesture(%d)\n", (short)length);
		return test_gesture(length);
	}
	else {
		printf("mouse: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned long parse_ulong(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return ULONG_MAX;
	}

	if (endptr == str) {
		printf("timer: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
