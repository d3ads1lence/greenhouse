/*
 * TaskManager.cpp
 *
 *  Created on: Apr 19, 2020
 *      Author: user
 */

/* FreeRTOS related */
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "boards.h"

/**
 * The size of the stack for the Logger task (in 32-bit words).
 * Logger uses sprintf internally so it is a rather stack hungry process.
 */
#define LOGGER_STACK_SIZE 512
/**
 * The priority of the Logger task.
 */
#define LOGGER_PRIORITY 1

#if NRF_LOG_ENABLED
static TaskHandle_t m_logger_thread;      /**< Logger thread. */
#endif
static TaskHandle_t m_led_thread;		/**< Led thread. */


static void led_thread (void * pvParameter);
static void logger_thread(void * arg);
extern "C" {
void vApplicationIdleHook( void );
}


void TaskManager_Start(void)
{
#if NRF_LOG_ENABLED
    // Start execution.
    if (pdPASS != xTaskCreate(logger_thread,
                              "LOGGER",
                              LOGGER_STACK_SIZE,
                              NULL,
                              LOGGER_PRIORITY,
                              &m_logger_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }
#endif

    if (pdPASS != xTaskCreate(led_thread,
                              "LED",
							  configMINIMAL_STACK_SIZE + 200,
                              NULL,
                              2,
                              &m_led_thread))
    {
        APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
    }

    /* Activate deep sleep mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    NRF_LOG_INFO("Initialization end");

    // Start FreeRTOS scheduler.
    vTaskStartScheduler();
}

/**@brief LED0 task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
static void led_thread (void * pvParameter)
{
    UNUSED_PARAMETER(pvParameter);
    /* Toggle LEDs. */
    while (true)
    {
        for (int i = 0; i < LEDS_NUMBER; i++)
        {
            bsp_board_led_invert(i);
            vTaskDelay(5000);
        }
    }
}


#if NRF_LOG_ENABLED
/**@brief Thread for handling the logger.
 *
 * @details This thread is responsible for processing log entries if logs are deferred.
 *          Thread flushes all log entries and suspends. It is resumed by idle task hook.
 *
 * @param[in]   arg   Pointer used for passing some arbitrary information (context) from the
 *                    osThreadCreate() call to the thread.
 */
static void logger_thread(void * arg)
{
    UNUSED_PARAMETER(arg);

    while (1)
    {
        NRF_LOG_FLUSH();

        vTaskSuspend(NULL); // Suspend myself
    }
}
#endif //NRF_LOG_ENABLED

/**@brief A function which is hooked to idle task.
 * @note Idle hook must be enabled in FreeRTOS configuration (configUSE_IDLE_HOOK).
 */

void vApplicationIdleHook( void )
{
#if NRF_LOG_ENABLED
     vTaskResume(m_logger_thread);
#endif
}
