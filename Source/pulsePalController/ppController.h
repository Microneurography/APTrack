#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"


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

private:

	int protocolStepNumber;

	int elementCount;

	float stimulusVoltage;

	float protocolDuration;

	int64 protocolStepEndingTime;

	int64 protocolEndingTime;

	std::vector<protocolDataElement> protocolData;

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
	ScopedPointer<TextEditor> fileName_text;

	File lastFilePath;

	void loadFile(String file);//, std::vector<protocolDataElement> data);

	void timerCallback(int timerID) override;

	PulsePal pulsePal;
	uint32_t pulsePalVersion;

	bool pulsePalConnected;


	void StopCurrentProtocol();

	void sendProtocolStepToPulsePal(protocolDataElement protocol);

};