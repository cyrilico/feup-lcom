#include <limits.h>
#include <string.h>
#include <errno.h>
#include <minix/drivers.h>
#include <minix/sysutil.h>
#include "keyboard.h"
#include "timer.h"
#include "i8254.h"
#include "utils.h"

int hookid_kbd = 10;
int SCROLLLOCK_ON = 0;
int NUMLOCK_ON = 0;
int CAPSLOCK_ON = 0;

int kbd_subscribe_int() {
	/*Variable that will hold return value in case of successful call, since sys_irq calls will modify hookid value*/
	int hookid_kbd_bit = BIT(hookid_kbd);

	if(sys_irqsetpolicy(IRQ_KBD, IRQ_EXCLUSIVE | IRQ_REENABLE, &hookid_kbd) != 0)
		return -1;

	if(sys_irqenable(&hookid_kbd) != 0)
		return -1;

	return hookid_kbd_bit;
}

int kbd_unsubscribe_int() {

	if(sys_irqdisable(&hookid_kbd) != 0)
		return -1;

	if(sys_irqrmpolicy(&hookid_kbd) != 0)
		return -1;

	return 0;
}

char scancode_to_letter(unsigned long code){
	switch(code){
	case A_BREAK:
		return 'a';
	case B_BREAK:
		return 'b';
	case C_BREAK:
		return 'c';
	case D_BREAK:
		return 'd';
	case E_BREAK:
		return 'e';
	case F_BREAK:
		return 'f';
	case G_BREAK:
		return 'g';
	case H_BREAK:
		return 'h';
	case I_BREAK:
		return 'i';
	case J_BREAK:
		return 'j';
	case K_BREAK:
		return 'k';
	case L_BREAK:
		return 'l';
	case M_BREAK:
		return 'm';
	case N_BREAK:
		return 'n';
	case O_BREAK:
		return 'o';
	case P_BREAK:
		return 'p';
	case Q_BREAK:
		return 'q';
	case R_BREAK:
		return 'r';
	case S_BREAK:
		return 's';
	case T_BREAK:
		return 't';
	case U_BREAK:
		return 'u';
	case V_BREAK:
		return 'v';
	case W_BREAK:
		return 'w';
	case X_BREAK:
		return 'x';
	case Y_BREAK:
		return 'y';
	case Z_BREAK:
		return 'z';
	default:
		return 0;
	}
}

Keyboard* create_keyboard(){
	Keyboard* keyboard = (Keyboard*)(malloc(sizeof(Keyboard)));
	keyboard->scancode = 0;
	keyboard->scancode_aux = 0;
	keyboard->codestatus = FULLY_READ;

	return keyboard;
}

void read_scancode(Keyboard* keyboard){
	unsigned long code = read_scancode_asm();
	if(code != -1){
		if(keyboard->codestatus == READ_AGAIN){
			code = code << 8;
			code |= keyboard->scancode_aux;
			keyboard->scancode = code;
			keyboard->codestatus = FULLY_READ;
		}
		else if(code == TWO_BYTE_SCANCODE){
			keyboard->scancode_aux = code;
			keyboard->codestatus = READ_AGAIN;
		}
		else
			keyboard->scancode = code;
	}
}

int full_scancode_received(Keyboard* keyboard){
	if(keyboard->codestatus == FULLY_READ)
		return 1;
	else
		return 0;
}

int key_detected(Keyboard* keyboard, unsigned long key){
	if(full_scancode_received(keyboard) && keyboard->scancode == key)
		return 1;
	else
		return 0;
}

void delete_keyboard(Keyboard* keyboard){
	free(keyboard);
}
