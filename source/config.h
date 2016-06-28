/*
* config.h
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         Basic configuration variables for the example
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

const String &MBED_SERVER_ADDRESS = "coap://device-avatar.zatar.com";    // zatar PROD server Device API interface URI
const int &MBED_SERVER_PORT = 5683;			// Zatar unsecured UDP port number

const String &MANUFACTURER = "Raspberry Pi Foundation";	// manufacturer that will be reported to Zatar
const String &TYPE = "rpi";				// device type that will be reported to Zatar
const String &MODEL_NUMBER = "RaspberryPiIoTKit";  // this value must correspond to Zatar "model" parameter in target Avatar Definition

const String &FIRMWARE_VERSION = "1.1";

static int INTERFACE_LIFETIME = 3600;		// mBed Client will establish and re-register the Interface for this time interval
static int KEEP_ALIVE_INTERVAL = 30;		// main application will send "phantom updates" to unused resource at this interval

static int BUTTON_WAIT_1 = 100000;		// number of microseconds we wait between testing for button actions
static int BUTTON_WAIT_DISPLAY_MULTIPLE = 4; 	// number of button_waits (> 0) to dwell on one led pattern in watchForButtonPresses

static bool SECURITY = false;			// set to false for NO SECURITY (should only be used for demos and testing) or to true for M2MSecurity::Certificate mode security


