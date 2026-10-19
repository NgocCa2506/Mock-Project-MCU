/*
 * boot_validate.c
 *
 *  Decide whether the Application in Flash can be started.
 */

#include <stdint.h>

#include "boot_validate.h"
#include "flash_layout.h"
#include "app_header_validate.h"

app_status_t validate_app(void)
{
    app_header_status_t header_status;
    uint32_t reset_handler;

    /* Check Magic Number[5], App Size and App CRC first. */
    header_status = app_header_validate();

    if (header_status == APP_HEADER_MAGIC_INVALID)
    {
        return APP_MAGIC_NUMBER_INVALID;
    }

    if (header_status == APP_HEADER_SIZE_INVALID)
    {
        return APP_SIZE_INVALID;
    }

    if (header_status == APP_HEADER_CRC_INVALID)
    {
        return APP_CRC_INVALID;
    }

    /* Reset Handler is the second word of the Application vector table. */
    reset_handler = *(volatile const uint32_t *)
        (APP_START_ADDRESS + RESET_HANDLER_OFFSET);

    /* Keep the current Flash sanity check. */
    if ((reset_handler & 0xFF000000UL) != FLASH_START_ADDRESS)
    {
        return APP_RESET_HANDLER_INVALID;
    }

    return APP_VALID;
}
