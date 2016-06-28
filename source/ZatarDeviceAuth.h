/*
* ZatarDeviceAuth.h
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This is the Zatar Custom Object header file
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

#ifndef SOURCE_ZATARDEVICEAUTH_H_
#define SOURCE_ZATARDEVICEAUTH_H_

#include "mbed-client/m2mobject.h"

class M2MResource;
class M2MResourceInstance;

/**
 * this class represent the Zatar Device authentication object
 */

class ZatarDeviceAuth : public M2MObject {

public:



	typedef enum {
		DeviceToken,
		AuthenticationStatus
	}ZatarAuthResource;


private:

	ZatarDeviceAuth();
	//prevent the assignment operator
	ZatarDeviceAuth& operator=(const ZatarDeviceAuth&);
	//prevent the use of copy constructor
	ZatarDeviceAuth(const ZatarDeviceAuth&);

	virtual ~ZatarDeviceAuth();

public:

	static ZatarDeviceAuth* get_instance();
	static void delete_instance();

	String resource_value_string(ZatarAuthResource resource, uint16_t instance_id = 0) const;
	int64_t resource_value_int(ZatarAuthResource resource, uint16_t instance_id = 0) const;
	bool is_resource_present(ZatarAuthResource resource)const;
	uint16_t total_resource_count()const;
	uint16_t per_resource_count(ZatarAuthResource resource)const;

private:

	M2MResource* create_dev_token_resource();
	M2MResource* create_auth_status();
	M2MResourceInstance* get_resource_instance(ZatarAuthResource dev_res, uint16_t instance_id) const;
	String resource_name(ZatarAuthResource resource) const;
	bool check_value_range(ZatarAuthResource resource, const int64_t value) const;

private :

	M2MObjectInstance* _zatar_Auth_instance;     //Not owned

protected:

	static ZatarDeviceAuth* _instance;
};

#endif /* SOURCE_ZATARDEVICEAUTH_H_ */
