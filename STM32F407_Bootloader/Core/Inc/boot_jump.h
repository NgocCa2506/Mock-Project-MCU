#ifndef INC_BOOT_JUMP_H_
#define INC_BOOT_JUMP_H_

enum
{
    APP_VALID = 0,
    APP_MAGIC_NUMBER_INVALID,
    APP_SIZE_INVALID,
    APP_CRC_INVALID,
    APP_RESET_HANDLER_INVALID
};

int validate_app(void);
void jump_to_app(void);

#endif /* INC_BOOT_JUMP_H_ */
