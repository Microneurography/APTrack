#pragma once
#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"
#include "../LfpLatencyProcessor.h"

#define TIMER_UI 0
#define TIMER_PROTOCOL 1
struct protocolDataElement {
	float duration;
	float rate;
	float voltage;
	String comment;
};


class ppController : public Component,
	public ButtonListener,
	public MultiTimer
{
public:

	ppController();



	~ppController();

	void buttonClicked(Button* buttonThatWasClicked) override;

	void paint(Graphics& g) override;

	void paintOverChildren(Graphics& g) override;

	void resized() override;

	void setStimulusVoltage(float newVoltage);
	void setProcessor(LfpLatencyProcessor* processor);
private:

	int protocolStepNumber;

	int elementCount;

	float stimulusVoltage;

	float protocolDuration;

	int64 protocolStepEndingTime;

	int64 protocolEndingTime;

	std::vector<protocolDataElement> protocolData;
	std::string protocolName = "";

	String formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration);

	ScopedPointer<TextEditor> protocolStepSummary_text;
	ScopedPointer<Label> protocolStepSummary_label;

	ScopedPointer<TextEditor> protocolTimeLeft_text;
	ScopedPointer<Label> protocolTimeLeft_label;

	ScopedPointer<TextEditor> protocolStepTimeLeft_text;
	ScopedPointer<Label> protocolStepTimeLeft_label;

	ScopedPointer<TextEditor> protocolStepComment_text;
	ScopedPointer<Label> protocolStepComment_label;

	ScopedPointer<UtilityButton> getFileButton;
	ScopedPointer<UtilityButton> startStopButton;
	ScopedPointer<TextEditor> fileName_text;
	LfpLatencyProcessor * processor;
	File lastFilePath;

	void loadFile(String file);//, std::vector<protocolDataElement> data);

	void timerCallback(int timerID) override;

	PulsePal pulsePal;
	uint32_t pulsePalVersion;

	bool pulsePalConnected;

	float RELAY_TTL_delay_s = 0.0005f; // The delay in seconds between 
	void StopCurrentProtocol();
	void StartCurrentProtocol();

	void sendProtocolStepToPulsePal(protocolDataElement protocol);

};