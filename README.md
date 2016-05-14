# BBRemote

Is a Raspbian code to check the raspberry status from Blynk app.


## Install the app
1. Download Blynk: for iPhone or Android and open it
2. Scan this QR code

![Alt text](Image-1.jpg?raw=true "Title")

## Compile in the Raspberry
3. Open a Terminal in the raspberry pi 2/3
4. git clone https://github.com/simonegenovese/BBRemote
5. cd BBRemote
6. make clean all target=raspberry
7. sudo ./bbremote —token=yourTokenHere

## Autoboot
1. Edit with your token the bbremoted file (nano bbremoted)
2. sudo cp bbremoted /etc/init.d/
3. cd /etc/init.d/
4. sudo update-rc.d bbremoted defaults
