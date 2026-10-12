/*
 * app_validate.h
 *
 *  Created on: Oct 12, 2026
 *      Author: CA
 */

#ifndef INC_APP_VALIDATE_H_
#define INC_APP_VALIDATE_H_

typedef enum
{
    APP_INVALID = 0,
    APP_VALID
} app_status_t;

app_status_t validate_app(void);

#endif /* INC_APP_VALIDATE_H_ */
