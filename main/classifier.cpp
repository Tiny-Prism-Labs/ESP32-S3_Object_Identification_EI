#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <stdio.h>

#define LED_PIN GPIO_NUM_21
uint8_t *temp_buffer;
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{

    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0)
    {
        out_ptr[out_ptr_ix] = (temp_buffer[pixel_ix] << 16) + (temp_buffer[pixel_ix + 1] << 8) + temp_buffer[pixel_ix + 2];

        // go to the next pixel
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }
    // and done!
    return 0;
    //   memcpy(out_ptr, features + offset, length * sizeof(float));
    //   return 0;
}

void print_inference_result(ei_impulse_result_t result)
{

    // Print how long it took to perform inference
    ei_printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
              result.timing.dsp,
              result.timing.classification,
              result.timing.anomaly);

    // Print the prediction results (object detection)
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    ei_printf("Object detection bounding boxes:\r\n");
    for (uint32_t i = 0; i < result.bounding_boxes_count; i++)
    {
        ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
        if (bb.value == 0)
        {
            continue;
        }
        ei_printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                  bb.label,
                  bb.value,
                  bb.x,
                  bb.y,
                  bb.width,
                  bb.height);
    }

    // Print the prediction results (classification)
#else
    ei_printf("Predictions:\r\n");
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++)
    {
        ei_printf("  %s: ", ei_classifier_inferencing_categories[i]);
        ei_printf("%.5f\r\n", result.classification[i].value);
    }
#endif

    // Print anomaly result (if it exists)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("Anomaly prediction: %.3f\r\n", result.anomaly);
#endif
}

int classify(uint8_t *snapshot_buf)
{

    temp_buffer = snapshot_buf;
    gpio_pad_select_gpio(LED_PIN);
    gpio_reset_pin(LED_PIN);

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    ei_sleep(100);

    ei_impulse_result_t result = {nullptr};

    ei_printf("Edge Impulse standalone inferencing (Espressif ESP32)\n");

    // blink LED
    gpio_set_level(LED_PIN, 1);

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    if (res != EI_IMPULSE_OK)
    {
        ei_printf("ERR: Failed to run classifier (%d)\n", res);
        return res;
    }
    print_inference_result(result);
    free(snapshot_buf);
    gpio_set_level(LED_PIN, 0);
    ei_sleep(1000);
    return 1;
}