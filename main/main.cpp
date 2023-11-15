/**
 * @mainpage 
 * @file main.cpp
 * @author Errol Joshua (errol@tinyprismlabs.com)
 * @brief File contains all main initializations for the operations and as well as control of different operation required to run the program
 * @version 0.1
 * @date 2023-11-15
 * 
 * @copyright Copyright (c) 2023 Tiny Prism Labs Private Limited <contact@tinyprismlabs.com>
 * 
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "classifier.cpp"
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "esp_log.h"
#include "camera.cpp"

uint8_t *snapshot_buf;
TaskHandle_t cameraTaskHandler;

/**
 * @brief This funtion call attached to ```cameraTaskHandler```
 * 
 * This is initialize the camera module, capture a image and then call classify function to run ML inference. This is created using xTaskCreatePinnedToCore
 * and is pinned to core 0 of ESP32-S3. Please refer https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/system/freertos_idf.html for more info.
 * 
 * @param args empty parameter
 */
void xCamera(void *args)
{

    if (ESP_OK != init_camera())
    {
        return;
    }

    while (1)
    {
        snapshot_buf = (uint8_t *)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
        take_picture(snapshot_buf);
        classify(snapshot_buf);

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}


/**
 * @brief Main funtion
 * 
 * This main fn is accompanied by extern as ESP32-S3 toolchain takes only c files
 * 
 */
extern "C" void app_main()
{
    xTaskCreatePinnedToCore(xCamera, "camera_classify", 2048 * 2, NULL, 10, cameraTaskHandler, 0); 

}
