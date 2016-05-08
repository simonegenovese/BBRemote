//
// Created by Simone Genovese on 08/05/16.
//

#include "BlynkApiWiringPi.h"
#include "BlynkSocket.h"
#include "BlynkOptionsParser.h"

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include "blynk-library/BlynkWidgets.h"

int GetCPULoad() {
    int FileHandler;
    char FileBuffer[1024];
    float load;

    FileHandler = open("/proc/loadavg", O_RDONLY);
    if(FileHandler < 0) {
        return -1; }
    read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
    sscanf(FileBuffer, "%f", &load);
    close(FileHandler);
    return (int)(load * 100);
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
        Blynk.virtualWrite(3,GetCPULoad());
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
    }
}

