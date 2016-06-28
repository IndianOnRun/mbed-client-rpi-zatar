
/*
* main.cpp
*
* (c) Zebra Technologies Corporation 2016
* Copyright (C) 2016 ZIH Corp and/or its affiliates.
*
* Description:
*         This class contains the main logic and the starting point of the example
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

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h> /* For SIGIGN and SIGINT */
#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconstants.h"
#include "security.h"
#include "ns_trace.h"

// added for ZATAR
#include "ZatarDeviceAuth.h"
#include "LWM2MConnectivity.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include "LedObject.h"
#include "ButtonObject.h"
#include "GPIOClass.h"
#include <string>
#include "config.h"

const String &MBED_USER_NAME_DOMAIN = MBED_DOMAIN;
const String &ENDPOINT_NAME = MBED_ENDPOINT_NAME;

//const uint8_t STATIC_VALUE[] = "Static value";

static void ctrl_c_handle_function(void);
typedef void (*signalhandler_t)(int); /* Function pointer type for ctrl-c */

// added for ZATAR
ZatarDeviceAuth* zatar_auth_object;
LWM2MConnectivity* connectivity_object;
LedObject *led1object;
LedObject *led2object;
LedObject *led3object;
ButtonObject *buttonObject;
bool ctrl_c_pressed;

class MbedClient: public M2MInterfaceObserver {
public:
    MbedClient(){
        _security = NULL;
        _interface = NULL;
        _device = NULL;
        _keepAliveObject = NULL;
        _error = false;
        _registered = false;
        _unregistered = false;
        _registration_updated = false;
        _value = 0;
    }

    ~MbedClient() {
        if(_security) {
            delete _security;
        }
        if( _register_security){
            delete _register_security;
        }
        if(_device) {
            M2MDevice::delete_instance();
        }
        if(_keepAliveObject) {
            delete _keepAliveObject;
        }
        if(_interface) {
            delete _interface;
        }
    }

    bool create_interface(String serialNo) {

       time_t t;
       srand((unsigned) time(&t));

       uint16_t port = rand() % 65535 + 12345;

       int i;

       // append board serial number to endpoint name

       char endpointName1[80] = "lwm2m-endpoint-RaspberryPiIotKit-";

       int j;
       for(i=0;i<80;i++){
           if(endpointName1[i]=='\0'){
               for(j=0;j<80;j++){
                   endpointName1[j+i]=serialNo[j];
                   if(serialNo[j]=='\0'){
                       endpointName1[j+i-1] = 0;
                       break;
                   }
               }
           break;
           }
       }

       const String &ENDPOINT = &endpointName1[0];


       _interface = M2MInterfaceFactory::create_interface(*this,
                                                  ENDPOINT,
                                                  "rpi_sample",
                                                  INTERFACE_LIFETIME,
                                                  port,
                                                  MBED_USER_NAME_DOMAIN,
                                                  M2MInterface::UDP,
                                                  M2MInterface::LwIP_IPv4,
                                                  "");
        printf("\nEndpoint Name : %s\n", ENDPOINT.c_str());
        return (_interface == NULL) ? false : true;
    }



    bool register_successful() {
        while(!_registered && !_error) {
            printf("\n...not registered yet...sleeping...");
            sleep(1);
        }
        return _registered;
    }

    bool isRegisteredNow(){

	if(_registered){
		return 1;
	}
	else{
		return 0;
	}
    }

    bool unregister_successful() {
        while(!_unregistered && !_error) {
	   sleep(1);
        }
        return _unregistered;
    } 

    bool registration_update_successful() {
        while(!_registration_updated && !_error) {
        }
        return _registration_updated;
    }

