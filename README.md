# sps30
Control program for the senirion sps30 particle matter sensor (I²C interface)

## Hardware/OS
* Raspberry Pi (Zero/w)
* Sensirion SPS30
* Raspbian Stretch Lite (version: Nov 2018)

## Pi - I²C (Inter-Integrated Circuit / i2c) 
activate i2c with the raspi-config
```
sudo raspi-config
```
enter rapsi config menu navigate to 5. Interfacing Options
![raspi-config1](https://raw.githubusercontent.com/brannow/sps30/master/docs/raspi-config_level1.png)

go to P5 I2C and enable it

![raspi-config2](https://raw.githubusercontent.com/brannow/sps30/master/docs/i2c_enable.png)

double check the config.txt 

in ```/boot/config.txt```
 
Add the correct clock rate for 200kHz. <br /> (NOTE: rPI clock speed is CPU bound and very inaccurate so i set it to 230000)
```
dtparam=i2c_arm=on
dtparam=i2c_arm_baudrate=230000
```

## GPIO Pins

![gpio_i2c](https://raw.githubusercontent.com/brannow/sps30/master/docs/gpio_i2c.png)

NOTE: powering off the PI before connect the cables to the GPIO.<br />
NOTE: i2c is not suitable for long wirings keep the wires as short as possible (not longer than 10cm / 4inches)

## Test

install i2c-tools via apt:
``` 
sudo apt update
sudo apt install i2c-tools
```

after that, check if the sensor is reachable:
```
i2cdetect -y 1
```
the output should look like this: 
``` 
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- -- 
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
30: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- 
60: -- -- -- -- -- -- -- -- -- 69 -- -- -- -- -- -- 
70: -- -- -- -- -- -- -- --  
```

0x69 is the i2c address from the sps30 - sensor

## Compile 
execute the build script 
```
./build
```

## Execute
The final binary is ```./bin/sps30```

[TODO: add more here]
