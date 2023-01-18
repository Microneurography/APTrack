#ifndef __USTEPPER_H__
#define __USTEPPER_H__

#include <SerialLib.h>
#include <string.h>

class UStepper
{
public:
UStepper();
~UStepper();
void initialize();
bool isConnected();
void setPosition(float voltage);
void setRelativePosition(float voltage);

private:
ofSerial serial;

};

#endif