    bool create_register_object() {
        bool success = false;
        _register_security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);
        if(_register_security) {

            char buffer[6];
            sprintf(buffer,"%d",MBED_SERVER_PORT);
            m2m::String port(buffer);

            m2m::String addr = MBED_SERVER_ADDRESS;
            addr.append(":", 1);
            addr.append(port.c_str(), size_t(port.size()) );
	    printf("\nTarget server address = %s\n", addr.c_str());

	    if(SECURITY == false){

		// SECURITY setting is from config.h

	        _register_security->set_resource_value(M2MSecurity::M2MServerUri, addr);
        	_register_security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);
		}
	    else{
            	_register_security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::Certificate);
            	_register_security->set_resource_value(M2MSecurity::ServerPublicKey,SERVER_CERT,sizeof(SERVER_CERT));
            	_register_security->set_resource_value(M2MSecurity::PublicKey,CERT,sizeof(CERT));
            	_register_security->set_resource_value(M2MSecurity::Secretkey,KEY,sizeof(KEY));
		}
            success = true;
        }
	else{
	    printf("\n\n...ERROR - create _register_security failed... terminating program.\n\n");
            exit(1);
	}
        return success;
    }

    bool create_device_object(String serialNo) {
        bool success = false;


//     first strip off offending chars from serial number string

       int i;

       for(i=0;i<80;i++) {
            if(serialNo[i]=='\0'){
               serialNo[i-1]=0;
               break;
            }
       }

       const String &SNUMBER = &serialNo[0];

        _device = M2MInterfaceFactory::create_device();
        if(_device) {
            if(_device->create_resource(M2MDevice::Manufacturer,MANUFACTURER)     &&
               _device->create_resource(M2MDevice::DeviceType,TYPE)        &&
               _device->create_resource(M2MDevice::ModelNumber,MODEL_NUMBER)      &&
               _device->create_resource(M2MDevice::SerialNumber,SNUMBER) &&
               _device->create_resource(M2MDevice::FirmwareVersion, FIRMWARE_VERSION)) {
                success = true;
            }
        }
        return success;
    }


    ZatarDeviceAuth* create_zatar_auth_object() {
        return ZatarDeviceAuth::get_instance();
    }

    bool create_led_object(int objNum){
        if(1){
            if(objNum == 1){    // red led
              led1object = new LedObject(objNum);
              if(led1object){
                led1object->create_text_resource(led1object->getMyStatus());
                led1object->create_toggle_led_resource("OFF");
                led1object->setLed();
                return true;
              }
              else return false;
            }
            else if(objNum == 2){    // yellow led
              led2object = new LedObject(objNum);
              if(led2object){
                led2object->create_text_resource(led2object->getMyStatus());
                led2object->create_toggle_led_resource("OFF");
                led2object->setLed();
                return true;
              }
              else return false;
            }
            else if(objNum == 3){    // green led
              led3object = new LedObject(objNum);
              if(led3object){
                led3object->create_text_resource(led3object->getMyStatus());
                led3object->create_toggle_led_resource("OFF");
                led3object->setLed();
                return true;
              }
              else return false;
            }
        return false;
        }
        return false;
    }

    bool create_button_object(){
        buttonObject = new ButtonObject;
        if(buttonObject)return true;
        else return false;
    }


    LWM2MConnectivity* create_connectivity_object(String ipadd) {
        LWM2MConnectivity* connectivity = LWM2MConnectivity::get_instance();

       const String &IPADDRESS = &ipadd[0];


        if(connectivity) {

/*	Lightweight Machine-to-Machine (LWM2M) connectivity object
	Several resources are possible - we will only implement the "IPADDRESS" resource for this sample
	See the LWM2MConnectivity.cpp code for more details
*/

            //connectivity->create_resource(LWM2MConnectivity::NetworkBearer, 41);
            //connectivity->create_resource(LWM2MConnectivity::AvailableNetworkBearer, 41);
            //connectivity->create_resource(LWM2MConnectivity::RadioSignalStrength, 0);
            connectivity->create_resource(LWM2MConnectivity::IPAddresses, IPADDRESS);
        }
        return connectivity;
    }


