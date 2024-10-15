#pragma once

#include "esp_timer.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/touch_pad.h>
#include <driver/gpio.h>
#include <unordered_map>
#include <functional>
#include <set>
#include <vector>
#include <string>

enum SpecKeys
{
  SPECKEY_NONE,
  SPECKEY_1,
  SPECKEY_2,
  SPECKEY_3,
  SPECKEY_4,
  SPECKEY_5,
  SPECKEY_6,
  SPECKEY_7,
  SPECKEY_8,
  SPECKEY_9,
  SPECKEY_0,
  SPECKEY_Q,
  SPECKEY_W,
  SPECKEY_E,
  SPECKEY_R,
  SPECKEY_T,
  SPECKEY_Y,
  SPECKEY_U,
  SPECKEY_I,
  SPECKEY_O,
  SPECKEY_P,
  SPECKEY_A,
  SPECKEY_S,
  SPECKEY_D,
  SPECKEY_F,
  SPECKEY_G,
  SPECKEY_H,
  SPECKEY_J,
  SPECKEY_K,
  SPECKEY_L,
  SPECKEY_ENTER,
  SPECKEY_SHIFT,
  SPECKEY_Z,
  SPECKEY_X,
  SPECKEY_C,
  SPECKEY_V,
  SPECKEY_B,
  SPECKEY_N,
  SPECKEY_M,
  SPECKEY_SYMB,
  SPECKEY_SPACE,
  JOYK_UP,
  JOYK_DOWN,
  JOYK_LEFT,
  JOYK_RIGHT,
  JOYK_FIRE,
  // special pseudo keys
  SPECKEY_DEL,
  SPECKEY_BREAK,
};

const std::unordered_map<SpecKeys, char> specKeyToLetter = {
  { SPECKEY_0, '0' },
  { SPECKEY_1, '1' },
  { SPECKEY_2, '2' },
  { SPECKEY_3, '3' },
  { SPECKEY_4, '4' },
  { SPECKEY_5, '5' },
  { SPECKEY_6, '6' },
  { SPECKEY_7, '7' },
  { SPECKEY_8, '8' },
  { SPECKEY_9, '9' },
  { SPECKEY_A, 'A' },
  { SPECKEY_B, 'B' },
  { SPECKEY_C, 'C' },
  { SPECKEY_D, 'D' },
  { SPECKEY_E, 'E' },
  { SPECKEY_F, 'F' },
  { SPECKEY_G, 'G' },
  { SPECKEY_H, 'H' },
  { SPECKEY_I, 'I' },
  { SPECKEY_J, 'J' },
  { SPECKEY_K, 'K' },
  { SPECKEY_L, 'L' },
  { SPECKEY_M, 'M' },
  { SPECKEY_N, 'N' },
  { SPECKEY_O, 'O' },
  { SPECKEY_P, 'P' },
  { SPECKEY_Q, 'Q' },
  { SPECKEY_R, 'R' },
  { SPECKEY_S, 'S' },
  { SPECKEY_T, 'T' },
  { SPECKEY_U, 'U' },
  { SPECKEY_V, 'V' },
  { SPECKEY_W, 'W' },
  { SPECKEY_X, 'X' },
  { SPECKEY_Y, 'Y' },
  { SPECKEY_Z, 'Z' },
};

const int TOUCH_CALIBRATION_SAMPLES = 100;
const float TOUCH_THRESHOLD = 0.1;

class TouchKeyboardV2
{
private:
  // this is used for key down and key up events
  using KeyEventType = std::function<void(SpecKeys keyCode, bool isPressed)>;
  KeyEventType m_keyEvent;
  // this is used for key pressed evens and supports repeating keys
  using KeyPressedEventType = std::function<void(SpecKeys keyCode)>;
  KeyPressedEventType m_keyPressedEvent;

  // touch pad calibration values
  uint32_t calibrations[8][5] = {0};
  // all the touch pads we are using
  const touch_pad_t touchPads[5] = {TOUCH_PAD_NUM13, TOUCH_PAD_NUM12, TOUCH_PAD_NUM11, TOUCH_PAD_NUM10, TOUCH_PAD_NUM9};

