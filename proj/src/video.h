#ifndef __VIDEO_H
#define __VIDEO_H

#define BIT(n) (0x01 << (n))
#define VBE_CALL 0x4F00
#define VBE_SET_MODE 0x2
#define VBE_GET_MODE_INFO 0x1
#define VBE_GET_CONTROLLER_INFO 0x0
#define INT10 0x10
#define SUCCESSFUL_VBE_CALL 0x4F
#define MODE105 0x105
#define BLACK 0

#endif
