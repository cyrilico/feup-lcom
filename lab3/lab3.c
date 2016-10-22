#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include "test3.h"

static int proc_args(int argc, char **argv);
static unsigned long parse_ulong(char *str, int base);
static void print_usage(char **argv);

int main(int argc, char **argv)
{
	/* Initialize service */
	sef_startup();
	/* Enable IO-sensitive operations for ourselves */
  	sys_enable_iop(SELF);

	if (argc == 1) {					/* Prints usage of the program if no arguments are passed */
		print_usage(argv);
		return 0;
	}
	else return proc_args(argc, argv);
}

static void print_usage(char **argv)
{
	printf("Usage: one of the following:\n"
			"\t service run %s -args \"test_scan <decimal no. - 0: C version, other: assembly version>\"\n"
			"\t service run %s -args \"test_leds <array of decimal no. [0-2] - sequence of leds to manipulate>\"\n"
			"\t service run %s -args \"test_timed_scan <decimal no. - waiting time>\"\n",
			argv[0], argv[0], argv[0]);
}

static int proc_args(int argc, char *argv[]) {
  /*check the function to test: if the first characters match, accept it */
if (strncmp(argv[1], "test_scan", strlen("test_scan")) == 0) {
	  if( argc != 3 ) {
		  printf("keyboard: wrong no of arguments for test of kbd_test_scan() \n");
		  return 1;
	  }
	  unsigned long c_or_asm;
	  if( (c_or_asm = parse_ulong(argv[2], 10)) == ULONG_MAX )
	  		  return 1;
	  printf("keyboard:: kbd_test_scan(%lu)\n", c_or_asm);
	  kbd_test_scan(c_or_asm);
	  return 0;
  } else if (strncmp(argv[1], "test_leds", strlen("test_leds")) == 0) {
	  if( argc < 3) { //Since the function has been correctly identified, means that empty array was provided regardless of size specified
		  printf("keyboard: wrong no of arguments for test of kbd_test_leds()\n");
		  return 1;
	  }
	  unsigned long size = argc - 2;
	  unsigned int *leds;
	  unsigned int i;
	  for(i = 0; i < size; i++){
		  leds[i] = parse_ulong(argv[2+i],10);
		  if(leds[i] == ULONG_MAX)
			  return 1;
		  if(leds[i] > 2){ //Invalid led value
			  printf("keyboard: invalid led value, must be in range [0-2]\n");
			  return 1;
		  }
	  }
	  printf("keyboard:: kbd_test_leds for array [ ");
	  for(i = 0; i < size; i++){
		  printf("%lu ",leds[i]);
	  }
	   printf(" ]\n");
	  kbd_test_leds(size,leds);
	  return 0;
  } else if (strncmp(argv[1], "test_timed_scan", strlen("test_timed_scan")) == 0) {
  	  if( argc != 3) {
  		  printf("keyboard: wrong no of arguments for test of kbd_test_timed_scan() \n");
  		  return 1;
  	  }
  	  unsigned long waiting_time;
  	  if( (waiting_time = parse_ulong(argv[2], 10)) == ULONG_MAX )
  	  		  return 1;
  	  printf("keyboard:: kbd_test_timed_scan(%lu)\n",waiting_time);
  	  kbd_test_timed_scan(waiting_time);
  	  return 0;
  }  else {
  	  printf("keyboard:: non valid function \"%s\" to test\n", argv[1]);
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
		printf("keyboard: parse_ulong: no digits were found in %s\n", str);
		return ULONG_MAX;
	}

	/* Successful conversion */
	return val;
}
