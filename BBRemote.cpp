//
// Created by Simone Genovese on 08/05/16.
//

#include "blynk-library/linux/BlynkApiWiringPi.h"
#include "blynk-library/linux/BlynkSocket.h"
#include "blynk-library/linux/BlynkOptionsParser.h"

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include "blynk-library/BlynkWidgets.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdio.h"
#include <time.h>
#include <unistd.h>
#include <glibtop.h>
#include <glibtop/cpu.h>
#include <glibtop/mem.h>

using namespace std;
/* last total cpu time */
guint64 cpu_total_time_last_;
guint64 cpu_used_time_last_;



void timer_start(std::function<void(void)> func, unsigned int interval) {
    std::thread([func, interval]() {
        while (true) {
            func();
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
    }).detach();
}

unsigned long getMEM() {
    glibtop_init();
    glibtop_mem memory;
    glibtop_get_mem(&memory);
    return (unsigned long) (memory.cached + memory.buffer + memory.free) * 100 / memory.total;
}


float getCPU() {
    /* initial glibtop_cpu data */
    glibtop_cpu cpu;
    glibtop_get_cpu(&cpu);

    if (!cpu_total_time_last_) {
        cpu_total_time_last_ = cpu.total;
        cpu_used_time_last_ = cpu.user + cpu.nice + cpu.sys;
        return 0.0;
    }

    /* get system load */
    float load;
    float total, used;

    total = cpu.total - cpu_total_time_last_;
    used = cpu.user + cpu.nice + cpu.sys - cpu_used_time_last_;
    cpu_total_time_last_ = cpu.total;
    cpu_used_time_last_ = cpu.user + cpu.nice + cpu.sys;

    load = 100 * used / max(total, 1.0f);
    load = min(load, 100.0f);

    return load;
}

double getTemp() {
    FILE *temperature;
    double T;
    temperature = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temperature == NULL); //print some message
    fscanf(temperature, "%lf", &T);
    T /= 1000;
    fclose(temperature);
    return T;
}

void updateBlynk() {
    Blynk.virtualWrite(3, getCPU());
    Blynk.virtualWrite(4, getMEM());
    Blynk.virtualWrite(0, getTemp());
}

BLYNK_WRITE(V1) {
        printf("Got a value: %s\n", param[0].asStr());
        double T = getTemp();
        Blynk.virtualWrite(0, T);
}

BLYNK_WRITE(V2) {
        printf("Reboot: %s\n", param[0].asStr());
        system("init 6");
}


int main(int argc, char *argv[]) {
    printf("Starting raspBlynk\n");
    const char *auth, *serv, *port;
    parse_options(argc, argv, auth, serv, port);

    printf("Connecting...\n");
    Blynk.begin(auth, serv, port);

    timer_start(updateBlynk, 2000);
    while (true) {
        Blynk.run();
    }
}

