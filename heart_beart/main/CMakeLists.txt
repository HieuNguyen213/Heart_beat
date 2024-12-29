
idf_component_register(SRCS "Blynk_IoT.c" "i2c_api.c" "spi_api.c" "ssd1306.c" "main.c"
                    INCLUDE_DIRS "")
# Thêm tùy chọn tắt cảnh báo cho toàn bộ component
add_compile_options(-Wno-format)                   
