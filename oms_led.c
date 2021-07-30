//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     oms_led.c
///
///  \brief    Operation manual switch handler. State machine for application switch
///
///  \author   <BrT> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#include <wago_oms_API.h>
#include <diagnostic_API.h>
#include <led_server.h>
#include <unistd.h>
#include <pthread.h>

#include "oms_led.h"
#include "utils.h"
#include "kbus-daemon.h"
#include "kbus.h"
#include "modbus.h"

static pthread_t oms_led_thread;
static char oms_led_task_running = 1;
static tOmsDevice *omsDev = NULL;

#define OMS_RUN 0x01
#define OMS_STOP 0x02
#define OMS_RESET 0x04
#define OMS_ERROR 0x08

#define RUN_COLOR_OFF     0x00040004
#define RUN_COLOR_GREEN   0x00040000
#define RUN_COLOR_BLINK   0x00040005
#define RUN_COLOR_RESET   0x00040001

/**
 * @brief Setting the RUN LED with the defined event-ids.
 * @param[in] color - event-id which executes the specified led color pattern.
 */
static inline void setRunLEDColor(int color)
{
   log_EVENT_LogId(color, true);
}

/**
 * @brief Reading Operation Manual Switch and returning the actual state.
 * @param[in] dev - tOmsDevice device
 * @return actual switch state or error state
 * @retval 0x01 RUN state
 * @retval 0x02 STOP state
 * @retval 0x04 RESET state
 * @retval 0x08 ERROR
 */
static int readOMS(tOmsDevice *dev)
{
    int32_t switchState     = 0;

    if (dev == NULL)
    {
        return OMS_ERROR;
    }

    if(OMS_RETURN_OK == dev->GetRunKey(dev, &switchState))
    {
        if(switchState == 1) // Switch in position "RUN"
        {
            return OMS_RUN;
        }
    }
    else
    {
        return OMS_ERROR;
    }

    if(OMS_RETURN_OK == dev->GetStopKey(dev, &switchState))
    {
        if (switchState == 1) // Switch in position "STOP"
        {
            return OMS_STOP;
        }
    }
    else
    {
        return OMS_ERROR;
    }

    if (OMS_RETURN_OK == dev->GetResetKey(dev, &switchState))
    {
        if(switchState == 1) // Switch in position "RESET"
        {
            return OMS_RESET;
        }
    }
    // Will be only executed if GetKeyReset failes.
    return OMS_ERROR;
}

/**
 * @brief Function for state machine to set the correct behavior if OMS is switched to stop.
 */
static void oms_led_Application_STOP(void)
{
    dprintf(VERBOSE_STD, "--> Application in STOP mode\n");
    if (kbus_ApplicationStateStop() < 0)
    {
        dprintf(VERBOSE_STD, "Error\n");
    }
    modbus_ApplicationStateStop();
}

/**
 * @brief Function for state machine to set the correct behavior if OMS is switched to run.
 */
static void oms_led_Application_RUN(void)
{
    dprintf(VERBOSE_STD, "--> Application in RUN mode\n");
    if (kbus_ApplicationStateRun() < 0)
    {
        dprintf(VERBOSE_STD, "Error\n");
    }
    modbus_ApplicationStateRun();
}

