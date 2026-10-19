/*
 * app_header_validate.h
 *
 *  Read and validate the Application Header stored at APP_HEADER_START_ADDRESS.
 *  The header format itself is defined in app_header.h.
 */

#ifndef INC_APP_HEADER_VALIDATE_H_
#define INC_APP_HEADER_VALIDATE_H_

#include <stdint.h>

#include "app_header.h"

typedef enum
{
    APP_HEADER_VALID = 0,

    APP_HEADER_MAGIC_INVALID,

    APP_HEADER_SIZE_INVALID,

    APP_HEADER_CRC_INVALID

} app_header_status_t;

const app_header_t *app_header_get(void);

/*
 * Validate Magic Number, Size and CRC.
 */
app_header_status_t app_header_validate(void);

#endif /* INC_APP_HEADER_VALIDATE_H_ */
