/*
* ZatarDeviceAuth.cpp
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This is the Zatar Custom Object Code
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

#include "ZatarDeviceAuth.h"

// changed for ZATAR - moved from zatarDeviceAuth.h "public" section

const String DEVICE_TOKEN_RES_ID = "0";
const String AUTH_STATUS_RES_ID = "1";

const String LWM2M_ZATAR_DEVICE_AUTH_OBJ_ID = "23854";

const String &DEVICE_TOKEN = "<DEVICE_TOKEN>";    // INSERT YOUR DCT HERE that you obtained from developer.zatar.com account

int64_t authStatus = 0;

ZatarDeviceAuth* ZatarDeviceAuth::_instance = NULL;

ZatarDeviceAuth* ZatarDeviceAuth::get_instance() {
	return _instance == NULL ? new ZatarDeviceAuth() : _instance;
}

void ZatarDeviceAuth::delete_instance() {
	if(_instance) {
		delete _instance;
		_instance = NULL;
	}
}

ZatarDeviceAuth::ZatarDeviceAuth():M2MObject(LWM2M_ZATAR_DEVICE_AUTH_OBJ_ID) {

	_zatar_Auth_instance = M2MObject::create_object_instance();

	if(_zatar_Auth_instance) {
		_zatar_Auth_instance->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
		create_dev_token_resource();
		create_auth_status();
	}
}

ZatarDeviceAuth::~ZatarDeviceAuth() {
}


M2MResource* ZatarDeviceAuth::create_dev_token_resource() {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;
	if(!is_resource_present(ZatarDeviceAuth::DeviceToken) &&
		DEVICE_TOKEN.size() <= MAX_ALLOWED_STRING_LENGTH &&
		_zatar_Auth_instance) {
			res = _zatar_Auth_instance->create_dynamic_resource(DEVICE_TOKEN_RES_ID,
								OMA_RESOURCE_TYPE,
								M2MResourceInstance::STRING,
								true, false);
			if(res ) {
				res->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
				res->set_operation(operation);
				if (DEVICE_TOKEN.empty()) {
					res->clear_value();
				} else {
					res->set_value((const uint8_t*)DEVICE_TOKEN.c_str(),
										(uint32_t)DEVICE_TOKEN.length());
				}
			}
	}
	return res;
}

M2MResource* ZatarDeviceAuth::create_auth_status() {
	M2MResource* res = NULL;
	M2MBase::Operation operation = M2MBase::GET_PUT_ALLOWED;
	if(!is_resource_present(ZatarDeviceAuth::AuthenticationStatus) &&
		_zatar_Auth_instance) {
			res = _zatar_Auth_instance->create_dynamic_resource(AUTH_STATUS_RES_ID,
								OMA_RESOURCE_TYPE,
								M2MResourceInstance::INTEGER,
								true, false);

			if(res) {
				res->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
				char *buffer = (char*)memory_alloc(20);
				if(buffer) {
					int size = snprintf(buffer, 20,"%lld",authStatus);

						res->set_operation(operation);
						res->set_value((const uint8_t*)buffer,
										(const uint32_t)size);
						memory_free(buffer);
				}
			}
	}
	return res;
}


String ZatarDeviceAuth::resource_value_string(ZatarAuthResource resource, uint16_t instance_id) const {
	String value = "";
	M2MResourceInstance* res = get_resource_instance(resource, instance_id);
	if(res) {
		if(ZatarDeviceAuth::DeviceToken == resource) {

			uint8_t* buffer = NULL;
			uint32_t length = 0;
			//get and convert to string
			res->get_value(buffer,length);

			char *char_buffer = (char*)malloc(length+1);
			if(char_buffer) {
				memset(char_buffer,0,length+1);
				memcpy(char_buffer,(char*)buffer,length);

				String s_name(char_buffer);
				value = s_name;
				if(char_buffer) {
					free(char_buffer);
				}
			}
			if(buffer) {
				free(buffer);
			}
		}
	}
	return value;
}

int64_t ZatarDeviceAuth::resource_value_int(ZatarAuthResource resource, uint16_t instance_id) const {
	int64_t value = -1;
	M2MResourceInstance* res =  get_resource_instance(resource, instance_id);
	if(res) {
		if(ZatarDeviceAuth::AuthenticationStatus == resource ) {
			//get and convert to int
			uint8_t* buffer = NULL;
			uint32_t length = 0;
			res->get_value(buffer,length);
			if(buffer) {
				value = atoi((const char*)buffer);
				free(buffer);
			}
		}
	}
	return value;
}

bool ZatarDeviceAuth::is_resource_present(ZatarAuthResource resource) const {
	return get_resource_instance(resource, 0) != NULL;
}

uint16_t ZatarDeviceAuth::per_resource_count(ZatarAuthResource resource) const {
	uint16_t count = 0;
	if(_zatar_Auth_instance) {
		count = _zatar_Auth_instance->resource_count(resource_name(resource));
	}
	return count;
}

uint16_t ZatarDeviceAuth::total_resource_count() const {
	uint16_t count = 0;
	if(_zatar_Auth_instance) {
		count = _zatar_Auth_instance->resources().size();
	}
	return count;
}

M2MResourceInstance* ZatarDeviceAuth::get_resource_instance(ZatarAuthResource resource, uint16_t instance_id) const {
	M2MResource* res = NULL;
	M2MResourceInstance* inst = NULL;
	if(_zatar_Auth_instance) {
		res = _zatar_Auth_instance->resource(resource_name(resource));
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

String ZatarDeviceAuth::resource_name(ZatarAuthResource resource) const {
	String name;
	switch(resource) {
	case DeviceToken:
		name = DEVICE_TOKEN_RES_ID;
		break;
	case AuthenticationStatus:
		name = AUTH_STATUS_RES_ID;
		break;
	default:
		name = "";
		break;
	}
	return name;
}

