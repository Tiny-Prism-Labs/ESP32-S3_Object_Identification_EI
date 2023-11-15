# Object Detection using XIAO ESP32S3 Sense 

This repository runs an [Retail Image Classification](https://github.com/edgeimpulse/expert-projects/blob/main/image-projects/retail-image-classification-nvidia-jetson-nano.md) by [Adam Milton-Barker](https://www.adammiltonbarker.com) on XIAO ESP32S3 Sense. The original EI project was built for Jetson Nano but we ported the it to XIAO ESP32S3 Sense.

# Goal and Objectives

I have noticed that their isnt much development happening towards building EI models for ESP32-S3 using ESP-IDF and as such wanted to contribute this as project to encourage to continue development to it.I maynot accept PR for this repo but if you have suggestions or improvements for documentation with which more developers can use it I would appreciate it.

## Requirements

### Hardware

* [XIAO ESP32S3 Sense](https://www.seeedstudio.com/XIAO-ESP32S3-Sense-p-5639.html).

While the script is mainly tested with XIAO ESP32S3 Sense, other ESP32-based development boards will work too with changes to pins of the camera and other code changes.

### Software

* [Edge Impulse CLI](https://docs.edgeimpulse.com/docs/cli-installation).

* [ESP IDF 4.4](https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/get-started/index.html).

I personally use dockerized version of ESP-IDF as versions are much easier to manage which I on different versions of ESP-IDF

* [ESP IDF 4.4 Docker](https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32s3/api-guides/tools/idf-docker-image.html)

## Building the application


### Build Flash Monitor

1. Open the project in Docker:
   ```bash
   docker run --privileged --rm -v $PWD:/project -w /project -it espressif/idf:release-v4.4
   ```
2. Compile:
   ```bash
   idf.py build flash monitor
   ```
### Output

You can expect output like this

```bash
I (1527) example:take_picture: Taking picture...
I (1567) example:take_picture: Picture taken! Its size was: 4956 bytes
I (1637) gpio: GPIO[21]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Edge Impulse standalone inferencing (Espressif ESP32)
Timing: DSP 3 ms, inference 4657 ms, anomaly 0 ms
Predictions:
  Apples: 0.69141
  Bananas: 0.02344
  Broccoli: 0.16016
  Chilli: 0.12500
I (12437) example:take_picture: Taking picture...
I (12437) example:take_picture: Picture taken! Its size was: 5356 bytes
I (12507) gpio: GPIO[21]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0 
Edge Impulse standalone inferencing (Espressif ESP32)
Timing: DSP 3 ms, inference 4657 ms, anomaly 0 ms
Predictions:
  Apples: 0.86328
  Bananas: 0.11328
  Broccoli: 0.02344
  Chilli: 0.00000

```

# To-Do List

- [ ] Having a Web UI to click picture and then classify
- [ ] Logging the classified data to SD card along with the image