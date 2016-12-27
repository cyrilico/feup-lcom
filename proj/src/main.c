#include "dispatcher.h"

  /* Current VESA graphics mode chosen: 0x114 (800x600, 16bit colors in RGB 5:6:5) */

void print_usage(char **argv)
{
	printf("Usage:\n"
			"\t service run %s -args \"<path to the repository's parent directory> \n"
			"Example : service run &s -args \"/home/feup/\" **DON'T FORGET THE SLASH** /n",
			argv[0], argv[0]);
}

int main(int argc, char **argv) {
	/* Initialize service */
	sef_startup();

	if(argc != 2) {
		print_usage(argv);
		return 0;
	}
	/* Enable IO-sensitive operations for ourselves */
	sys_enable_iop(SELF);

	srand(time(NULL));

	if(start_graphic_mode() != OK)
		return -1;

	Dispatcher* dispatcher = create_dispatcher(argv[1]);

	while(dispatcher->state != EXIT_PROGRAM)
		interrupt_handler(dispatcher);

	delete_dispatcher(dispatcher);

	return exit_graphic_mode();
}



