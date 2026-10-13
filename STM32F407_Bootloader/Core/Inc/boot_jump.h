/*
 * boot_jump.h
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 */

#ifndef INC_BOOT_JUMP_H_
#define INC_BOOT_JUMP_H_

enum {
	APP_VALID = 0,
	APP_RESET_HANDLER_INVALID
};

int validate_app(void);
void jump_to_app(void);

#endif /* INC_BOOT_JUMP_H_ */
