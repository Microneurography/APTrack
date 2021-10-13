#pragma once
#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"
#include "../LfpLatencyProcessor.h"

#define TIMER_UI 0
#define TIMER_PROTOCOL 1
using namespace juce;


class ppControllerVisualizer : public Component,
							   public ButtonListener,
							   public MultiTimer
{
public:
	ppControllerVisualizer(LfpLatencyProcessor* processor);

	~ppControllerVisualizer();

	void buttonClicked(Button *buttonThatWasClicked) override;

	void paint(Graphics &g) override;

	void paintOverChildren(Graphics &g) override;

	void resized() override;


private:
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
	LfpLatencyProcessor *processor;
	File lastFilePath;

	void timerCallback(int timerID) override;

	bool pulsePalConnected;
};