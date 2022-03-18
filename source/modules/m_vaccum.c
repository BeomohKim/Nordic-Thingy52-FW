#include "m_vaccum.h"
#include "sdk_config.h"
#include "app_timer.h"
#include "ble_nus.h"
#include <stdint.h>
#include <string.h>

#define  NRF_LOG_MODULE_NAME "m_vaccum   "
#include "nrf_log.h"
#include "macros_common.h"

ble_nus_t                    m_nus;                          // Structure to identify the NUS service.
static m_vaccum_event_handler_t     m_evt_handler;                  // Event handler function pointer.
static vaccum_param_t               m_vaccum_param;                 // vaccum parameters.
static bool                         m_ble_nus_configured = false;   // Has the BLE battery service been initalized?

/** @brief Timer for periodic battery measurement.
 */
APP_TIMER_DEF(vaccum_app_timer_id);

/**@brief Function for passing the BLE event to the vaccum module.
 *
 * @details This callback function will be called from the BLE handling module.
 *
 * @param[in] p_ble_evt    Pointer to the BLE event.
 */
static void vaccum_on_ble_evt(ble_evt_t * p_ble_evt)
{
    ble_nus_on_ble_evt(&m_nus, p_ble_evt);
}


static void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length)
{
    NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.\r\n");
    NRF_LOG_HEXDUMP_DEBUG(p_data, length);
}


/**@brief Function for initializing the Thingy Battery Service.
 *
 * @details This callback function will be called from the ble handling module to initialize the Battery service.
 *
 * @retval NRF_SUCCESS If initialization was successful.
 */
static uint32_t vaccum_service_init(bool major_minor_fw_ver_changed)
{
    uint32_t       err_code;
    ble_nus_init_t nus_init;

    memset(&nus_init, 0, sizeof(nus_init));

    NRF_LOG_DEBUG("vaccum_service_init: ble_nus_init \r\n ");

    
    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    NRF_LOG_DEBUG("ble_nus_init %d \r\n ", err_code);

    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEBUG("ble_nus_init passed \r\n ");

    m_ble_nus_configured = true;

    return NRF_SUCCESS;
}

static void app_timer_periodic_handler(void* temp)
{
    static uint8_t i = 'a', j = 'a';
    uint32_t err_code;

    uint8_t data[16];
    uint16_t len;

    data[0] = 0xaa;
    data[1] = 0x55;    
    data[2] = 3;
    data[3] = i;
    data[4] = j;
    data[5] = 256-(data[0]+data[1]+data[2]+data[3]+data[4]);
    data[6] = 0x0d;
    data[7] = 0x0a;
    
    i = i+1;
    j = j+1;

    if (i == 'e') i = 'a';
    if (j == 'v') j = 'a';

    len = 8;

    NRF_LOG_INFO("NUS App timer \r\n");
  
    do
    {
        err_code = ble_nus_string_send(&m_nus, data, len);
        NRF_LOG_INFO("NUS send error %d \r\n", err_code);
        if ( (err_code != NRF_ERROR_INVALID_STATE) && (err_code != NRF_ERROR_BUSY) )
        {
            
        }
    } while (err_code == NRF_ERROR_BUSY);

}

uint32_t m_vaccum_init(m_ble_service_handle_t * p_handle, m_vaccum_init_t const * const p_vaccum_init)
{
    uint32_t err_code;

    NULL_PARAM_CHECK(p_handle);
    NULL_PARAM_CHECK(p_vaccum_init);

    NRF_LOG_DEBUG("Init \r\n");

    p_handle->ble_evt_cb = vaccum_on_ble_evt;
    p_handle->init_cb    = vaccum_service_init;  // Pointer to ble init function.

    m_evt_handler = p_vaccum_init->evt_handler;
    m_vaccum_param = p_vaccum_init->vaccum_param;

    //set gpio config
    #if 1
    app_timer_periodic_handler(NULL);

    err_code = app_timer_create(&vaccum_app_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                app_timer_periodic_handler);
    RETURN_IF_ERROR(err_code);

    err_code = app_timer_start(vaccum_app_timer_id,
                               APP_TIMER_TICKS(1000), NULL);
    RETURN_IF_ERROR(err_code);
    #else
    UNUSED_VARIABLE(err_code);
    UNUSED_VARIABLE(app_timer_periodic_handler);
    UNUSED_VARIABLE(vaccum_app_timer_id);
    #endif

    return NRF_SUCCESS;
}
