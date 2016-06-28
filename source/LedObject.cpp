/*
* LedObject.cpp
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This class is for the 3 leds on the PiBrella board for the Rpi
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
#include "LedObject.h"

// Modified for Zatar Rpi - moved from header file to here - tnb
const String LED_1_STATUS_RES_ID = "5850";           // RED LED
const String LED_1_TOGGLE_COMMAND_RES_ID = "5853";
const String LED_2_STATUS_RES_ID = "5860";           // YELLOW LED
const String LED_2_TOGGLE_COMMAND_RES_ID = "5863";
const String LED_3_STATUS_RES_ID = "5870";           // GREEN LED
const String LED_3_TOGGLE_COMMAND_RES_ID = "5873";


const String LWM2M_ZATAR_LED_OBJ_ID = "23856";
const String &ON = "ON";
const String &OFF = "OFF";

String status;
int ledNumber;

// added by TNB for zatar for multiple LEDs
String statusResourceID, toggleCommandResourceID;

LedObject::LedObject(int ledint):M2MObject(LWM2M_ZATAR_LED_OBJ_ID) {

        ledNumber = ledint;

	_led_instance = M2MObject::create_object_instance();

        switch(ledNumber){
           case 1:   // red pibrella led
               statusResourceID = LED_1_STATUS_RES_ID;
               toggleCommandResourceID = LED_1_TOGGLE_COMMAND_RES_ID;
               gpio27 = new GPIOClass("27");    	// create pin 27 object
               gpio27->setdir_gpio("out");              // set it up as output
               break;
           case 2:   // yellow pibrella led
               statusResourceID = LED_2_STATUS_RES_ID;
               toggleCommandResourceID = LED_2_TOGGLE_COMMAND_RES_ID;
               gpio17 = new GPIOClass("17");    	// create pin 17 object
               gpio17->setdir_gpio("out");              // set it up as output
               break;
           case 3:   // green pibrella led
               statusResourceID = LED_3_STATUS_RES_ID;
               toggleCommandResourceID = LED_3_TOGGLE_COMMAND_RES_ID;
               gpio4 = new GPIOClass("4");    		// create pin 4 object
               gpio4->setdir_gpio("out");               // set it up as output
               break;
           default:
               statusResourceID = LED_1_STATUS_RES_ID;
               toggleCommandResourceID = LED_1_TOGGLE_COMMAND_RES_ID;
               gpio27 = new GPIOClass("27");    	// create pin 27 object
               gpio27->setdir_gpio("out");              // set it up as output
               break;
        }

	status = ON;
}

LedObject::~LedObject() {

	switch(ledNumber){

		case(1):	// RED led - gpio 27
        		delete gpio27;
			break;
		case(2):	// YELLOW led - gpio 17
			delete gpio17;
			break;
		case(3):	// GREEN led - gpio 4
			delete gpio4;
			break;
	}

}

void LedObject::test(){

     printf("\n\n...Inside LedObject::test...ledNumber = %i\n\n", ledNumber);
     printf("_led_instance = %x\n", _led_instance);
     printf("statusResourceID = %s\n", statusResourceID.c_str());
     printf("toggleCommandResourceID = %s\n\n", toggleCommandResourceID.c_str());
}

void LedObject::clearLed(){

     // turn off LED

     switch(ledNumber){

         case(1):    // RED led
             gpio27->setval_gpio("0");
             break;
         case(2):    // YELLOW led
             gpio17->setval_gpio("0");
             break;
         case(3):    // GREEN led
             gpio4->setval_gpio("0");
             break;
         default:
             break;
     }

}

void LedObject::setLed(){

     // turn on LED

     switch(ledNumber){

         case(1):    // RED led
             gpio27->setval_gpio("1");
             break;
         case(2):    // YELLOW led
             gpio17->setval_gpio("1");
             break;
         case(3):    // GREEN led
             gpio4->setval_gpio("1");
             break;
         default:
             break;
     }

}

void LedObject::executeLedToggle(){

        printf("\n--> received toggle command...ledNumber = %i, status = %s\n\n", ledNumber, status.c_str());
        switch(ledNumber){
            case 1:       // red led on pibrella bd
                if(status == ON){
                    gpio27->setval_gpio("1");
                }
                else{
                    gpio27->setval_gpio("0");
                }
                break;
            case 2:       // yellow led on pibrella bd
                if(status == ON){
                    gpio17->setval_gpio("1");
                }
                else{
                    gpio17->setval_gpio("0");
                }
                break;
            case 3:       // green led on pibrella bd
                if(status == ON){
                    gpio4->setval_gpio("1");
                }
                else{
                    gpio4->setval_gpio("0");
                }
                break;
            default:
                break;
        }

}

String& LedObject::getMyStatus(){

       return status;
}


M2MResource* LedObject::create_text_resource(String &value) {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;
	if(!is_resource_present(LedObject::LedStatus) && _led_instance) {

		res = _led_instance->create_dynamic_resource(statusResourceID,
				OMA_RESOURCE_TYPE,
				M2MResourceInstance::STRING,
				true, false);
		if(res) {
			res->set_operation(operation);
			if (value.empty()) {
				res->clear_value();
			} else {
				res->set_value((const uint8_t*)value.c_str(),
						(uint32_t)value.length());
			}
		}
	}
	return res;
}

M2MResource* LedObject::create_toggle_led_resource(String payload) {
	M2MResource* res = NULL;
	String resource_id = "";

	M2MBase::Operation operation = M2MBase::POST_ALLOWED;
	if(!is_resource_present(LedObject::LedToggle) && _led_instance) {
	res = _led_instance->create_dynamic_resource(toggleCommandResourceID,
				OMA_RESOURCE_TYPE,
				M2MResourceInstance::STRING, true, false);
		if(res) {
			res->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
			res->set_operation(operation);
			if (payload.empty()) {
				res->clear_value();
			} else {
				res->set_value((const uint8_t*)payload.c_str(),
						(uint32_t)payload.length());
			}
			res->set_execute_function(execute_callback(this, &LedObject::toggleLed));
		}
                else{
                    printf("\n\nERROR creating dynamic resource\n\n");
                }
	}
        else{
            printf("\n\nERROR in LedObject::create_toggle_led_resource...resource already present!\n\n");
        }
	return res;
}

bool LedObject::is_resource_present(LedResource resource) const {
	return get_resource_instance(resource, 0) != NULL;
}

uint16_t LedObject::per_resource_count(LedResource resource) const {
	uint16_t count = 0;
	if(_led_instance) {
		count = _led_instance->resource_count(resource_name(resource));
	}
	return count;
}

uint16_t LedObject::total_resource_count() const {
	uint16_t count = 0;
	if(_led_instance) {
		count = _led_instance->resources().size();
	}
	return count;
}

M2MResourceInstance* LedObject::get_resource_instance(LedResource resource, uint16_t instance_id) const {
	M2MResource* res = NULL;
	M2MResourceInstance* inst = NULL;
	if(_led_instance) {
		res = _led_instance->resource(resource_name(resource));
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

String LedObject::resource_name(LedResource resource) const {
	String name;
	switch(resource) {
	case LedStatus:
		name = statusResourceID;
		break;
	case LedToggle:
		name = toggleCommandResourceID;
		break;
	default:
		name = "";
		break;
	}
	return name;
}

void LedObject::toggleLed(void*text) {

	if(status == OFF) {
		status = ON;
	}
	else {
		status = OFF;
	}

	M2MResourceInstance* resIns = get_resource_instance(LedStatus, 0);

	resIns->set_value((const uint8_t*)status.c_str(),(uint32_t)status.length());

        // actually toggle the led

        executeLedToggle();

}

