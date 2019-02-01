# sps30
Control program for the senirion sps30 particle matter sensor (UART interface)

## Hardware/OS
* Raspberry Pi (Zero/w)
* Sensirion SPS30
* Raspbian Stretch Lite (version: Nov 2018)

## Pi Serial 
in terms to activate UART over GPIO pin (8 and 10) we need to disable every service that blocks us.
```
sudo raspi-config
```
enter rapsi config menu navigate to 5. Interfacing Options
![raspi-config1](https://raw.githubusercontent.com/brannow/sps30/master/docs/raspi-config_level1.png)

go to P6 Serial and disable the login shell but leave the hardware running

![raspi-config2](https://raw.githubusercontent.com/brannow/sps30/master/docs/raspi-config_level2.png)

at the end the result screen should something like this 
![raspi-config_success](https://raw.githubusercontent.com/brannow/sps30/master/docs/raspi-config_disable_serial_success.png)

double check the cmdline.txt and config.txt 

in /boot/```config.txt```

check if enable_uart=1 exists. 
Plus this line at the end ```dtoverlay=pi3-disable-bt``` 
```
enable_uart=1
dtoverlay=pi3-disable-bt
```
in /boot/```cmdline.txt```

```
dwc_otg.lpm_enable=0 console=tty1 root=PARTUUID=d8d0a62d-02 rootfstype=ext4 elevator=deadline fsck.repair=yes rootwait
```
no other "console" stuff should be there (exception: console=tty1)

## disable BT hciuart
check status with ```sudo systemctl status hciuart```
if the service not found or "active: (dead)" this is fine.<br/>
if it's active 
```
sudo systemctl stop hciuart
sudo systemctl disable hciuart
```

## UART first test

### hardware

Wire GPIO pin 8 and 10 together:<br/>
![gpio](https://raw.githubusercontent.com/brannow/sps30/master/docs/gpio.png)

### software
use screen or minicom (i'll prefer screen)
install over apt: ```sudo apt install screen```

minicom: 
```
minicom -D /dev/serial0
```

screen: 
```
screen /dev/serial0 115200
```

if you can type and see what you type the connection is Fine!<br/>
if you see nothing something is wrong! maybe try serial port /dev/serial1 instead.<br/>
*Remember to terminale the screen / minicon session otherwise UART is blocked*

