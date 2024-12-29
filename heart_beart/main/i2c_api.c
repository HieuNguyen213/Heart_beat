#include "i2c_api.h"


// Hàm khởi tạo I2C, chỉ cần cài đặt 1 lần khi bắt đầu
esp_err_t i2c_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_freq
    };
    return i2c_param_config(I2C_PORT, &conf);
}

// // Hàm đọc dữ liệu từ I2C
esp_err_t i2c_read(uint8_t chip_addr, uint8_t data_addr, uint8_t *data_rd, size_t len)
{
    i2c_init();
    vTaskDelay(1);
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, 
                       I2C_MASTER_TX_BUF_DISABLE, 0);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | READ_BIT, ACK_CHECK_EN);
    if (len > 1) {
        i2c_master_read(cmd, data_rd, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd +len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) { 
         //for(int i=0; i<len; i++){ printf ("%d %x\n", i, data_rd[i]); }
    }
    else if (ret == ESP_ERR_TIMEOUT) { ESP_LOGW("", "Bus is busy");}
    else { ESP_LOGW("", "Read failed"); }
    i2c_driver_delete(I2C_PORT);
    vTaskDelay(2);
    return 0;
}

// // Hàm ghi dữ liệu vào I2C
esp_err_t i2c_write(uint8_t chip_addr, uint8_t data_addr, uint8_t wr_data)
{
    i2c_init();
    i2c_driver_install(I2C_PORT, I2C_MODE_MASTER, I2C_MASTER_RX_BUF_DISABLE, 
                       I2C_MASTER_TX_BUF_DISABLE, 0);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, wr_data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_OK) { //ESP_LOGI("", "Write OK addr %x  data %x\n", chip_addr, wr_data);
    } else if (ret == ESP_ERR_TIMEOUT) { ESP_LOGW("", "Bus is busy");
    } else { ESP_LOGW("", "Write Failed"); }
    i2c_driver_delete(I2C_PORT);
    return 0;
}

// Hàm xóa I2C driver (nếu cần thiết)
void i2c_cleanup()
{
    i2c_driver_delete(I2C_PORT);  // Xoá driver I2C
}
