// LfpLatencySpectrogramControlPanel.cpp

#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencySpectrogramControlPanel.h"
#include "LfpLatencyElements.h"

ScopedPointer<LfpLatencyLabelVerticalSlider> LfpLatencySpectrogramControlPanel::imageThreshold; // = new LfpLatencyLabelVerticalSlider("Image Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::highImageThreshold; // = new LfpLatencyLabelTextEditor("High Image Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::detectionThreshold; // = new LfpLatencyLabelTextEditor("Detection Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::lowImageThreshold; // = new LfpLatencyLabelTextEditor("Low Image Threshold")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::subsamplesPerWindow; // = new LfpLatencyLabelSlider("Subsamples Per Window")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::startingSample; // = new LfpLatencyLabelSlider("Starting Sample")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::conductionDistance; // = new LfpLatencyLabelSlider("Conduction Distance")


LfpLatencySpectrogramControlPanel::LfpLatencySpectrogramControlPanel(LfpLatencyProcessorVisualizerContentComponent* content)
{
    outline = new GroupComponent("Color control");

    imageThreshold = new LfpLatencyLabelVerticalSlider("Image Threshold");
    if (content->getExtendedColorScale()) {
        imageThreshold->setSliderRange(0, 1000, 0);
    }
    else
    {
        imageThreshold->setSliderRange(0, 100, 0);
    }
    imageThreshold->addSliderListener(content);
    imageThreshold->setSliderMaxValue(content->getHighImageThreshold());
    imageThreshold->setSliderValue(content->getDetectionThreshold());
    imageThreshold->setSliderMinValue(content->getLowImageThreshold());
    
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
    subsamplesPerWindow->setSliderValue(content->getSubsamplesPerWindow()); // TODO: not sure we need this for initialisation

    startingSample = new LfpLatencyLabelSlider("Starting Sample");
    startingSample->setSliderRange(0, 30000, 1);
    startingSample->addSliderListener(content);
    startingSample->setSliderValue(content->getStartingSample());

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

    auto textItemHeight = area.getHeight() * 0.1;
    highImageThreshold->setBounds(area.removeFromTop(textItemHeight));
    detectionThreshold->setBounds(area.removeFromTop(textItemHeight));
    lowImageThreshold->setBounds(area.removeFromTop(textItemHeight));

    auto sliderItemHeight = area.getHeight() / 3.0;
    subsamplesPerWindow->setBounds(area.removeFromTop(sliderItemHeight));
    startingSample->setBounds(area.removeFromTop(sliderItemHeight));
    conductionDistance->setBounds(area.removeFromTop(sliderItemHeight));
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

void LfpLatencySpectrogramControlPanel::setStartingSampleValue(double newValue)
{
    startingSample->setSliderValue(newValue);
}

void LfpLatencySpectrogramControlPanel::setSubsamplesPerWindowValue(double newValue)
{
    subsamplesPerWindow->setSliderValue(newValue);
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowValue() const
{
    return subsamplesPerWindow->getSliderValue();
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowMaximum() const
{
    return subsamplesPerWindow->getSliderMaximum();
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowMinimum() const
{
    return subsamplesPerWindow->getSliderMinimum();
}

void LfpLatencySpectrogramControlPanel::changeSubsamplesPerWindowValue(double deltaValue)
{
    setSubsamplesPerWindowValue(getSubsamplesPerWindowValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getStartingSampleValue() const
{
    return startingSample->getSliderValue();
}

double LfpLatencySpectrogramControlPanel::getStartingSampleMaximum() const
{
    return startingSample->getSliderMaximum();
}

double LfpLatencySpectrogramControlPanel::getStartingSampleMinimum() const
{
    return startingSample->getSliderMinimum();
}

void LfpLatencySpectrogramControlPanel::changeStartingSampleValue(double deltaValue)
{
    setStartingSampleValue(getStartingSampleValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getImageThresholdMaxValue() const
{
    return imageThreshold->getSliderMaxValue();
}

void LfpLatencySpectrogramControlPanel::setImageThresholdMaxValue(double newValue)
{
    imageThreshold->setSliderMaxValue(newValue);
}

void LfpLatencySpectrogramControlPanel::changeImageThresholdMaxValue(double deltaValue)
{
    setImageThresholdMaxValue(getImageThresholdMaxValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getImageThresholdMinValue() const
{
    return imageThreshold->getSliderMinValue();
}

void LfpLatencySpectrogramControlPanel::setImageThresholdMinValue(double newValue)
{
    imageThreshold->setSliderMinValue(newValue);
}

void LfpLatencySpectrogramControlPanel::changeImageThresholdMinValue(double deltaValue)
{
    setImageThresholdMinValue(getImageThresholdMinValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getImageThresholdMaximum() const
{
    return imageThreshold->getSliderMaximum();
}

double LfpLatencySpectrogramControlPanel::getImageThresholdMinimum() const
{
    return imageThreshold->getSliderMinimum();
}

void LfpLatencySpectrogramControlPanel::loadParameters(const std::map<String, String>& newParameters)
{
	imageThreshold->setSliderValue(newParameters.at("Image Threshold").getDoubleValue());
    subsamplesPerWindow->setSliderValue(newParameters.at("Subsamples Per Window").getDoubleValue());
    startingSample->setSliderValue(newParameters.at("Starting Sample").getDoubleValue());
    conductionDistance->setSliderValue(newParameters.at("Conduction Distance").getDoubleValue());
}