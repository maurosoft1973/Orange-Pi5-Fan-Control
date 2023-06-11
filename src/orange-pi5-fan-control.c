#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <softPwm.h>
#include "library/argparse.h"

static const char* const usages[] = {
    "orange-pi5-fan-control [options] [[--] args]",
    "orange-pi5-fan-control [options]",
    NULL,
};

// Define
#define FAN_PIN_DEFAULT 16
#define MIN_TEMP_DEFAULT 35
#define MAX_TEMP_DEFAULT 65
#define CPU_TEMP_FILE_DEFAULT "/sys/class/thermal/thermal_zone0/temp"
#define MIN_PWM_DEFAULT 0
#define MAX_PWM_DEFAULT 100
#define DELAY_DEFAULT 5

int main(int argc, const char** argv) {
    int fan_pin = FAN_PIN_DEFAULT;
    int min_temp = MIN_TEMP_DEFAULT;
    int max_temp = MAX_TEMP_DEFAULT;
    char *cpu_temp_file = CPU_TEMP_FILE_DEFAULT;
    int min_pwm = MIN_PWM_DEFAULT;
    int max_pwm = MAX_PWM_DEFAULT;
    int delay = DELAY_DEFAULT;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Options"),
        OPT_INTEGER('f', "fanpin", &fan_pin, "Fan Pin (Default 16)", NULL, 0, 0),
        OPT_INTEGER('m', "mintemp", &min_temp, "Min Temperature (Default 35 °C)", NULL, 0, 0),
        OPT_INTEGER('x', "maxtemp", &max_temp, "Max Temperature (Default 65 °C)", NULL, 0, 0),
        OPT_STRING('c', "cputempfile", &cpu_temp_file, "File Temperature (Default /sys/class/thermal/thermal_zone0/temp)", NULL, 0, 0),
        OPT_INTEGER('p', "minpwm", &min_pwm, "Min PWM (Default 0%)", NULL, 0, 0),
        OPT_INTEGER('a', "maxpwn", &max_pwm, "Max PWM (Default 100%)", NULL, 0, 0),
        OPT_INTEGER('d', "delay", &delay, "Delay read next temp value (Default 5 sec)", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nOrange Pi 5 PWM Fan Control\nReads the CPU temperature and adjusts the velocity of fan speed based on that information.","");
    argc = argparse_parse(&argparse, argc, argv);

    //printf("fan_pin: %d\n", fan_pin);
    //printf("min_temp: %d\n", min_temp);
    //printf("max_temp: %d\n", max_temp);
    //printf("min_pwm: %d\n", min_pwm);
    //printf("max_pwm: %d\n", max_pwm);
    //printf("delay: %d\n", delay);

    int temp;
    float temp_ratio, pwm_ratio;
    int pwm_value = 0;

    if (wiringPiSetup() == -1) {
        printf("wiringPiSetup failed\n");
        return 1;
    }

    pinMode(fan_pin, OUTPUT);
    softPwmCreate(fan_pin, min_pwm, max_pwm);

    while(1) {
        FILE* temp_file = fopen(cpu_temp_file, "r");

        if (temp_file == NULL) {
            printf("Failed to read CPU temperature file\n");
            return 1;
        }

        fscanf(temp_file, "%d", &temp);
        fclose(temp_file);

        temp /= 1000;

        if (temp < min_temp) {
            pwm_value = min_pwm;
        } else if (temp > max_temp) {
            pwm_value = max_pwm;
        } else {
            temp_ratio = (float)(temp - min_temp) / (max_temp - min_temp);
            pwm_ratio = temp_ratio * (max_pwm - min_pwm);
            pwm_value = (int)(pwm_ratio + min_pwm);
        }

        softPwmWrite(fan_pin, pwm_value);

        printf("CPU: %d°C | PWM: %d%%\n", temp, pwm_value);

        sleep(delay);
    }

    return 0;
}
