#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "i2c_api.h"
#include "ssd1306.h"
#include "Blynk_IoT.h"


#define I2C_ADDR_MAX30102 0x57
#define i2c_port 0
#define CONFIG_MOSI_GPIO 23
#define CONFIG_SCLK_GPIO 18
#define CONFIG_CS_GPIO 5
#define CONFIG_DC_GPIO 4 
#define i2c_frequency 800000


int irpower = 0,
    rpower = 0,
    lirpower = 0,
    lrpower = 0,
    finger_on_sensor = 0,
    frame_id = 0;

float heartrate = 0.0,
      pctspo2 = 100.0,
      meastime;

SSD1306_t display;

void max30102_init()
{
    i2c_init();
    uint8_t data;
    data = ( 0x2 << 5);  //sample averaging 0=1,1=2,2=4,3=8,4=16,5+=32
    i2c_write(I2C_ADDR_MAX30102, 0x08, data);
    data = 0x03;                //mode = red and ir samples
    i2c_write(I2C_ADDR_MAX30102, 0x09, data);
    data = ( 0x3 << 5) + ( 0x3 << 2 ) + 0x3; //first and last 0x3, middle smap rate 0=50,1=100,etc
    i2c_write(I2C_ADDR_MAX30102, 0x0a, data);
    data = 0xd0;                //ir pulse power
    i2c_write(I2C_ADDR_MAX30102, 0x0c, data);
    data = 0xa0;                //red pulse power
    i2c_write(I2C_ADDR_MAX30102, 0x0d, data);
}

void display_init()
{
  spi_master_init(&display, 23, 18, 5, 4, 2);
  ssd1306_init(&display, 128, 64);
  ssd1306_clear_screen(&display, false);
  ssd1306_contrast(&display, 0xff);
}

void clear_screen(int new_frame_id)
{
  if (new_frame_id != frame_id)
    ssd1306_clear_screen(&display, false);
}

int draw_bpm(int rate)
{
  char bpm[4];
  char oxy[18];
  int len = sprintf(bpm, "%d", rate);
  int lenoxy = sprintf(oxy, "Oxygen sat: %d%%", (int)pctspo2);

  int new_frame_id = 1;
  clear_screen(new_frame_id);
  ssd1306_display_text(&display, 0, "BPM:", 4, false);
  ssd1306_display_text_x3(&display, 2, bpm, len, false);
  ssd1306_display_text(&display, 6, oxy, lenoxy, false);

  // Gửi dữ liệu đến Blynk
  send_data_to_blynk(rate, (int)pctspo2);


  // Clear screen - In terminal, just update the text
  // printf("\n--- New Frame ---\n");
  
  // printf("BPM: %s\n", bpm);
  // printf("%s\n", oxy);

  return new_frame_id;
}

int draw_finger_req()
{
  int new_frame_id = 3;

  clear_screen(new_frame_id);
  ssd1306_display_text(&display, 0, "Start", 15, false);


  // Display message on monitor (terminal)
  //printf("\nPut your finger on the sensor.\n");

  return new_frame_id;
}

void draw_data()
{
  int rate;
  while (1)
  {
    rate = (int)heartrate;

    if (finger_on_sensor && rate)
      frame_id = draw_bpm(rate);
    else
      frame_id = draw_finger_req();

    // Add a small delay to prevent overwhelming the terminal with output
    vTaskDelay(pdMS_TO_TICKS(1000));  // Delay 1 second (adjust as needed)
  }
}

/**
 * @brief Taken from https://hackaday.io/project/164155-esp-heart-rate-monitor/details
 * but couple parts have been refactored.
 */
