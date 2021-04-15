// LfpLatencySpectrogramControlPanel.cpp

#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencySpectrogramControlPanel.h"
#include "LfpLatencyElements.h"

LfpLatencySpectrogramControlPanel::LfpLatencySpectrogramControlPanel(LfpLatencyProcessorVisualizerContentComponent* content)
{
    outline = new GroupComponent("Color control");

    imageThreshold = new LfpLatencyLabelVerticalSlider("Image Threshold");
    imageThreshold->setSliderRange(0, 100, 0);
    imageThreshold->addSliderListener(content);
    imageThreshold->setSliderMinValue(0);
    imageThreshold->setSliderMaxValue(90);
    imageThreshold->setSliderValue(50);
    
    highImageThreshold = new LfpLatencyLabelTextEditor("High Image Threshold");
    highImageThreshold->setTextEditorText(String(imageThreshold->getSliderMaximum()) + " uV");
    detectionThreshold = new LfpLatencyLabelTextEditor("Detection Threshold");
    detectionThreshold->setTextEditorText(String(imageThreshold->getSliderValue()) + " uV");
    lowImageThreshold = new LfpLatencyLabelTextEditor("Low Image Threshold");
    lowImageThreshold->setTextEditorText(String(imageThreshold->getSliderMinimum()) + " uV");
    
    subsamplesPerWindow = new LfpLatencyLabelSlider("Subsamples Per Window");
    int maxSubsample = std::round(DATA_CACHE_SIZE_SAMPLES / SPECTROGRAM_HEIGHT); // TODO: remove constants
    subsamplesPerWindow->setSliderRange(1, maxSubsample, 1);
    subsamplesPerWindow->addSliderListener(content);
    subsamplesPerWindow->setSliderValue(1); // TODO: not sure we need this for initialisation

    startingSample = new LfpLatencyLabelSlider("Starting Sample");
    startingSample->setSliderRange(0, 30000, 1);
    startingSample->addSliderListener(content);
    startingSample->setSliderValue(0); // TODO: not sure we need this for initialisation

    // TODO: conduction distance not used?
    conductionDistance = new LfpLatencyLabelSlider("Conduction Distance");
    conductionDistance->setSliderRange(0, 2000, 1);
    conductionDistance->addSliderListener(content);

    addAndMakeVisible(outline);
    addAndMakeVisible(imageThreshold);
    addAndMakeVisible(highImageThreshold);
    addAndMakeVisible(detectionThreshold);
    addAndMakeVisible(lowImageThreshold);
    addAndMakeVisible(subsamplesPerWindow);
    addAndMakeVisible(startingSample);
    addAndMakeVisible(conductionDistance);
}

void LfpLatencySpectrogramControlPanel::resized()
{
    // https://docs.juce.com/master/tutorial_rectangle_advanced.html
    auto area = getLocalBounds();
    outline->setBounds(area);

    // Remove outline's border width
    auto borderWidth = 20;
    area.removeFromTop(borderWidth);
    area.removeFromBottom(borderWidth);
    area.removeFromLeft(borderWidth);
    area.removeFromRight(borderWidth);

    auto sliderWidth = getWidth()/3;
    imageThreshold->setBounds(area.removeFromRight(sliderWidth));

    auto textItemHeight = 24;
    highImageThreshold->setBounds(area.removeFromTop(textItemHeight));
    detectionThreshold->setBounds(area.removeFromTop(textItemHeight));
    lowImageThreshold->setBounds(area.removeFromTop(textItemHeight));

    auto sliderItemHeight = 64;
    subsamplesPerWindow->setBounds(area.removeFromTop(sliderItemHeight));
    startingSample->setBounds(area.removeFromTop(sliderItemHeight));
    conductionDistance->setBounds(area.removeFromTop(sliderItemHeight));
}

bool LfpLatencySpectrogramControlPanel::keyPressed(const KeyPress& k) {
    auto subsamplesPerWindowValue = subsamplesPerWindow->getSliderValue();
    //Increase subsamplesperwindow
    if ((k.getTextCharacter() == '=' || k.getTextCharacter() == '+' || k == KeyPress::numberPadAdd) && (subsamplesPerWindowValue < subsamplesPerWindow->getSliderMaximum())) {
        subsamplesPerWindow->setSliderValue(subsamplesPerWindowValue + 5);
        return true;
    }
    //Decrease subsamplesperwindow
    else if ((k.getTextCharacter() == '-' || k == KeyPress::numberPadSubtract) && (subsamplesPerWindowValue > subsamplesPerWindow->getSliderMinimum())) {
        subsamplesPerWindow->setSliderValue(subsamplesPerWindowValue - 5);
        return true;
    }

    auto startingSampleValue = startingSample->getSliderValue();
    //Increase starting sample
    if ((k == KeyPress::upKey || k == KeyPress::numberPad8) && (startingSampleValue < startingSample->getSliderMaximum())) {
        startingSample->setSliderValue(startingSample->getSliderValue() + 100);
        return true;
    }
    //Decrease starting sample
    else if ((k == KeyPress::downKey || k == KeyPress::numberPad2) && (startingSampleValue > startingSample->getSliderMinimum())) {
        startingSample->setSliderValue(startingSample->getSliderValue() - 100);
        return true;
    }

    auto highImageThreshold = imageThreshold->getSliderMaxValue();
    auto lowImageThreshold = imageThreshold->getSliderMinValue();
    //Increase highImageThreshold
    if ((k == KeyPress::pageUpKey || k == KeyPress::numberPad9) && (highImageThreshold < imageThreshold->getSliderMaximum())) {
        imageThreshold->setSliderMaxValue(highImageThreshold + 2);
        return true;
    }
    //Decrease highImageThreshold
    else if ((k == KeyPress::pageDownKey || k == KeyPress::numberPad3) && (highImageThreshold > imageThreshold->getSliderMinimum())) {
        imageThreshold->setSliderMaxValue(highImageThreshold - 2);
        return true;
    }
    //Increase lowImageThreshold
    else if ((k == KeyPress::homeKey || k == KeyPress::numberPad7) && (lowImageThreshold < imageThreshold->getSliderMaximum())) {
        imageThreshold->setSliderMinValue(lowImageThreshold + 2);
        return true;
    }
    //Decrease lowImageThreshold
    else if ((k == KeyPress::endKey || k == KeyPress::numberPad1) && (lowImageThreshold > imageThreshold->getSliderMinimum())) {
        imageThreshold->setSliderMinValue(lowImageThreshold - 2);
        return true;
    }
    
    return false;
}

void LfpLatencySpectrogramControlPanel::setImageThresholdRange(double newMinimum, double newMaximum, double newInterval) {
    imageThreshold->setSliderRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencySpectrogramControlPanel::setHighImageThresholdText(const String& newText) {
    highImageThreshold->setTextEditorText(newText);
}

void LfpLatencySpectrogramControlPanel::setDetectionThresholdText(const String& newText) {
    detectionThreshold->setTextEditorText(newText);
}

void LfpLatencySpectrogramControlPanel::setLowImageThresholdText(const String& newText) {
    lowImageThreshold->setTextEditorText(newText);
}
