#include "UStepper.h";

#ifdef _WINDOWS_
#include <windows.h>
#else

#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif

UStepper::UStepper()
{
}

UStepper::~UStepper()
{
}

void UStepper::initialize()
{

    std::cout << "Searching for UStepper..." << std::endl;

    //
    // lsusb shows Device 104: ID 1eaf:0004
    // updated udev rules file, but still need to run as root -- no idea why
    //
    // try this instead: sudo chmod o+rw /dev/ttyACM0
    //
    // works fine, but you have to re-do it every time
    //

    vector<ofSerialDeviceInfo> devices = serial.getDeviceList();

    string path, name;

    for (int i = 0; i < devices.size(); i++) //AB 29/9/17: cycle through devices to find PulsePal
    {
        int id = devices[i].getDeviceID();
        path = devices[i].getDevicePath();
        name = devices[i].getDeviceName();

        serial.setup(id, 115200);
        if (isConnected())
        {
            std::cout << "uStepper connected on port: " << name << std::endl;
            return;
        }
    }
}

bool UStepper::isConnected()
{

    unsigned char *handshakeMessage = (unsigned char *)"VER";
    serial.writeBytes(handshakeMessage, 3);
    Sleep(100);
    unsigned char responseBytes[1] = {0};
    serial.readBytes(responseBytes, 1);

    if (responseBytes[0] == 1)
    {
        std::cout << "Found UStepper! " << std::endl;
        return true;
    }
    return false;
}

void UStepper::setPosition(float voltage){
    std::string str_message = ("S" + std::to_string(voltage));
    unsigned char* message = (unsigned char*)str_message.c_str();
    serial.writeBytes(message, str_message.length());

  
}

void UStepper::setRelativePosition(float voltage){
    std::string str_message = ("D" + std::to_string(voltage));
    unsigned char* message = (unsigned char*)str_message.c_str();
    serial.writeBytes(message, str_message.length());
}