  void setupTouchPad()
  {
    touch_pad_init();
    // this is the magic line to make it go faster... no real idea what these values really do...
    touch_pad_set_charge_discharge_times(4);
    touch_pad_set_measurement_interval(50);
    // touch_pad_set_meas_time(4, 50);
    touch_pad_set_voltage(TOUCH_PAD_HIGH_VOLTAGE_THRESHOLD, TOUCH_PAD_LOW_VOLTAGE_THRESHOLD, TOUCH_PAD_ATTEN_VOLTAGE_THRESHOLD);
    touch_pad_set_idle_channel_connect(TOUCH_PAD_IDLE_CH_CONNECT_DEFAULT);
    touch_pad_denoise_t denoise = {
        .grade = TOUCH_PAD_DENOISE_BIT4,
        .cap_level = TOUCH_PAD_DENOISE_CAP_L4,
    };
    touch_pad_denoise_set_config(&denoise);
    touch_pad_denoise_enable();
    // Touch Sensor Timer initiated
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER); // returns ESP_OK
    touch_pad_fsm_start();
    for (int touchPadIdx = 0; touchPadIdx < 5; touchPadIdx++)
    {
      touch_pad_config(touch_pad_t(touchPads[touchPadIdx]));
    }
  }

  void calibrate()
  {
    printf("**** Calibrating touch pads\n");
    for (int i = 0; i < 8; i++)
    {
      gpio_set_level(GPIO_NUM_14, (i & 1) ? 1 : 0);
      gpio_set_level(GPIO_NUM_21, (i & 2) ? 1 : 0);
      gpio_set_level(GPIO_NUM_47, (i & 4) ? 1 : 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      // calibration - take the average of 5 readings
      for (int touchPadIdx = 0; touchPadIdx < 5; touchPadIdx++)
      {
        unsigned long aveTouch = 0;
        for (int j = 0; j < 5; j++)
        {
          uint32_t touch;
          touch_pad_read_raw_data(touch_pad_t(touchPads[touchPadIdx]), &touch);
          aveTouch += touch;
        }
        calibrations[i][touchPadIdx] = aveTouch / 5;
        printf("Calibration[%d][%d] = %ld\n", i, touchPadIdx, calibrations[i][touchPadIdx]);
      }
    }
  }

  static void keyboardTask(void *arg);

public:
  // mapping from address to bit to spectrum key
  // we have 8 addresses and 5 bits per address
  const SpecKeys rows[8][5] = {
      // FEFE
      {SPECKEY_SHIFT, SPECKEY_Z, SPECKEY_X, SPECKEY_C, SPECKEY_V},
      // FBFE
      {SPECKEY_Q, SPECKEY_W, SPECKEY_E, SPECKEY_R, SPECKEY_T},
      // F7FE
      {SPECKEY_1, SPECKEY_2, SPECKEY_3, SPECKEY_4, SPECKEY_5},
      // FDFE
      {SPECKEY_A, SPECKEY_S, SPECKEY_D, SPECKEY_F, SPECKEY_G},
      // EFFE
      {SPECKEY_0, SPECKEY_9, SPECKEY_8, SPECKEY_7, SPECKEY_6},
      // BFFE
      {SPECKEY_ENTER, SPECKEY_L, SPECKEY_K, SPECKEY_J, SPECKEY_H},
      // 7FFE
      {SPECKEY_SPACE, SPECKEY_SYMB, SPECKEY_M, SPECKEY_N, SPECKEY_B},
      // DFFE
      {SPECKEY_P, SPECKEY_O, SPECKEY_I, SPECKEY_U, SPECKEY_Y}};

  // this records the time the key was last pressed - 0 indicates it was never pressed
  std::unordered_map<SpecKeys, int> isKeyPressed = {
      {SPECKEY_1, 0},
      {SPECKEY_2, 0},
      {SPECKEY_3, 0},
      {SPECKEY_4, 0},
      {SPECKEY_5, 0},
      {SPECKEY_6, 0},
      {SPECKEY_7, 0},
      {SPECKEY_8, 0},
      {SPECKEY_9, 0},
      {SPECKEY_0, 0},
      {SPECKEY_Q, 0},
      {SPECKEY_W, 0},
      {SPECKEY_E, 0},
      {SPECKEY_R, 0},
      {SPECKEY_T, 0},
      {SPECKEY_Y, 0},
      {SPECKEY_U, 0},
      {SPECKEY_I, 0},
      {SPECKEY_O, 0},
      {SPECKEY_P, 0},
      {SPECKEY_A, 0},
      {SPECKEY_S, 0},
      {SPECKEY_D, 0},
      {SPECKEY_F, 0},
      {SPECKEY_G, 0},
      {SPECKEY_H, 0},
      {SPECKEY_J, 0},
      {SPECKEY_K, 0},
      {SPECKEY_L, 0},
      {SPECKEY_ENTER, 0},
      {SPECKEY_SHIFT, 0},
      {SPECKEY_Z, 0},
      {SPECKEY_X, 0},
      {SPECKEY_C, 0},
      {SPECKEY_V, 0},
      {SPECKEY_B, 0},
      {SPECKEY_N, 0},
      {SPECKEY_M, 0},
      {SPECKEY_SYMB, 0},
      {SPECKEY_SPACE, 0}};

  TouchKeyboardV2(KeyEventType keyEvent, KeyPressedEventType keyPressEvent) : m_keyEvent(keyEvent), m_keyPressedEvent(keyPressEvent) {};

  void start()
  {
    gpio_set_direction(GPIO_NUM_14, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_47, GPIO_MODE_OUTPUT);
    // initialise the touch pads
    setupTouchPad();
    calibrate();
    // start the task
    xTaskCreatePinnedToCore(
        keyboardTask,
        "keyboardTask",
        4096,
        this,
        1,
        NULL,
        0);
  }
  void setToggleMode(bool toggle) {}
};