/*

This was modified from the original code supplied by ARM

It now is used to create a "phantom object" which does not appear in the Zatar Avatar Definition

But is used as a "keep alive" to keep the Avatar in Online state

By updating the dynamic resource to a new value every 30 seconds

*/
    bool create_keepAlive_object() {
        bool success = false;
        _keepAliveObject = M2MInterfaceFactory::create_object("24001");
        if(_keepAliveObject) {
            M2MObjectInstance* inst = _keepAliveObject->create_object_instance();
            if(inst) {
		success = true;
                M2MResource* res = inst->create_dynamic_resource("1001",
                                                                 OMA_RESOURCE_TYPE,
                                                                 M2MResourceInstance::INTEGER,
                                                                 true);
                char buffer[20];
                int size = sprintf(buffer,"%d",_value);
                  res->set_operation(M2MBase::GET_PUT_ALLOWED);
                  res->set_value((const uint8_t*)buffer,
                                 (const uint32_t)size);
                _value++;

/*
                inst->create_static_resource("1000",
                                             OMA_RESOURCE_TYPE,
                                             M2MResourceInstance::STRING,
                                             STATIC_VALUE,
                                             sizeof(STATIC_VALUE)-1);
*/
            }
        }
        return success;
    }

    void update_keepAlive_resource() {
        if(_keepAliveObject) {
            M2MObjectInstance* inst = _keepAliveObject->object_instance();
            if(inst) {
                M2MResource* res = inst->resource("1001");
                printf("%d\n", _value);
                char buffer[20];
                int size = sprintf(buffer,"%d",_value);
                res->set_value((const uint8_t*)buffer,
                               (const uint32_t)size);
                _value++;
            }
        }
    }

    void register_lwm2m_device(){
        M2MObjectList object_list;
        object_list.push_back(_device);
        object_list.push_back(_keepAliveObject);
       // added for ZATAR RPi demo - need to add all objects that are in the Zatar Avatar Definition in order for them to appear in the Avatar
	object_list.push_back(zatar_auth_object);
	object_list.push_back(connectivity_object);
	object_list.push_back(led1object);
	object_list.push_back(led2object);
	object_list.push_back(led3object);
        object_list.push_back(buttonObject);
    if(_interface) {
        _interface->register_object(_register_security,object_list);
        }else {
        printf("\n\nInterface doesn't exist, exiting!!\n");
            exit(1);
    	}
    }

    void update_register() {
	printf("\n\n...inside update_register\n\n");
        uint32_t updated_lifetime = 100;
        if(_interface) {
            _interface->update_registration(_register_security,updated_lifetime);
        }

    }

    void unregister() {
        if(_interface) {
            _interface->unregister_object(NULL);
        }
    }

    void bootstrap_done(M2MSecurity */*server_object*/){
    }

    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
        _registered = true;
        printf("\n\n***Registered***\n");
    }

    void object_unregistered(M2MSecurity */*server_object*/){
        _unregistered = true;
        printf("\nUnregistered\n");
    }

    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        _registration_updated = true;

    }

    void error(M2MInterface::Error error){
        _error = true;
        String error_code;
	switch(error) {
        case M2MInterface::ErrorNone:
             error_code += "M2MInterface::ErrorNone";
             break;
            case M2MInterface::AlreadyExists:
             error_code += "M2MInterface::AlreadyExists";
             break;
            case M2MInterface::BootstrapFailed:
             error_code += "M2MInterface::BootstrapFailed";
             break;
            case M2MInterface::InvalidParameters:
             error_code += "M2MInterface::InvalidParameters";
             break;
            case M2MInterface::NotRegistered:
             error_code += "M2MInterface::NotRegistered";
             break;
            case M2MInterface::Timeout:
             error_code += "M2MInterface::Timeout";
             break;
            case M2MInterface::NetworkError:
             error_code += "M2MInterface::NetworkError check Internet connection";
             break;
            case M2MInterface::ResponseParseFailed:
             error_code += "M2MInterface::ResponseParseFailed";
             break;
            case M2MInterface::UnknownError:
             error_code += "M2MInterface::UnknownError";
             break;
            case M2MInterface::MemoryFail:
             error_code += "M2MInterface::MemoryFail";
             break;
            case M2MInterface::NotAllowed:
             error_code += "M2MInterface::NotAllowed";
            break;
        }
        printf("\nError occured  : %s\n", error_code.c_str());
       	ctrl_c_handle_function();   // exit on error
    }


bool buttonIsPressed(GPIOClass *gpioButton){

   // check for button press

    string inputState = "";
    gpioButton->getval_gpio(inputState);

    if(inputState == "1"){
          return 1;
    }
    else{
        return 0;
    }

    return -1;
}

void value_updated(M2MBase *base, M2MBase::BaseType type) {

//	this callback occurs when the lwm2m server updates a resource on the lwm2m device

	printf("\nLwM2M Resource has been updated\n");

    }

private:

    M2MInterface        *_interface;
    M2MSecurity         *_security;
    M2MSecurity         *_register_security;
    M2MDevice           *_device;
    M2MObject           *_keepAliveObject;
    bool                _error;
    bool                _registered;
    bool                _unregistered;
    bool                _registration_updated;
    int                 _value;
};

