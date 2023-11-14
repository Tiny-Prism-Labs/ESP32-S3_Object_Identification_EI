#include "esp_camera.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_err.h"
#include "esp_log.h"
#include "model-parameters/model_metadata.h"
// WROVER-KIT PIN Map
#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM

#define PWDN_GPIO_NUM -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 10
#define SIOD_GPIO_NUM 40
#define SIOC_GPIO_NUM 39

#define Y9_GPIO_NUM 48
#define Y8_GPIO_NUM 11
#define Y7_GPIO_NUM 12
#define Y6_GPIO_NUM 14
#define Y5_GPIO_NUM 16
#define Y4_GPIO_NUM 18
#define Y3_GPIO_NUM 17
#define Y2_GPIO_NUM 15
#define VSYNC_GPIO_NUM 38
#define HREF_GPIO_NUM 47
#define PCLK_GPIO_NUM 13

#define LED_GPIO_NUM 21
uint8_t *snapshot_buf; // points to the output of the capture
static const char *TAG = "Camera";

#define EI_CAMERA_RAW_FRAME_BUFFER_COLS 320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS 240
#define EI_CAMERA_FRAME_BYTE_SIZE 3

esp_err_t camera_init()
{

    camera_config_t config;

    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;

    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;

    // XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    config.xclk_freq_hz = 20000000;
    config.ledc_timer = LEDC_TIMER_0;
    config.ledc_channel = LEDC_CHANNEL_0;

    config.pixel_format = PIXFORMAT_JPEG; // YUV422;GRAYSCALE;RGB565;JPEG
    config.frame_size = FRAMESIZE_QVGA;   // QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    config.jpeg_quality = 12; // 0-63 lower number means higher quality
    config.fb_count = 1;      // if more than one; i2s runs in continuous modeconfig. Use only with JPEG
    config.fb_location = CAMERA_FB_IN_PSRAM;
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;


        // initialize the camera
        esp_err_t err = esp_camera_init(&config);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Camera Init Failed");
            return err;
        }

        return ESP_OK;

}

esp_err_t camera_capture()
{
    // acquire a frame
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb)
    {
        ESP_LOGE(TAG, "Camera Capture Failed");
        return ESP_FAIL;
    }
    // replace this with your own function
    // process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);

    snapshot_buf = (uint8_t *)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
    // return the frame buffer back to the driver for reuse
    esp_camera_fb_return(fb);

    ei::image::processing::crop_and_interpolate_rgb888(
        snapshot_buf,
        EI_CAMERA_RAW_FRAME_BUFFER_COLS,
        EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
        snapshot_buf,
        EI_CLASSIFIER_INPUT_WIDTH,
        EI_CLASSIFIER_INPUT_HEIGHT);

    return ESP_OK;
}