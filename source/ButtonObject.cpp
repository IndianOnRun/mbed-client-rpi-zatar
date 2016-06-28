/*
* ButtonObject.cpp
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This class contains the logic for the PiBrella Button on Rpi
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

#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2mconstants.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "ButtonObject.h"

const String BUTTON_STATUS_RES_ID = "7000";           		// pibrella button up/dwn status
const String CLEAR_BUTTON_COUNTER_COMMAND_RES_ID = "7001";	// button counter clear command
const String BUTTON_COUNTER_VALUE_RES_ID = "7002";           	// button counter current value


const String LWM2M_ZATAR_BUTTON_OBJ_ID = "23900";
const String &UP = "UP";
const String &DOWN = "DOWN";

String buttonStatus;
int buttonCount;

String buttonStatusResourceID, buttonCounterValueResourceID, clearButtonCounterCommandResourceID;

ButtonObject::ButtonObject():M2MObject(LWM2M_ZATAR_BUTTON_OBJ_ID) {

	_button_instance = M2MObject::create_object_instance();

        buttonStatusResourceID = BUTTON_STATUS_RES_ID;
	buttonCounterValueResourceID = BUTTON_COUNTER_VALUE_RES_ID;
        clearButtonCounterCommandResourceID = CLEAR_BUTTON_COUNTER_COMMAND_RES_ID;
	buttonStatus = UP;
	buttonCount = 0;

	// set up LwM2M resources

	createStringResource(buttonStatusResourceID);
        updateStringResourceValue(buttonStatusResourceID, buttonStatus);
	createIntegerResource(buttonCounterValueResourceID);
        updateIntegerResourceValue(buttonCounterValueResourceID, buttonCount);
	createCommandResource(clearButtonCounterCommandResourceID);

}

ButtonObject::~ButtonObject() {


}


M2MResource* ButtonObject::createStringResource(String resID) {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;

	if(!is_resource_present(resID) && _button_instance) {
		res = _button_instance->create_dynamic_resource(resID,
				OMA_RESOURCE_TYPE,
				M2MResourceInstance::STRING,
				true, false);
		if(res) {
			res->set_operation(operation);
			res->clear_value();
		}
	}
	return res;
}


M2MResource* ButtonObject::createIntegerResource(String resID) {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;

	if(!is_resource_present(resID) && _button_instance) {
		res = _button_instance->create_dynamic_resource(resID,
				OMA_RESOURCE_TYPE,
				M2MResourceInstance::INTEGER,
				true, false);
		if(res) {
			res->set_operation(operation);
			res->clear_value();
		}
	}
	return res;
}

M2MResource* ButtonObject::createCommandResource(String resID) {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::POST_ALLOWED;

	if(!is_resource_present(resID) && _button_instance) {
	res = _button_instance->create_dynamic_resource(resID,
				OMA_RESOURCE_TYPE,
				M2MResourceInstance::STRING, true, false);
		if(res) {
			res->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
			res->set_operation(operation);
         		res->clear_value();
			res->set_execute_function(execute_callback(this, &ButtonObject::clearButtonCounter));
		}
                else{
                    printf("\n\nERROR creating command resource\n\n");
                }
	}
        else{
            printf("\n\nERROR in ButtonObject::createCommandResource...resource already present!\n\n");
        }
	return res;
}


M2MResourceInstance* ButtonObject::get_resource_instance(String resID, uint16_t instance_id) const {
	M2MResource* res = NULL;
	M2MResourceInstance* inst = NULL;
	if(_button_instance) {
		res = _button_instance->resource(resID);
		if(res) {
			if(res->supports_multiple_instances()) {
				inst = res->resource_instance(instance_id);
			} else {
				inst = res;
			}
		}
	}
	return inst;
}


bool ButtonObject::is_resource_present(String resID) const {
	return get_resource_instance(resID, 0) != NULL;
}

void ButtonObject::clearButtonCounter(void*text){

    printf("\n\n...ButtonObject...received clearButtonCounter command\n\n");
    buttonCount = 0;
    updateIntegerResourceValue(buttonCounterValueResourceID, buttonCount);

}

void ButtonObject::buttonUpDetected(){

    buttonStatus = UP;
    updateStringResourceValue(buttonStatusResourceID, buttonStatus);
    printf("\n\n...ButtonObject...button UP detected!\n\n");


}


void ButtonObject::buttonDownDetected(){

    buttonStatus = DOWN;
    buttonCount++;
    updateStringResourceValue(buttonStatusResourceID, buttonStatus);
    updateIntegerResourceValue(buttonCounterValueResourceID, buttonCount); 
    printf("\n\n...ButtonObject...button DOWN detected! buttonCount = %i\n\n", buttonCount);

}


void ButtonObject::updateStringResourceValue(String resID, String value){

    M2MResource* res = NULL;
    if(_button_instance){
        res = _button_instance->resource(resID);
        if(res){
                M2MResourceInstance* resIns = get_resource_instance(resID,0);
        	resIns->set_value((const uint8_t*)value.c_str(),(uint32_t)value.length());
        }
	else{
            printf("\n\nERROR - ButtonObject...updateStringResourceValue...res is NULL...exiting\n\n");
        }
    }
    else{
        printf("\n\nERROR - ButtonObject...updateStringResourceValue... _button_instance does not exist...exiting\n\n");
    }

}

String ButtonObject::convertIntToString(int integerValue){

    char str[16];
    sprintf(str,"%i", integerValue);
    return str;

}

void ButtonObject::updateIntegerResourceValue(String resID, int value){

    M2MResource* res = NULL;
    if(_button_instance){
        res = _button_instance->resource(resID);
        if(res){
                M2MResourceInstance* resIns = get_resource_instance(resID,0);
		char buffer[16];
    		int size = sprintf(buffer,"%i", value);
        	resIns->set_value((const uint8_t*)buffer,(uint32_t)size);
        }
	else{
            printf("\n\nERROR - ButtonObject...updateIntegerResourceValue...res is NULL...exiting\n\n");
        }
    }
    else{
        printf("\n\nERROR - ButtonObject...updateIntegerResourceValue... _button_instance does not exist...exiting\n\n");
    }

}

