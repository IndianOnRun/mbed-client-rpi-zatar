# README

Sample code that demonstrates how to use ARM mBed Client on Raspberry Pi with pibrella board to connect and interact with Zatar IoT Cloud.

### License

README.md

Copyright (C) 2016 ZIH Corp and/or its affiliates.

 Description:
         This README is for the Raspberry Pi Sample code for Zatar
 Revision History:
         06/2016   Zebra Technologies Corporation

 License(s):
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.



### Main Features of this Project

* Connects Raspberry Pi with Zatar
* Uses RPi with PiBrella expansion board and ARM mBed Client connecting to Zatar's Device API
* Uses CoAP protocol over UDP (no security)
* Exposes Lightweight Machine-to-Machine (LwM2M) object model for the Pi
* Turn on/off LEDs of PiBrella from Zatar
* Reports base properties type, manufacturer, model, firmware version, and ip_address
* Detects and reports PiBrella pushbutton events and button counter
* 4 small PiBrella leds cycle through pattern to show program is running
* 4 small leds will all turn ON when button is pressed and resume pattern when button is released

## How to Setup and Use the Project

### I. Set up Build Environment on your Raspberry Pi by Following these Instructions
> _Note: These instructions were based on the blog entry from ARM located [here](http://blog.mbed.com/post/137562162237/mbed-client-communication-with-mbed-device). The instructions were tested on an RPi2 using NOOBS v1.9.0._

1. First get latest version of Raspian OS onto your Raspberry Pi device. We recommend using NOOBS installed on a SD card with minimum size of 8GB. To get the latest NOOBS go [here](https://www.raspberrypi.org/downloads/noobs/) and follow the instructions.
2. Make sure your Pi is up and running with Raspian and connected to the Internet before proceeding.
3. You should perform the remaining steps from the GUI Desktop of the Pi. Open one terminal window for all the following commands. Later you will also need to open a web browser window. 
>You will need to have an account and log into it at **_developer.mbed.org_** in order to download the mbed client components needed to build your project, so if you do not have an mbed account you will need to create one to complete this project.
3. First make sure you have updated the OS to latest versions by typing the following at the command prompt:
 > sudo apt-get update

4. Now install the yotta dependencies by typing in the following command at the command prompt:
 > sudo apt-get install python-setuptools  cmake build-essential ninja-build python-dev libffi-dev libssl-dev
5. Install gcc compiler by typing in the following command at the command prompt:
 > sudo apt-get install gcc-arm-none-eabi

6. Update pip by typing _(Note: Make sure that this operation causes pip to be updated to v8.1.2 or later. This is required in order for yotta to install properly.)_:
 > sudo pip install -U pip
7. Install python virtual environment by typing:
 > sudo pip install virtualenv

8. Create the virtual environment by typing:
 > virtualenv mbed-venv
9. Change to the virtual env directory:
 > cd mbed-venv

10. Activate the virtual environment (to deactivate later, type “deactivate”). After this is done, you’ll see (mbed-venv) in front of the usual command prompt.:
 > source ./bin/activate 

11. Install yotta on the virtual environment by typing:
 > sudo pip install -U yotta
12. Clone this repo into the mbed-venv virtual environment directory by typing:
 > Git clone http://github.com/zatar-iot/mbed-client-rpi-zatar.git

13. Change into the project directory:
 > cd mbed-client-rpi-zatar
14. Set the yotta target for your project by typing:
 > yotta target arm-linux-native

15. At this point you should be prompted to log into your mbed account to download the mbed client components from ARM. If you do not have an mbed account you will need to create one. Go to _**http://developer.mbed.org**_ and log in. Once successfully logged in to your mBed acount, **THEN HIT ENTER** at the command line prompt to initiate the download of necessary components.
16. Now you are ready to build and run code using yotta and the virtual environment. 
 > If you want you can verify that you can successfully build your code at this point by typing "yotta build" or "yt build" at the command prompt. The first time you do this, yotta will go out to the Internet and gather all the components necessary to build using mBed Client prior to building your project. This should all occur without error, however the resultant executable will not yet work with Zatar without completing the remaining steps in these instructions.

    **Before your program will be able to transfer data successfully to Zatar, you must first load your personal Device Code Token into the sample code by following Step II below.** 



### II. Configure your project with your own custom Device Code Token (DCT)

* IMPORTANT: BEFORE this code will operate properly, you **MUST** go to [the Zatar Dev Zone](https://developer.zatar.com), register for a FREE developer account, and use the Device Software Manager application (left toolbar) to check out a Device Code Token (DCT). Put this DCT into your code in the module called "ZatarDeviceAuth.cpp". Go to that file and search for "DEVICE_TOKEN" and put your DCT where indicated.
>_Without the DCT properly configured, the program will build, run and successfully register on Zatar, however Zatar will ignore and disregard any data coming from the device unless the DCT is valid. Your device will appear in the "claimed" state in your account on Zatar and never be updated_.

### III. Build and Run Your Code
* Build your code by typing:
 > yotta build

* You can then run the executable by typing:
 > sudo ./build/arm-linux-native/source/mbed-client-rpi-zatar

### IV. Log into your account on Zatar and claim your device
* The serial number of your pi should be displayed on the Pi's console along with some other information indicating successful registration. If so, then go to [Zatar](https://www.zatar.com/rdm-js) and log into your account and claim your device using the instructions for the [Zatar Rpi Demo](https://bitbucket.org/tberarducci/mbed-client-rpi-executables).

**_That’s it! For more information on how to use the executable, refer to our documentation for the Raspberry Pi Demo on Zatar. As always, thanks for your interest in Zatar._**


### For More Info ###

* For more info send an email to developers@zebra.com
