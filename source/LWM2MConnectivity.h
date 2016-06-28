/*
* LWM2MConnectivity.h
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This is the header file for the LWM2MConnectivity class
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
#ifndef SOURCE_LWM2MCONNECTIVITY_H_
#define SOURCE_LWM2MCONNECTIVITY_H_

#include "mbed-client/m2mobject.h"

class M2MResource;
class M2MResourceInstance;

class LWM2MConnectivity : public M2MObject {

public:

	const static uint8_t GSM = 0;
	const static uint8_t TDSCDMAc = 1;
	const static uint8_t WCDMA = 2;
	const static uint8_t CDMA2000 = 3;
	const static uint8_t WiMAX = 4;
	const static uint8_t LTETDD = 5;
	const static uint8_t LTEFDD = 6;
	const static uint8_t WLAN = 21;
	const static uint8_t Bluetooth = 22;
	const static uint8_t IEEE802154 = 23;
	const static uint8_t Ethernet = 41;
	const static uint8_t DSL = 42;
	const static uint8_t PLC = 43;




	//start with these 4
	typedef enum {
		NetworkBearer,
		AvailableNetworkBearer,
		RadioSignalStrength,
		IPAddresses
	}ConnectivityResource;

private:

	/**
	 * constructor
	 */
	LWM2MConnectivity();
	//prevent the assignment operator
	LWM2MConnectivity& operator=(const LWM2MConnectivity&);
	//prevent the use of copy constructor
	LWM2MConnectivity(const LWM2MConnectivity&);

	virtual ~LWM2MConnectivity();

public:

	//only 1  should exist, make it as a singleton
	static LWM2MConnectivity* get_instance();
	static void delete_instance();

	M2MResource* create_resource(ConnectivityResource resource, const String &value);
	M2MResource* create_resource(ConnectivityResource resource, int64_t value);
	M2MResourceInstance* create_resource_instance(ConnectivityResource resource, int64_t value, uint16_t instance_id);
	String resource_value_string(ConnectivityResource resource, uint16_t instance_id = 0) const;
	int64_t resource_value_int(ConnectivityResource resource, uint16_t instance_id = 0) const;
	bool is_resource_present(ConnectivityResource resource)const;
	uint16_t total_resource_count()const;
	uint16_t per_resource_count(ConnectivityResource resource)const;

private:

	M2MResourceInstance* get_resource_instance(ConnectivityResource dev_res, uint16_t instance_id) const;
	String resource_name(ConnectivityResource resource) const;
	bool check_value_range(ConnectivityResource resource, const int64_t value) const;

private :

	M2MObjectInstance* _connectivity_instance;     //Not owned

protected:

	static LWM2MConnectivity* _instance;
};

#endif /* SOURCE_LWM2MCONNECTIVITY_H_ */
