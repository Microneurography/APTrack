#pragma once
#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"
#include "../LfpLatencyProcessor.h"
#include "serial/UStepper.h"

#define TIMER_PROTOCOL 0

class LfpLatencyProcessor;
struct protocolDataElement {
	float duration;
	float rate;
	float voltage;
	String comment;
};


class ppController :
	public MultiTimer
{
friend class ppControllerVisualizer;
public:

	ppController(LfpLatencyProcessor *processor);

	~ppController();

	void setStimulusVoltage(float newVoltage);
	float getStimulusVoltage();
	void setMinStimulusVoltage(float newMinVoltage);
	float getMinStimulusVoltage();
	void setMaxStimulusVoltage(float newMaxVoltage);
	float getMaxStimulusVoltage();
	
	bool initializeConnection(); // returns false if connection fails
	bool isProtocolRunning();
private:

	bool protocolRunning;
	int protocolStepNumber;

	int elementCount;

	float stimulusVoltage;
	float minStimulusVoltage = 0;
	float maxStimulusVoltage = 10;

	float protocolDuration;

	int64 protocolStepEndingTime;

	int64 protocolEndingTime;

	std::vector<protocolDataElement> protocolData;
	std::string protocolName = "";

	String formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration);

	LfpLatencyProcessor * processor;

	void loadFile(String file);//, std::vector<protocolDataElement> data);

	void timerCallback(int timerID) override;

	PulsePal* pulsePal;
	uint32_t pulsePalVersion;

	bool pulsePalConnected;

	float RELAY_TTL_delay_s = 0.0005f; // The delay in seconds between 
	void StopCurrentProtocol();
	void StartCurrentProtocol();

	void sendProtocolStepToPulsePal(protocolDataElement protocol);

	std::unique_ptr<UStepper> ustepper;

};