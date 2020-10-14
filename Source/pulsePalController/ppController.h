#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"


struct protocolDataElement {
	float duration;
	float rate;
	float voltage;
	String comment;
};


class blinkingButton : public Component,
	public MultiTimer
{
public:

	blinkingButton();

	~blinkingButton();

	void startFlashing(float rate);

	void stopFlashing();

	void paint(Graphics& g) override;

private:
	float flashAlpha = 0.0f;

	int pulsePeriod;
	int zapPeriod;

	Colour colour{ Colours::red };

	void timerCallback(int timerID) override;
};

class ppController : public Component,
	public ButtonListener,
	public MultiTimer,
	public SliderListener
{
public:

	ppController();

	~ppController();

	void buttonClicked(Button* buttonThatWasClicked) override;

	void sliderValueChanged(Slider* sliderThatWasMoved) override;

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

	ScopedPointer<Slider> stimulusVoltageSlider;

	ScopedPointer<UtilityButton> getFileButton;

	ScopedPointer<TextEditor> fileName_label;

	ScopedPointer<blinkingButton> flashingComponentDemo;

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
};