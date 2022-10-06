#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "PID.h"

/* Controller parameters */
#define PID_KP 5.0f
#define PID_KI 0.01f
#define PID_KD 0.01f

#define PID_TAU 0.02f

#define PID_LIM_MIN -100.0f
#define PID_LIM_MAX 0.0f

#define PID_LIM_MIN_INT -100.0f
#define PID_LIM_MAX_INT 0.0f

#define SAMPLE_TIME_S 1.0f

/* Maximum run-time of simulation */
#define SIMULATION_TIME_MAX 4.0f

/* Simulated dynamical system (first order) */
float TestSystem_Update();
int fan_init();
int fan_close();
int set_fan(float temp);


int main()
{
    /* Initialise PID controller */
    PIDController pid = {PID_KP, PID_KI, PID_KD,
                         PID_TAU,
                         PID_LIM_MIN, PID_LIM_MAX,
                         PID_LIM_MIN_INT, PID_LIM_MAX_INT,
                         SAMPLE_TIME_S};

    PIDController_Init(&pid);

    printf("Time (s)\tSystem Output\tControllerOutput\tperiod\r\n");
    fan_init();
    for (int t = 0;; t += SAMPLE_TIME_S)
    {

        /* Get measurement from system */
        float measurement = TestSystem_Update();
        /* Compute new control signal */
        PIDController_Update(&pid, 40, measurement);
        
        printf("%d\t%f\t%f\t%d\r\n", t, measurement, pid.out,set_fan(pid.out));
        sleep(SAMPLE_TIME_S);
    }
    // fan_close();
    return 0;
}

float TestSystem_Update()
{

    static float output = 0.0f;
    const char *sensor = "/sys/class/thermal/thermal_zone1/temp";
    int fd = open(sensor, O_RDONLY);
    if (-1 == fd)
    {
        printf("sensor open fail\n");
        return -1;
    }
    else
    {
        // printf("sensor open ok\n");
    }
    char temp[8] = {0};
    read(fd, temp, sizeof(temp));

    // printf("%f\n", atof(temp) / 1000);

    return atof(temp) / 1000;
}
int fan_init()
{
    int fd = open("/sys/class/pwm/pwmchip1/export", O_WRONLY);
    if (-1 == fd)
    {
        printf("pwmchip open fail\n");
        return -1;
    }
    char buf[1] = "0";
    if (write(fd, buf, sizeof(buf))==-1)
    {
        printf("pwmchip export fail or running\n");
        // return -1;
    }
    sleep(1);

    fd = open("/sys/class/pwm/pwmchip1/pwm0/period", O_WRONLY);
    if (-1 == fd)
    {
        printf("period open fail\n");
        return -1;
    }
    char buf1[11] = "1000000000";
    if (write(fd, buf1, sizeof(buf1))==-1)
    {
        printf("set period fail\n");
        return -1;
    }

    fd = open("/sys/class/pwm/pwmchip1/pwm0/duty_cycle", O_WRONLY);
    if (-1 == fd)
    {
        printf("duty_cycle open fail\n");
        return -1;
    }
    char buf2[11] = "100000000";
    if (write(fd, buf2, sizeof(buf2))==-1)
    {
        printf("set duty_cycle fail\n");
        return -1;
    }

    fd = open("/sys/class/pwm/pwmchip1/pwm0/enable", O_WRONLY);
    if (-1 == fd)
    {
        printf("enable open fail\n");
        return -1;
    }
    char buf3[1] = "1";
    if (write(fd, buf3, sizeof(buf3))==-1)
    {
        printf("set enable fail\n");
        return -1;
    }
    return 1;
}


int fan_close()
{

    static float output = 0.0f;
    const char *pwmchip = "/sys/class/pwm/pwmchip1/unexport";
    int fd = open(pwmchip, O_WRONLY);
    if (-1 == fd)
    {
        printf("pwmchip open fail\n");
        return -1;
    }
    else
    {
        // printf("pwmchip open ok\n");
    }
    char buf[1] = "0";
    int ok =write(fd, buf, sizeof(buf));

    // printf("%f\n", atof(temp) / 1000);
    // printf("%d\n",ok);
    return ok;
}


int set_fan(float temp)
{
    int fd = open("/sys/class/pwm/pwmchip1/pwm0/period", O_WRONLY);
    if (-1 == fd)
    {
        printf("period open fail\n");
        return -1;
    }
    
    // printf("%d\n",temp);
    temp=100000000/(0-100/20*temp/100);
    
    // printf("%f\n",temp);
    char buf1[11];
    sprintf(buf1,"%d",(int)temp);
    // printf(buf1);
    if (write(fd, buf1, sizeof(buf1))==-1)
    {
        printf("set period fail\n");
        return -1;
    }
    return (int)temp;
}