void* wait_for_unregister(void* arg) {
    MbedClient *client;
    client = (MbedClient*) arg;
    if(client->unregister_successful()) {
        printf("Unregistered.\n");
    }
    return NULL;
}

void* send_keepAlive_update(void* arg) {

/* this code is run in separate thread off the main
   to update 'phantom' resource to keep the lwm2m connection with Zatar
   alive.
*/

    MbedClient *client;
    client = (MbedClient*) arg;
    static uint8_t counter = 0;
    while(1) {
        sleep(1);
        if(counter >= KEEP_ALIVE_INTERVAL &&
           client->isRegisteredNow()) {
            printf("Sending keep-alive update to Zatar...");
            client->update_keepAlive_resource();
            counter = 0;
        }
        else
            counter++;
    }
    return NULL;
}

void* update_register(void* arg) {
    MbedClient *client;
    client = (MbedClient*) arg;
    static uint8_t counter = 0;
    while(1) {
        sleep(INTERFACE_LIFETIME);
        if(client->register_successful()) {
            client->update_register();
        }

    }
    return NULL;
}

void* watchForButtonPresses(void* arg){
    MbedClient *client;
    client = (MbedClient*) arg;
    void* text;

    GPIOClass *gpio11 = new GPIOClass("11"); // create link to button on RPi BCM 11

    gpio11->setdir_gpio("in");  // set it to input

    // set up local pibrella leds - these leds will be used for local feedback only

    GPIOClass *gpio22 = new GPIOClass("22");
    gpio22->setdir_gpio("out");
    GPIOClass *gpio23 = new GPIOClass("23");
    gpio23->setdir_gpio("out");
    GPIOClass *gpio24 = new GPIOClass("24");
    gpio24->setdir_gpio("out");
    GPIOClass *gpio25 = new GPIOClass("25");
    gpio25->setdir_gpio("out");


    while(!ctrl_c_pressed){

	bool triggered = false;
	int i;

	// wait for button down - check once every BUTTON_WAIT_1 intervals; display pattern on local leds 
	// and change pattern every BUTTON_WAIT_DISPLAY_MULTIPLE times BUTTON_WAIT_1 intervals
	// bomb out as soon as you detect either button_down OR ctrl-c-pressed

	while(!ctrl_c_pressed && !triggered){

	  for(i=0;i<BUTTON_WAIT_DISPLAY_MULTIPLE;i++){

	    if(client->buttonIsPressed(gpio11) || ctrl_c_pressed || triggered){

		triggered = true;
		break;
	    }
	    else{

          	// POSITION 1 - send pattern to local leds for user feedback

	  	gpio22->setval_gpio("1");
	  	gpio23->setval_gpio("0");
	  	gpio24->setval_gpio("0");
	  	gpio25->setval_gpio("0");

		usleep(BUTTON_WAIT_1);
		}

	  }

	  for(i=0;i<BUTTON_WAIT_DISPLAY_MULTIPLE;i++){

	    if(client->buttonIsPressed(gpio11) || ctrl_c_pressed || triggered){

		triggered = true;
		break;
	    }
	    else{

          	// POSITION 2 - send pattern to local leds for user feedback

	  	gpio22->setval_gpio("0");
	  	gpio23->setval_gpio("0");
	  	gpio24->setval_gpio("1");
	  	gpio25->setval_gpio("0");

		usleep(BUTTON_WAIT_1);
		}

	  }

	  for(i=0;i<BUTTON_WAIT_DISPLAY_MULTIPLE;i++){

	    if(client->buttonIsPressed(gpio11) || ctrl_c_pressed || triggered){

		triggered = true;
		break;
	    }
	    else{

          	// POSITION 3 - send pattern to local leds for user feedback

	  	gpio22->setval_gpio("0");
	  	gpio23->setval_gpio("1");
	  	gpio24->setval_gpio("0");
	  	gpio25->setval_gpio("0");

		usleep(BUTTON_WAIT_1);
		}

	  }

	  for(i=0;i<BUTTON_WAIT_DISPLAY_MULTIPLE;i++){

	    if(client->buttonIsPressed(gpio11) || ctrl_c_pressed || triggered){

		triggered = true;
		break;
	    }
	    else{

          	// POSITION 4 - send pattern to local leds for user feedback

	  	gpio22->setval_gpio("0");
	  	gpio23->setval_gpio("0");
	  	gpio24->setval_gpio("0");
	  	gpio25->setval_gpio("1");

		usleep(BUTTON_WAIT_1);
		}

	  }

	}

        if(!ctrl_c_pressed){

		// button down detected

		// send pattern to local leds for user feedback

		gpio22->setval_gpio("1");
		gpio23->setval_gpio("1");
		gpio24->setval_gpio("1");
		gpio25->setval_gpio("1");

            	buttonObject->buttonDownDetected();
        }

	// wait for button up

	while(client->buttonIsPressed(gpio11) && !ctrl_c_pressed){usleep(100000);}

	if(!ctrl_c_pressed){

		// button up detected

            	buttonObject->buttonUpDetected();
        }
    }

    gpio22->setval_gpio("0");
    gpio23->setval_gpio("0");
    gpio24->setval_gpio("0");
    gpio25->setval_gpio("0");

    delete gpio11;

    pthread_exit(NULL);

}

