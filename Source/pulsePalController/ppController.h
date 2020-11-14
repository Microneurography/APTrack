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

	void resized() override;

	void setStimulusVoltage(float newVoltage);

private:

	String formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration);

	// TODO: Maybe move to inside timer?

	//ScopedPointer<Label> msLabel;
	ScopedPointer<TextEditor> protocolStepNumber_label;
	ScopedPointer<TextEditor> protocolRate_label;
	ScopedPointer<TextEditor> protocolDuration_label;
	ScopedPointer<TextEditor> protocolTimeLeft_label;
	ScopedPointer<TextEditor> protocolVoltage_label;
	ScopedPointer<TextEditor> protocolComment_label;

	ScopedPointer<UtilityButton> getFileButton;

	ScopedPointer<TextEditor> fileName_label;

	std::vector<protocolDataElement> protocolData;

	float stimulusVoltage;

	float param_mAperVolts;

	void timerCallback(int timerID) override;

	File lastFilePath;

	void loadFile(String file);//, std::vector<protocolDataElement> data);

	int protocolStepNumber;

	int elementCount;

	int64 endingTime;

	RelativeTime secondsLeft;

	// PulsePal Specific
	PulsePal pulsePal;
	uint32_t pulsePalVersion;

	void sendProtocolStepToPulsePal(protocolDataElement protocol);

};