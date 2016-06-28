/*
* LWMConnectivity.cpp
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This class contains the Connectivity Object for the LwM2M device
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

#include "LWM2MConnectivity.h"

// changed for ZATAR - moved from LWM2MConnectiviy.h "public" section
const String NETWORK_BEARER_RES_ID = "0";
const String AVAILABLE_NET_BREADER_RES_ID = "1";
const String RADIO_SIGNAL_STRG = "2";
const String IP_ADRESS = "4";

LWM2MConnectivity* LWM2MConnectivity::_instance = NULL;

LWM2MConnectivity* LWM2MConnectivity::get_instance() {
	return _instance == NULL ? new LWM2MConnectivity() : _instance;
}

void LWM2MConnectivity::delete_instance() {
	if(_instance) {
		delete _instance;
		_instance = NULL;
	}
}

LWM2MConnectivity::LWM2MConnectivity():M2MObject(M2M_CONNECTIVITY_MONITOR_ID) {
	set_observable(true);
	_connectivity_instance = M2MObject::create_object_instance();

	if(_connectivity_instance) {
		_connectivity_instance->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
		_connectivity_instance->set_observable(true);
	}
}

LWM2MConnectivity::~LWM2MConnectivity() {
}


M2MResource* LWM2MConnectivity::create_resource(ConnectivityResource resource, const String &value) {
	M2MResource* res = NULL;
	String resource_id = "";
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;
	if(!is_resource_present(resource) && value.size() <= MAX_ALLOWED_STRING_LENGTH) {
		switch(resource) {
		case IPAddresses:
			resource_id = IP_ADRESS;
			break;
		default:
			break;
		}
	}

	if(!resource_id.empty()) {
		if(_connectivity_instance) {
			res = _connectivity_instance->create_dynamic_resource(resource_id,
					OMA_RESOURCE_TYPE,
					M2MResourceInstance::STRING,
					true, false);
			if(res ) {
				res->set_coap_content_type(COAP_CONTENT_OMA_TLV_TYPE);
				res->set_operation(operation);
				if (value.empty()) {
					res->clear_value();
				} else {
					res->set_value((const uint8_t*)value.c_str(),
							(uint32_t)value.length());
				}
			}
		}
	}
	return res;
}

M2MResource* LWM2MConnectivity::create_resource(ConnectivityResource resource, int64_t value) {
	M2MResource* res = NULL;
	String resource_id = "";
	M2MBase::Operation operation = M2MBase::GET_ALLOWED;
	if(!is_resource_present(resource)) {
		switch(resource) {
		case NetworkBearer:
			resource_id = NETWORK_BEARER_RES_ID;
			break;
		case AvailableNetworkBearer:
			resource_id = AVAILABLE_NET_BREADER_RES_ID;
			break;
		case RadioSignalStrength:
			resource_id = RADIO_SIGNAL_STRG;
			break;
		default:
			break;
		}
	}

	if(!resource_id.empty()) {
		if(_connectivity_instance) {
			res = _connectivity_instance->create_dynamic_resource(resource_id,
					OMA_RESOURCE_TYPE,
					M2MResourceInstance::INTEGER,
					true, false);

			if(res) {
				char *buffer = (char*)memory_alloc(20);
				if(buffer) {
					int size = snprintf(buffer, 20,"%lld",value);

					res->set_operation(operation);
					res->set_value((const uint8_t*)buffer,
							(const uint32_t)size);
					memory_free(buffer);
				}
			}
		}
	}
	return res;
}

M2MResourceInstance* LWM2MConnectivity::create_resource_instance(ConnectivityResource resource, int64_t value, uint16_t instance_id) {
	M2MResourceInstance* res = NULL;
	String resource_id = "";

	if(AvailableNetworkBearer ==  resource) {
		resource_id = AVAILABLE_NET_BREADER_RES_ID;
	}

	if(!resource_id.empty()) {
		if(_connectivity_instance) {
			res = _connectivity_instance->create_dynamic_resource_instance(resource_id,
					OMA_RESOURCE_TYPE,
					M2MResourceInstance::INTEGER,
					true, instance_id);
		}

		if(res) {
			char *buffer = (char*)memory_alloc(20);
			if(buffer) {
				int size = snprintf(buffer, 20,"%lld",value);
				// Only read operation is allowed for above resources
				res->set_operation(M2MBase::GET_ALLOWED);
				res->set_value((const uint8_t*)buffer,
						(const uint32_t)size);
				memory_free(buffer);
			}
		}
	}
	return res;
}

String LWM2MConnectivity::resource_value_string(ConnectivityResource resource, uint16_t instance_id) const {
	String value = "";
	M2MResourceInstance* res = get_resource_instance(resource, instance_id);
	if(res) {
		if(LWM2MConnectivity::IPAddresses == resource) {

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

int64_t LWM2MConnectivity::resource_value_int(ConnectivityResource resource, uint16_t instance_id) const {
	int64_t value = -1;
	M2MResourceInstance* res =  get_resource_instance(resource, instance_id);
	if(res) {
		if(LWM2MConnectivity::NetworkBearer == resource ||
				LWM2MConnectivity::AvailableNetworkBearer == resource ||
				LWM2MConnectivity::RadioSignalStrength == resource) {
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

bool LWM2MConnectivity::is_resource_present(ConnectivityResource resource) const {
	return get_resource_instance(resource, 0) != NULL;
}

uint16_t LWM2MConnectivity::per_resource_count(ConnectivityResource resource) const {
	uint16_t count = 0;
	if(_connectivity_instance) {
		count = _connectivity_instance->resource_count(resource_name(resource));
	}
	return count;
}

uint16_t LWM2MConnectivity::total_resource_count() const {
	uint16_t count = 0;
	if(_connectivity_instance) {
		count = _connectivity_instance->resources().size();
	}
	return count;
}

M2MResourceInstance* LWM2MConnectivity::get_resource_instance(ConnectivityResource resource, uint16_t instance_id) const {
	M2MResource* res = NULL;
	M2MResourceInstance* inst = NULL;
	if(_connectivity_instance) {
		res = _connectivity_instance->resource(resource_name(resource));
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

String LWM2MConnectivity::resource_name(ConnectivityResource resource) const {
	String name;
	switch(resource) {
	case NetworkBearer:
		name = NETWORK_BEARER_RES_ID;
		break;
	case AvailableNetworkBearer:
		name = AVAILABLE_NET_BREADER_RES_ID;
		break;
	case RadioSignalStrength:
		name = RADIO_SIGNAL_STRG;
		break;
	case IPAddresses:
		name = IP_ADRESS;
		break;
	default:
		name = "";
		break;
	}
	return name;
}