void max30102_task() {
    int cnt, samp, tcnt = 0;
    uint8_t rptr, wptr;
    uint8_t data;
    uint8_t regdata[256];
    //int irmeas, redmeas;
    float firxv[5], firyv[5], fredxv[5], fredyv[5];
    float lastmeastime = 0;
    float hrarray[10],spo2array[10];
    int hrarraycnt = 0;
    while (1) {
      if (lirpower != irpower) {
        data = (uint8_t) irpower;
        i2c_write(I2C_ADDR_MAX30102, 0x0c, data);
        lirpower = irpower;
      }
      if (lrpower != rpower) {
        data = (uint8_t) rpower;
        i2c_write(I2C_ADDR_MAX30102, 0x0d, data);
        lrpower = rpower;
      }
      i2c_read(I2C_ADDR_MAX30102, 0x04, &wptr, 1);
      i2c_read(I2C_ADDR_MAX30102, 0x06, &rptr, 1);

      samp = ((32 + wptr) - rptr) % 32;
      i2c_read(I2C_ADDR_MAX30102, 0x07, regdata, 6 * samp);

      for (cnt = 0; cnt < samp; cnt++) {
        meastime = 0.01 * tcnt++;
        firxv[0] = firxv[1];
        firxv[1] = firxv[2];
        firxv[2] = firxv[3];
        firxv[3] = firxv[4];
        firxv[4] = (1 / 3.48311) *
                   (256 * 256 * (regdata[6 * cnt + 0] % 4) + 256 * regdata[6 * cnt + 1] + regdata[6 * cnt + 2]);
        firyv[0] = firyv[1];
        firyv[1] = firyv[2];
        firyv[2] = firyv[3];
        firyv[3] = firyv[4];
        firyv[4] = (firxv[0] + firxv[4]) - 2 * firxv[2]
                   + (-0.1718123813 * firyv[0]) + (0.3686645260 * firyv[1])
                   + (-1.1718123813 * firyv[2]) + (1.9738037992 * firyv[3]);

        fredxv[0] = fredxv[1];
        fredxv[1] = fredxv[2];
        fredxv[2] = fredxv[3];
        fredxv[3] = fredxv[4];
        fredxv[4] = (1 / 3.48311) *
                    (256 * 256 * (regdata[6 * cnt + 3] % 4) + 256 * regdata[6 * cnt + 4] + regdata[6 * cnt + 5]);
        fredyv[0] = fredyv[1];
        fredyv[1] = fredyv[2];
        fredyv[2] = fredyv[3];
        fredyv[3] = fredyv[4];
        fredyv[4] = (fredxv[0] + fredxv[4]) - 2 * fredxv[2]
                    + (-0.1718123813 * fredyv[0]) + (0.3686645260 * fredyv[1])
                    + (-1.1718123813 * fredyv[2]) + (1.9738037992 * fredyv[3]);

        if (-1.0 * firyv[4] >= 100 && -1.0 * firyv[2] > -1 * firyv[0] && -1.0 * firyv[2] > -1 * firyv[4])
        {
          if (meastime - lastmeastime < 0.5)
            continue;
          if (!(finger_on_sensor == 0 && meastime - lastmeastime > 2))
            finger_on_sensor = 1;

          hrarray[hrarraycnt % 5] = 60 / (meastime - lastmeastime);
          spo2array[hrarraycnt % 5] = 110 - 25 * ((fredyv[4] / fredxv[4]) / (firyv[4] / firxv[4]));
          if (spo2array[hrarraycnt % 5] > 100)
            spo2array[hrarraycnt % 5] = 99.9;

          // In ra kết quả lên monitor (serial)
          //printf("Time: %6.2f  Interval: %4.2f     HR= %5.1f     SpO2= %5.1f\n", meastime, meastime - lastmeastime, heartrate, pctspo2);

          lastmeastime = meastime;
          hrarraycnt++;
          heartrate = (hrarray[0] + hrarray[1] + hrarray[2] + hrarray[3] + hrarray[4]) / 5;
          if (heartrate < 40 || heartrate > 150)
            heartrate = 0;

          pctspo2 = ((spo2array[0] + spo2array[1] + spo2array[2] + spo2array[3] + spo2array[4]) / 5) + 45;
          if (pctspo2 < 50 || pctspo2 > 101)
            pctspo2 = 0;
        }
        if (heartrate && lastmeastime + 1.8 < 0.01 * tcnt)
          finger_on_sensor = 0;
      }
    }
}

/**
 * @brief Main function to initialize MAX30102 and start task.
 */
void app_main()
{
  //configure max30102 with i2c instructions
  initialize_wifi();
  max30102_init();
  display_init();

  xTaskCreate(max30102_task, "max30102_task", 4096, NULL, 5, NULL);
  xTaskCreate(draw_data, "draw_data_task", 4096, NULL, 5, NULL);
  //esp_restart();
}
