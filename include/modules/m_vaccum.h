 /** @file A9sense vaccum module
 *
 *
 */

#ifndef __M_VACCUM_MEAS_H__
#define __M_VACCUM_MEAS_H__

#include "m_ble.h"
#include "ble_nus.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    VACCUM_STATE_CHANGED,
    VACCUM_LEVEL_CHANGED    
}vaccum_event_type;

typedef struct
{
    vaccum_event_type type;
    uint8_t     value;
    /* data */
}m_vaccum_event_t;

/** @brief m_batt sensor event handler type. Should be implemented by user e.g. in main.c()
 */
typedef void (*m_vaccum_event_handler_t)(m_vaccum_event_t const * p_event);

/** @brief Input parameters for m_vaccum.
 */
typedef struct
{
    uint8_t             pwmin_pin_no;
    uint8_t             pwmout_pin_no;
}vaccum_param_t;

/** @brief Init parameters for m_vaccum.
 */
typedef struct
{
    m_vaccum_event_handler_t     evt_handler;        ///< Function pointer to the event handler (executed in main context).
    vaccum_param_t               vaccum_param;    ///< Input parameters.
}m_vaccum_init_t;

extern ble_nus_t m_nus;

uint32_t m_vaccum_init(m_ble_service_handle_t * p_handle, m_vaccum_init_t const * const p_vaccum_init);

#endif

/** @} */