static void *oms_led_task(void *none)
{
    UNUSED(none);

#define SWITCH_STATE_RUN 0x01
#define SWITCH_STATE_STOP   0x02
#define SWITCH_STATE_RESET  0x04
#define SWITCH_STATE_INIT   0x08
#define SWITCH_STATE_FIRST_ENTRY 0x80
    static uint8_t switchStateMaschine = SWITCH_STATE_INIT | SWITCH_STATE_FIRST_ENTRY;
    static uint8_t oldswitchStateMaschine;
    static uint8_t resetCounter = 0;
    while(oms_led_task_running)
    {
        oldswitchStateMaschine = switchStateMaschine;

        uint8_t switchPos = readOMS(omsDev);
        //Switch State Maschine
        switch(switchStateMaschine & ~SWITCH_STATE_FIRST_ENTRY)
        {
         case SWITCH_STATE_INIT:
          if (switchStateMaschine & SWITCH_STATE_FIRST_ENTRY)
          {
              dprintf(VERBOSE_STD, "-->Init\n");
              setRunLEDColor(RUN_COLOR_OFF);
          }
          if (switchPos == OMS_RUN)
              switchStateMaschine = SWITCH_STATE_RUN;
          else if (switchPos == OMS_STOP)
              switchStateMaschine = SWITCH_STATE_STOP;
          else if (switchPos == OMS_RESET)
              switchStateMaschine = SWITCH_STATE_RESET;
          break;
         case SWITCH_STATE_RUN:
          if (switchStateMaschine & SWITCH_STATE_FIRST_ENTRY)
          {
              dprintf(VERBOSE_STD, "--> RUN\n");
              setRunLEDColor(RUN_COLOR_GREEN);
              oms_led_Application_RUN();
          }
          if (switchPos == OMS_STOP)
              switchStateMaschine = SWITCH_STATE_STOP;
          else if (switchPos == OMS_RESET)
              switchStateMaschine = SWITCH_STATE_RESET;
          break;
         case SWITCH_STATE_STOP:
          if (switchStateMaschine & SWITCH_STATE_FIRST_ENTRY)
          {
              dprintf(VERBOSE_STD, "--> STOP\n");
              setRunLEDColor(RUN_COLOR_BLINK);
              oms_led_Application_STOP();
          }
          if (switchPos == OMS_RUN)
              switchStateMaschine = SWITCH_STATE_RUN;
          else if (switchPos == OMS_RESET)
              switchStateMaschine = SWITCH_STATE_RESET;
          break;
         case SWITCH_STATE_RESET:
          if (switchStateMaschine & SWITCH_STATE_FIRST_ENTRY)
          {
              dprintf(VERBOSE_STD, "--> RESET\n");
              setRunLEDColor(RUN_COLOR_RESET);
              resetCounter = 0;
          }

          if (resetCounter < 30)
          {
              resetCounter++;
              if (resetCounter >= 30)
              {
                  fprintf(stderr, "--> Execute RESET\n");
                  main_shutdownModules();
                  main_startUpModules();
              }
          }

          if (switchPos == OMS_RUN)
              switchStateMaschine = SWITCH_STATE_RUN;
          else if (switchPos == OMS_STOP)
              switchStateMaschine = SWITCH_STATE_STOP;
          break;

         default:
          break;
        }

        //Clear first-entry-flag
        if (oldswitchStateMaschine == switchStateMaschine)
        {
            if ( switchStateMaschine & SWITCH_STATE_FIRST_ENTRY)
            {
                switchStateMaschine &= ~SWITCH_STATE_FIRST_ENTRY;
            }
        }
        else // set first-entry-flag
        {
            switchStateMaschine |= SWITCH_STATE_FIRST_ENTRY;
        }
        usleep(100*1000);
    }
    return NULL;
}

int oms_led_start(void)
{
    oms_led_task_running = 1;
    //Open Operating Mode Switch device
    omsDev = oms_OpenDevice("/dev/input/event0", OMS_MODE_PASSIVE);
    if(omsDev == NULL)
    {
        printf("ERROR: oms_OpenDevice() failes\n");
        return -1;
    }

    log_EVENT_Init("kbusmodbusslave"); //Init eventlogging with program name

    if (pthread_create(&oms_led_thread, NULL,
                   &oms_led_task, NULL) != 0)
    {
        return -2;
    }

    return 0;
}

void oms_led_stop(void)
{
    oms_led_task_running = 0;
    pthread_join(oms_led_thread, NULL);
    oms_CloseDevice(omsDev);
}
