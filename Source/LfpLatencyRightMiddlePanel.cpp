// LfpLatencyRightMiddlePanel.cpp

#include "LfpLatencyRightMiddlePanel.h"

#include "LfpLatencyElements.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

LfpLatencyRightMiddlePanel::LfpLatencyRightMiddlePanel(LfpLatencyProcessorVisualizerContentComponent* content)
{
    ROISpikeLatency = new LfpLatencyLabelTextEditor("ROI Spike Location");
    ROISpikeValue = new LfpLatencyLabelTextEditor("ROI Spike Value");
    triggerThreshold = new LfpLatencyLabelSlider("Trigger Threshold");

    triggerThreshold->setSliderRange(0.1, 150);
    triggerThreshold->addSliderListener(content);
    triggerThreshold->setSliderValue(2.5);

    addAndMakeVisible(ROISpikeLatency);
    addAndMakeVisible(ROISpikeValue);
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
    ROISpikeValue->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);

    auto triggerThresholdHeight = 64;
    triggerThreshold->setBounds(area.removeFromTop(triggerThresholdHeight));
}

void LfpLatencyRightMiddlePanel::setROISpikeLatencyText(const String& newText)
{
    ROISpikeLatency->setTextEditorText(newText + " ms");
}

void LfpLatencyRightMiddlePanel::setROISpikeValueText(const String& newText)
{
    ROISpikeValue->setTextEditorText(newText + " uV");
}

double LfpLatencyRightMiddlePanel::getTriggerThresholdValue() const
{
    return triggerThreshold->getSliderValue();
}
