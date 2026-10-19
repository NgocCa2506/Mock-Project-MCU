/*
 * boot_validate.h
 *
 *  Decide whether the Application in Flash can be started.
 *  Checks the Application Header (Magic Number, size, CRC) and the
 *  Reset Handler address.
 */

#ifndef INC_BOOT_VALIDATE_H_
#define INC_BOOT_VALIDATE_H_

typedef enum
{
    APP_VALID = 0,
    APP_MAGIC_NUMBER_INVALID,
    APP_SIZE_INVALID,
    APP_CRC_INVALID,
    APP_RESET_HANDLER_INVALID

} app_status_t;

app_status_t validate_app(void);

#endif /* INC_BOOT_VALIDATE_H_ */