static MbedClient *mbedclient = NULL;

static void ctrl_c_handle_function(void)
{

    ctrl_c_pressed = true;

}

/*

    added utility methods for zatar

*/


static String  getCurrentIPAddress(void){

    String returnValue = "";
    struct ifaddrs *addrs, *temp_ifaddrs;
    char address[NI_MAXHOST];

    int ipAdd, n;

    if(getifaddrs(&addrs) == -1){
        printf("\n\nERROR in getCurrentIPAddress\n\n");
        return "ERROR";
    }

    for(temp_ifaddrs = addrs, n=0; temp_ifaddrs != NULL; temp_ifaddrs = temp_ifaddrs->ifa_next, n++){
        if(temp_ifaddrs->ifa_addr == NULL) continue;

        ipAdd = temp_ifaddrs->ifa_addr->sa_family;
        returnValue = temp_ifaddrs->ifa_name;

            if(returnValue == "wlan0" && ipAdd == AF_INET) {

                int s = getnameinfo(temp_ifaddrs->ifa_addr, sizeof(struct sockaddr_in), address, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

                if( s != 0){
                    printf("\n\nERROR getting IPAddress!\n\n");
                    return "ERROR";
                }

                return(address);
            }
            else if(returnValue == "eth0" && ipAdd == AF_INET) {

                int s = getnameinfo(temp_ifaddrs->ifa_addr, sizeof(struct sockaddr_in), address, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

                if( s != 0){
                    printf("\n\nERROR getting IPAddress!\n\n");
                    return "ERROR";
                }

                return(address);
            }

    }

    return returnValue;

}

String getSerialNumber(void){

    String errMsg = "unable to determine serial number";

    FILE *fp = fopen("/proc/cpuinfo", "r");

    if(!fp){
        perror("\n\ngetSerialNumber --> cpuinfo file open Failure\n\n");
        return errMsg;
    }
    else{
        int max = 80;
        int i=0;
        char line[100];

        while(fgets(line, max, fp)!=NULL){
             if(!strncmp(line, "Serial", 6)){
                // scan through char array to find only the serial number part
                while(line[i]!='0'){
                    if(line[i] == '\0'){
                        perror("\n\ngetSerialNumber --> ERROR EOL detected - exiting\n\n");
                        return errMsg;
                    }
                    i++;
                }
                while(line[i]=='0'){
                    if(line[i] == '\0'){
                        perror("\n\ngetSerialNumber --> ERROR EOL detected - exiting\n\n");
                        return errMsg;
                    }
                    i++;
                }
                // i is now pointing to the start of the real serial number
                fclose(fp);
                char *newLine;
                newLine = &line[i];
                return newLine;
            }
        }
    }
    fclose(fp);
    return errMsg;

}

void initializeGPIOPins(void){

	// run once at beginning of main this code to  make sure GPIO pins are avaialble for use

	printf("\n...initializing GPIO pins to unexported state...\n");
	char output[80];
	sprintf(output, "echo %c27%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c17%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c4%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c11%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c22%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c23%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c24%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);
	sprintf(output, "echo %c25%c > /sys/class/gpio/unexport", 0x22, 0x22);
	system(output);

}


int main() {

    ctrl_c_pressed = false;

    initializeGPIOPins();

    system("clear");
    printf("\n\n****************************************************************");
    printf("\n\nWelcome to the Zatar Raspberry Pi IoT Kit demo\n\n");
    printf("****************************************************************");
    printf("\n--> Make sure your pi is already connected to your local wifi\n and has Internet access prior to running this program.\n");
    printf("\n--> If you have a pibrella board attached to your pi you will\n have access to the red, yellow, and green leds from the cloud.\n");
    printf("\n--> Note: You can always break out of this program by hitting\n ctrl-c on your keyboard.\n\n");
    printf("****************************************************************\n");

    pthread_t observation_thread;    // being used as keep-alive for Zatar connection

    pthread_t buttonWatcherThread;

    MbedClient mbed_client;

    mbedclient = &mbed_client;

    String boardSerialNumber = "unknown";

    String ipAddress = "unknown";

    boardSerialNumber = getSerialNumber();

    ipAddress = getCurrentIPAddress();

    signal(SIGINT, (signalhandler_t)ctrl_c_handle_function);
    trace_init();

    printf("\nStarting up...\n");

    printf("\nDetected IP address is %s\r", ipAddress.c_str());
    printf("\n\nDetected Serial number: %s",boardSerialNumber.c_str());

    bool result = mbed_client.create_interface(boardSerialNumber);
    if(true == result) {
        printf("\nInterface created\n");
    }
    result = mbed_client.create_register_object();
    if(true == result) {
        printf("\nRegister object created\n");
    }

    result = mbed_client.create_device_object(boardSerialNumber);
    if(true == result){
        printf("\nDevice object created\n");
    }

    result = mbed_client.create_keepAlive_object();

    if(true == result) {
        printf("\nKeep Alive Object created\n");
    }

    connectivity_object = mbed_client.create_connectivity_object(ipAddress);
    if(connectivity_object){
        printf("\nConnectivity object created\n");
    }
    else{
        printf("\nERROR - Connectivity object not created\n");
        exit(1);
    }

    zatar_auth_object = mbed_client.create_zatar_auth_object();
    if(zatar_auth_object){
        printf("\nZatar auth object created\n");
    }
    else{
        printf("\nERROR - Zatar auth object not created\n");
        exit(1);
    }

    result = mbed_client.create_led_object(1);
    if(result == true){
        printf("\nCreated Led1 Object\n");
    }
    else{
        printf("\nERROR = failed to create Led1 Object\n");
    }
    result = mbed_client.create_led_object(2);
    if(result == true){
        printf("\nCreated Led2 Object\n");
    }
    else{
        printf("\nERROR - failed to create Led2 Object\n");
    }
    result = mbed_client.create_led_object(3);
    if(result == true){
        printf("\nCreated Led3 Object\n");
    }
    else{
        printf("\nERROR - failed to create Led3 Object\n");
    }

    result = mbed_client.create_button_object();
    if(result == true){
        printf("\nCreated Button Object\n");
    }
    else{
        printf("\nERROR - failed to create Button Object\n");
    }

    printf("\nRegistering LwM2M Device on Zatar...\n");
    mbed_client.register_lwm2m_device();

    if(mbedclient->register_successful()){
        printf("\n--> Success!!\n\nNow simply go onto Zatar and claim your Pi.\n\n");
        printf("Make sure you enter the serial number of this device.\n\n");
        printf("Serial Number: %s\n", boardSerialNumber.c_str());
        printf("(type ctrl-c to exit)\n\n");
    }
    else{
        printf("\n\nSorry could not register device on Zatar. Pls check instructions and try again.\n\n");
        exit(1);
    }

    pthread_create(&observation_thread, NULL, &send_keepAlive_update, (void*) &mbed_client);
    pthread_create(&buttonWatcherThread, NULL, &watchForButtonPresses, (void*) &mbed_client);

    while(!ctrl_c_pressed)usleep(100000);

    pthread_join(buttonWatcherThread, NULL);   // block until buttonwatcher is done

    // turn off leds before exiting

    led1object->clearLed();
    led2object->clearLed();
    led3object->clearLed();

    delete led1object;
    delete led2object;
    delete led3object;
    delete buttonObject;

    printf("\n\nMain...ERROR detected...exiting...\n\n");

    exit(1);

    return 0;
}

