# esp32-s3 project temperature sensor
## Objective
Connect esp32 to internet, retrieve datas from the temperature sensor, and send then to a server with http method

In progress......

## Specifications
* ESP-IDF-v4.4.2
* Esp32-s3

## Structure
* main file (named esp32_tempsensor.c) is in the folder main
* other files are in the folder components
* header file with global variables is in the folder config

## How to run ?
* Go to the folder esp32_tempsensor
* If you want to change the config, you can run idf.py menuconfig and change informations in Network Configuration and in Http Configuration
* enter command idf.py -p (your port where esp32 is connected) flash monitor
