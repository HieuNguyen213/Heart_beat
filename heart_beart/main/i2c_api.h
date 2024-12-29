#ifndef I2C_API_H
#define I2C_API_H

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"


#define TAG "SSD1306"


#define MAX30102_I2C_ADDR 0x57

#define i2c_freq 400000
#define I2C_PORT 0
#define I2C_MASTER_FREQ_HZ i2c_freq
#define I2C_TICKS_TO_WAIT 100	
#define I2C_NUM I2C_PORT
#define CONFIG_OFFSETX 0

#define SDA_PIN 21
#define SCL_PIN 22

#define I2C_MASTER_TX_BUF_DISABLE                  0                       
#define I2C_MASTER_RX_BUF_DISABLE                  0                       
#define WRITE_BIT                   I2C_MASTER_WRITE          
#define READ_BIT                     I2C_MASTER_READ          
#define ACK_CHECK_EN                             0x1                 
#define ACK_CHECK_DIS                            0x0               
#define ACK_VAL                                  0x0                    
#define NACK_VAL                                 0x1                  
#define LAST_NACK_VAL                            0x2  


esp_err_t i2c_init(void);
esp_err_t i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, size_t length);
esp_err_t i2c_write(uint8_t addr, uint8_t reg, uint8_t data);


#endif
