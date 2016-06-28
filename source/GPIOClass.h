/*

        author: Hussam Al-Hertani
        Description: The GPIOClass C++ class enables the Raspberry Pi its onboard GPIOs via the sysfs interface
        License: GPL2
        repo: https://github.com/halherta/RaspberryPi-GPIOClass-v2
        website: http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/

*
* GPIOClass.h
* 
* Description:
*         Raspberry Pi GPIO pins library header file
* Revision History:
*         05/2016   Zebra Technologies Corporation
*
* License(s):
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License version 2 as
*   published by the Free Software Foundation.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
*   02110-1301, USA.
*
*/

#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
/* GPIO Class
 * Purpose: Each object instatiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */
class GPIOClass
{
public:
	GPIOClass();
	GPIOClass(string gnum);
	~GPIOClass();
    int setdir_gpio(string dir);
    int setval_gpio(string val);
    int getval_gpio(string& val);
    string get_gpionum();
private:
    int export_gpio();
	int unexport_gpio();
	
	int valuefd;
	int directionfd;
	int exportfd;
	int unexportfd;
	string gpionum;
	
};

#endif
