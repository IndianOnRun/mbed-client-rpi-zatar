/*
* ButtonObject.h
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This class contains the header for ButtonObject.cpp
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

#ifndef SOURCE_BUTTON_OBJECT_H_
#define SOURCE_BUTTON_OBJECT_H_

#include "mbed-client/m2mobject.h"

class M2MResource;
class M2MResourceInstance;

class ButtonObject : public M2MObject{
public:
	ButtonObject();
	virtual ~ButtonObject();

        int& getButtonCounterValue();
        bool& getButtonUpDwnStatus();
	void clearButtonCounter(void*text);
        void buttonUpDetected();
        void buttonDownDetected();


private:
	typedef enum {
		ButtonStatus,
		ButtonCounterValue,
		ClearButtonCounter
	}ButtonResource;

	M2MResource* createCommandResource(String resID);
        M2MResource* createStringResource(String resID);
        M2MResource* createIntegerResource(String resID);
        void updateStringResourceValue(String resID, String value);
        void updateIntegerResourceValue(String resID, int value);
        String convertIntToString(int value);

	uint16_t total_resource_count() const;
	uint16_t per_resource_count(ButtonResource resource) const;
	bool is_resource_present(String resID) const;

	M2MResourceInstance* get_resource_instance(String resID, uint16_t instance_id) const;
	String resource_name(ButtonResource resource) const;

	M2MObjectInstance* _button_instance;     //Not owned
        String buttonStatusResourceID, buttonCounterValueResourceID, clearButtonCounterCommandResourceID, buttonStatus, count;

};


#endif /* SOURCE_BUTTON_OBJECT_H_ */
