/*
 * UStepper controller code
 * 
 * A simple interface into the uStepper.
 * 
 * This code can control a potentiometer for stimulus. 
 * It stores the number of rotations in case of power failure in EEPROM
 * 
 * The commands are 'S' for set, 'D' for diff
 * 'V' for printing the version
 * 'T' for Telling the current
 * 'A' for the Angle of the device
 * 'E' for printing the number of rotations stored in EEPROM
 * 
 */

#include <uStepperSLite.h>
#include <EEPROM.h>
#define EEPROMIDX 1
#define DEGREE_PER_UA 360

uStepperSLite stepper(100000, 500);

float oldValue = 0;
uint16_t numRotationsSinceReset = 1;

void setup()
{
    // put your setup code here, to run once:
    stepper.setup(NORMAL, SIXTEEN, 10.0, 5.0, true);
    Serial.begin(19200);
    //while ( !Serial );
    stepper.stop(false);
    EEPROM.get(EEPROMIDX, numRotationsSinceReset);
    setNRotations();
}

void setNRotations()
{

    uint8_t data[2];
    TIMSK1 &= ~(1 << OCIE1A);
    I2C.read(ENCODERADDR, ANGLE, 2, data);
    TIMSK1 |= (1 << OCIE1A);
    stepper.encoder.encoderOffset = 0; //assume the encoder is constant
    
    stepper.encoder.angle = (((uint16_t)data[0]) << 8) | (uint16_t)data[1];
    stepper.encoder.oldAngle = (((uint16_t)data[0]) << 8) | (uint16_t)data[1];

    stepper.encoder.angleMoved = ((float)((((uint16_t)data[0]) << 8) | (uint16_t)data[1]) - ((360.0f / 0.087890625) * (numRotationsSinceReset+1)));
    stepper.stepsSinceReset = 0;
}

void loop()
{
    // put your main code here, to run repeatedly:
    //while(stepper.getMotorState()!=0);
    //stepper.stop(true);
    if (Serial.available())
    {

        String readIn = Serial.readStringUntil('\n');

        //Serial.println(readIn);
        if (readIn.substring(0, 1) == "V")
        {
            Serial.println("1");
            Serial.flush();
        }
        else if (readIn.substring(0, 1) == "T")
        {
            Serial.println(String(oldValue));
            Serial.flush();
        }
        else if (readIn.substring(0, 1) == "A")
        {
            Serial.println(String(stepper.encoder.getAngleMoved()));
            Serial.flush();
        }
        else if (readIn.substring(0, 1) == "B")
        {
            Serial.println(String(stepper.encoder.getAngle()));
            Serial.flush();
        }
        else if (readIn.substring(0, 1) == "H")
        {
            //stepper.encoder.setHome();
            EEPROM.put(EEPROMIDX, (uint16_t)0);
            numRotationsSinceReset = 0;
            setNRotations();
            Serial.println("Home set");
        }
        else if (readIn.substring(0, 1) == "E")
        {
            //stepper.encoder.setHome();
            uint16_t v;
            EEPROM.get(EEPROMIDX, v);
            Serial.println(String(v));
        }
        float diff = 0;
        oldValue = -1 * stepper.encoder.getAngleMoved() / DEGREE_PER_UA;
        if (readIn.substring(0, 1) == "S")
        { // set the value absolute
            float newValue = readIn.substring(1).toFloat();
            diff = (newValue - oldValue);
        }
        else if (readIn.substring(0, 1) == "D")
        { // set the value relative (set diff)ƒ
            float newValue = readIn.substring(1).toFloat();
            diff = newValue;
        }
        //oldValue = stepper.encoder.getAngle();
        if ((readIn.substring(0, 1) == "D") || (readIn.substring(0, 1) == "S"))
        {
            if (oldValue + diff < 0)
            {
                diff = -oldValue; // cannot set below 0
                Serial.println("Requested < 0");
            }

            if (abs(diff * DEGREE_PER_UA * stepper.stepConversion) > 1)
            {
                Serial.println("Diff:" + String(diff));

                stepper.moveAngle(diff * DEGREE_PER_UA, SOFT);

                oldValue = oldValue + diff;
            }
        }
    }
    uint16_t newRotations = (uint16_t)ceil(-round(stepper.encoder.getAngleMoved()) / 360);
    if (numRotationsSinceReset != newRotations)
    {
        //Serial.println("updating EEPROM");
        EEPROM.put(EEPROMIDX, newRotations);
        numRotationsSinceReset = newRotations;
    }
}
