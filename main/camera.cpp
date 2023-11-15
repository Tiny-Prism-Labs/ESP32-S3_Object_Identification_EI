/**
 * @file camera.cpp
 * @author Errol Joshua (errol@tinyprismlabs.com)
 * @brief This file contains main camera operations from initialization to capturing images and processing it into desired format for inferencing
 * @version 0.1
 * @date 2023-11-15
 * 
 * 
 */
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "model-parameters/model_metadata.h"

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 240
#define EI_CAMERA_FRAME_BYTE_SIZE 3


static bool debug_nn = false; 
static bool is_initialised = false;
static const char *TAG = "example:take_picture";
/**
 * @brief Pins defination of OV2640 camera with formats for the image and other parameters
 * 
 */
static camera_config_t camera_config = {
    .pin_pwdn = -1,
    .pin_reset = -1,
    .pin_xclk = 10,
    .pin_sccb_sda = 40,
    .pin_sccb_scl = 39,

    .pin_d7 = 48,
    .pin_d6 = 11,
    .pin_d5 = 12,
    .pin_d4 = 14,
    .pin_d3 = 16,
    .pin_d2 = 18,
    .pin_d1 = 17,
    .pin_d0 = 15,
    .pin_vsync = 38,
    .pin_href = 47,
    .pin_pclk = 13,

   
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG, 
    .frame_size = FRAMESIZE_QVGA,     

    .jpeg_quality = 12, 
    .fb_count = 1,     
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};


/**
 * @brief This function initialises the camera module with defined pins 
 * 
 * @return ESP_OK if initilization is successfull. Or some esp_err_t with corresponding error.
 */
static esp_err_t init_camera(void)
{
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    return ESP_OK;
}

/**
 * @brief This Function takes picture
 * 
 * The picture is taken using esp_camera_fb_get() funtion and then it is convereted into rgb888 format using builtin funtion fmt2rgb888() 
 * and then cropped into the desired format that is required for inference using crop_and_interpolatergb888() funtion [More info about the function will filled in due time]
 * 
 * 
 * @param[out] snapshot_buf The captured and then after cropped image is saved in this variable and returned
 */

void take_picture(uint8_t *snapshot_buf)
{
    if (snapshot_buf == nullptr)
    {
        ei_printf("ERR: Failed to allocate snapshot buffer!\n");
        return;
    }
    ESP_LOGI(TAG, "Taking picture...");
    camera_fb_t *pic = esp_camera_fb_get();
    ESP_LOGI(TAG, "Picture taken! Its size was: %zu bytes", pic->len);
    bool converted = fmt2rgb888(pic->buf, pic->len, PIXFORMAT_JPEG, snapshot_buf);
    esp_camera_fb_return(pic);
    ei::image::processing::crop_and_interpolate_rgb888(
        snapshot_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        snapshot_buf,
        EI_CLASSIFIER_INPUT_WIDTH,
        EI_CLASSIFIER_INPUT_HEIGHT);

}