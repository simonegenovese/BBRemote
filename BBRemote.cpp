//
// Created by Simone Genovese on 08/05/16.
//

#include "BlynkApiWiringPi.h"
#include "BlynkSocket.h"
#include "BlynkOptionsParser.h"
#include <unistd.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include "blynk-library/BlynkWidgets.h"

#include <iostream>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdio.h"
#include <time.h>
#include <glibtop/cpu.h>
#include <unistd.h>
#include <glibtop.h>
#include <glibtop/mem.h>
using namespace std;
/* last total cpu time */
guint64 cpu_total_time_last_;
guint64 cpu_used_time_last_;

/* last io bytes */
long read_bytes_last_;
long write_bytes_last_;

/* nubmer of cpu */
guint64 ncpu_;

unsigned long getMEM()
{

    glibtop_init();
    glibtop_mem memory;
    glibtop_get_mem(&memory);



    return (unsigned long)(memory.cached + memory.buffer + memory.free) * 100/memory.total;
}


float getCPU()
{

    /* initial glibtop_cpu data */
    glibtop_cpu cpu;
    glibtop_get_cpu (&cpu);

    if (!cpu_total_time_last_){
        cpu_total_time_last_ = cpu.total;
        cpu_used_time_last_  = cpu.user + cpu.nice + cpu.sys;
        return 0.0;
    }

    /* get system load */
    float load;
    float total, used;

    total = cpu.total - cpu_total_time_last_;
    used  = cpu.user + cpu.nice + cpu.sys - cpu_used_time_last_;
    cpu_total_time_last_ = cpu.total;
    cpu_used_time_last_  = cpu.user + cpu.nice + cpu.sys;

    load  = 100 * used / max(total, 1.0f);
    load  = min (load, 100.0f);

    return load;



}

BLYNK_WRITE(V1) {
        printf("Got a value: %s\n", param[0].asStr());
        FILE *temperature;
        double T;
        temperature = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
        if (temperature == NULL)
        ; //print some message
        fscanf (temperature, "%lf", &T);
        T /= 1000;
        printf ("The temperature is %6.3f C.\n", T);
        fclose (temperature);
        Blynk.virtualWrite(0, T);

}

BLYNK_WRITE(V2) {
        printf("Reboot: %s\n", param[0].asStr());
        system("init 6");
}



int main(int argc, char *argv[]) {
    const char *auth, *serv, *port;
    parse_options(argc, argv, auth, serv, port);

    Blynk.begin(auth, serv, port);

    while (true) {
        Blynk.run();
        sleep(1000);
        Blynk.virtualWrite(3,getCPU());
        Blynk.virtualWrite(4,getMEM());
    }
}

