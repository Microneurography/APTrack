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

    vector<ofSerialDeviceInfo> devices = serial.getDeviceList();

    string path, name;

    for (int i = 0; i < devices.size(); i++) //AB 29/9/17: cycle through devices to find PulsePal
    {
        int id = devices[i].getDeviceID();
        path = devices[i].getDevicePath();
        name = devices[i].getDeviceName();

        std::cout << "    looking on " << name << std::endl;
        serial.setup(id, 19200);
        //Sleep(1000);
        if (isConnected())
        {
            std::cout << "uStepper connected on port: " << name << std::endl;
            return;
        }
        serial.close();
    }
}

bool UStepper::isConnected()
{
    Sleep(5000);
    unsigned char handshakeMessage[6] = {'V', '\n', 'V', '\n', 'V', '\n'};
    auto written = serial.writeBytes(handshakeMessage, 6);
    //serial.flush();
    Sleep(1000);

    uint8_t responseBytes[5] = {0};
    auto read = serial.readBytes(responseBytes, 5);
    serial.flush();
    if (responseBytes[0] == '1')
    {
        std::cout << "Found UStepper! " << std::endl;
        return true;
    }

    return false;
}

void UStepper::setPosition(float voltage)
{
    std::string str_message = ("S" + std::to_string(voltage) + "\n");
    uint8_t *message = (uint8_t *)str_message.c_str();
    int response = serial.writeBytes(message, str_message.length());
    while (serial.available())
    {
        serial.readByte();
    }
}

void UStepper::setRelativePosition(float voltage)
{
    std::string str_message = ("D" + std::to_string(voltage) + "\n");
    uint8_t *message = (uint8_t *)str_message.c_str();
    int response = serial.writeBytes(message, str_message.length());
    //Sleep(1000);
    //serial.drain();
    serial.flush();
}
