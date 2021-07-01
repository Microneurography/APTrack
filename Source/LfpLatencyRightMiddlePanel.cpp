// LfpLatencyRightMiddlePanel.cpp

#include "LfpLatencyRightMiddlePanel.h"

#include "LfpLatencyElements.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

LfpLatencyRightMiddlePanel::LfpLatencyRightMiddlePanel(LfpLatencyProcessorVisualizerContentComponent* content)
{
    ROISpikeLatency = new LfpLatencyLabelTextEditor("ROI Spike Location");
    ROISpikeMagnitude = new LfpLatencyLabelTextEditor("ROI Spike Value");
    triggerThreshold = new LfpLatencyLabelSlider("Trigger Threshold");

    triggerThreshold->setSliderRange(0.1, 150);
    triggerThreshold->addSliderListener(content);
    triggerThreshold->setSliderValue(2.5);

    addAndMakeVisible(ROISpikeLatency);
    addAndMakeVisible(ROISpikeMagnitude);
    addAndMakeVisible(triggerThreshold);
}

void LfpLatencyRightMiddlePanel::resized()
{
    auto area = getLocalBounds();
    auto itemHeight = 24;
    auto itemVerticalSpace = 5;

    area.removeFromTop(itemVerticalSpace);
    ROISpikeLatency->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    ROISpikeMagnitude->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);

    auto triggerThresholdHeight = 64;
    triggerThreshold->setBounds(area.removeFromTop(triggerThresholdHeight));
}

void LfpLatencyRightMiddlePanel::setROISpikeLatencyText(const String& newText)
{
    ROISpikeLatency->setTextEditorText(newText + " ms");
}

void LfpLatencyRightMiddlePanel::setROISpikeMagnitudeText(const String& newText)
{
    ROISpikeMagnitude->setTextEditorText(newText + " uV");
}

double LfpLatencyRightMiddlePanel::getTriggerThresholdValue() const
{
    return triggerThreshold->getSliderValue();
}
