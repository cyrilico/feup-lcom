#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "test5.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static unsigned short parse_ushort(char *str, int base);
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
			"\t service run %s -args \"test_init <decimal no.- mode> <decimal no. - delay>\"\n"
			"\t service run %s -args \"test_square <decimal no. - x> <decimal no. - y> <decimal no. - size> <decimal no. - color>\"\n"
			"\t service run %s -args \"test_line <decimal no. - xi> <decimal no. - yi> <decimal no. - xf> <decimal no. - yf> <decimal no. - color>\"\n"
			"\t service run %s -args \"test_xpm <decimal no. - xi> <decimal no. - yi> <string xpm>\"\n"
			"\t service run %s -args \"test_move <decimal no. - xi> <decimal no. - yi> <string xpm> <decimal no. - hor> <decimal no. - delta> <decimal no. - time>\"\n"
			"\t service run %s -args \"test_controller <void>\"\n",
			argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char **argv)
{
	if (strncmp(argv[1], "test_init", strlen("test_init")) == 0) {
		if (argc != 4) {
			printf("video_card: wrong no. of arguments for test_init()\n");
			return 1;
		}
		unsigned short mode = parse_ushort(argv[2], 16);
		if (mode == USHRT_MAX)
			return 1;
		unsigned short delay = parse_ushort(argv[3], 10);
		if (delay == USHRT_MAX)
			return 1;
		printf("video_card::test_init(%u,%u)\n", mode, delay);
		test_init(mode,delay);
		return 0;
	}
	else if (strncmp(argv[1], "test_square", strlen("test_square")) == 0) {
		if (argc != 6) {
			printf("video_cad: wrong no. of arguments for test_square()\n");
			return 1;
		}
		unsigned short x = parse_ushort(argv[2], 10);
		if (x == USHRT_MAX)
			return 1;
		unsigned short y = parse_ushort(argv[3], 10);
		if (y == USHRT_MAX)
			return 1;
		unsigned short size = parse_ushort(argv[4], 10);
		if (size == USHRT_MAX)
			return 1;
		unsigned long color = parse_ulong(argv[5], 16);
		if (color == ULONG_MAX)
			return 1;
		printf("video_card::test_square(%u,%u,%u,%lu)\n", x, y, size, color);
		return test_square(x,y,size,color);
	}
	else if (strncmp(argv[1], "test_line", strlen("test_line")) == 0) {
		if (argc != 7) {
			printf("video_card: wrong no of arguments for test_line()\n");
			return 1;
		}
		unsigned short xi = parse_ushort(argv[2], 10);
		if (xi == USHRT_MAX)
			return 1;
		unsigned short yi = parse_ushort(argv[3], 10);
		if (yi == USHRT_MAX)
			return 1;
		unsigned short xf = parse_ushort(argv[4], 10);
		if (xf == USHRT_MAX)
			return 1;
		unsigned short yf = parse_ushort(argv[5], 10);
		if (yf == USHRT_MAX)
			return 1;
		unsigned long color = parse_ulong(argv[6], 16);
		if (color == ULONG_MAX)
			return 1;

		printf("video_card::test_line(%u,%u,%u,%u,%lu)\n", xi, yi, xf, yf, color);
		return test_line(xi,yi,xf,yf,color);
	}
	/*Missing code for calling test_xpm and test_move (still strying to figure out parameter char* xpm[]) and how it should be tested here*/
	else if (strncmp(argv[1], "test_controller", strlen("test_controller")) == 0) {
		if (argc != 2) {
			printf("video_card: wrong no. of arguments for test_controller()\n");
			return 1;
		}
		printf("video_card::test_controller()\n");
		return test_controller();
	}
	else {
		printf("video_card: %s - no valid function!\n", argv[1]);
		return 1;
	}
}

static unsigned short parse_ushort(char *str, int base)
{
	char *endptr;
	unsigned long val;

	/* Convert string to unsigned long */
	val = strtoul(str, &endptr, base);

	/* Check for conversion errors */
	if ((errno == ERANGE && val == ULONG_MAX) || (errno != 0 && val == 0)) {
		perror("strtoul");
		return USHRT_MAX;
	}

	if (endptr == str) {
		printf("video_card: parse_ushort: no digits were found in %s\n", str);
		return USHRT_MAX;
	}

	/* Successful conversion */
	return (unsigned short)val;
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

