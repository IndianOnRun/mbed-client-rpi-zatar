/*
* LedObject.h
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This is the header file for LedObject.cpp
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

#ifndef SOURCE_LED_LEDOBJECT_H_
#define SOURCE_LED_LEDOBJECT_H_

#include "mbed-client/m2mobject.h"
#include "GPIOClass.h"

class M2MResource;
class M2MResourceInstance;

class LedObject : public M2MObject{
public:
	typedef enum {
		LedStatus,
		LedToggle
	}LedResource;

	LedObject(int ledNumber);
	virtual ~LedObject();

        String& getMyStatus();
	void toggleLed(void*text);

        void test();
        void clearLed();
        void setLed();

	M2MResource* create_toggle_led_resource(String payload);
        M2MResource* create_text_resource(String &value);

	uint16_t total_resource_count() const;
	uint16_t per_resource_count(LedResource resource) const;
	bool is_resource_present(LedResource resource) const;

private:

	M2MResourceInstance* get_resource_instance(LedResource dev_res, uint16_t instance_id) const;
	String resource_name(LedResource resource) const;
        void executeLedToggle();
	M2MObjectInstance* _led_instance;     //Not owned
        int ledNumber;
        String statusResourceID, toggleCommandResourceID, status;
        GPIOClass *gpio27, *gpio17, *gpio4;
};


#endif /* SOURCE_LED_LEDOBJECT_H_ */
