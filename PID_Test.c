#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "PID.h"

/* Controller parameters */
#define PID_KP 0.1f
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

int main()
{
    /* Initialise PID controller */
    PIDController pid = {PID_KP, PID_KI, PID_KD,
                         PID_TAU,
                         PID_LIM_MIN, PID_LIM_MAX,
                         PID_LIM_MIN_INT, PID_LIM_MAX_INT,
                         SAMPLE_TIME_S};

    PIDController_Init(&pid);

    printf("Time (s)\tSystem Output\tControllerOutput\r\n");

    for (int t = 0;; t += SAMPLE_TIME_S)
    {

        /* Get measurement from system */
        float measurement = TestSystem_Update();
        /* Compute new control signal */
        PIDController_Update(&pid, 40, measurement);

        printf("%d\t%f\t%f\r\n", t, measurement, pid.out);
        sleep(SAMPLE_TIME_S);
    }
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
