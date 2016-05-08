//
// Created by Simone Genovese on 08/05/16.
//

#include "BlynkApiWiringPi.h"
#include "BlynkSocket.h"
#include "BlynkOptionsParser.h"

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include "blynk-library/BlynkWidgets.h"

BLYNK_WRITE(V1) {
        printf("Got a value: %s\n", param[0].asStr());
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

