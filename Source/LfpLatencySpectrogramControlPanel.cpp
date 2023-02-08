// LfpLatencySpectrogramControlPanel.cpp
/*
    ------------------------------------------------------------------

    This file is part of APTrack, a plugin for the Open-Ephys Gui

    Copyright (C) 2019-2023 Eli Lilly and Company, University of Bristol, Open Ephys
    Authors: Aidan Nickerson, Grace Stangroome, Merle Zhang, James O'Sullivan, Manuel Martinez

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "LfpLatencySpectrogramControlPanel.h"

ScopedPointer<LfpLatencyLabelHorizontalSlider> LfpLatencySpectrogramControlPanel::imageThreshold;      // = new LfpLatencyLabelVerticalSlider("Image Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::highImageThreshold;        // = new LfpLatencyLabelTextEditor("High Image Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::detectionThreshold;        // = new LfpLatencyLabelTextEditor("Detection Threshold")
ScopedPointer<LfpLatencyLabelTextEditor> LfpLatencySpectrogramControlPanel::lowImageThreshold;         // = new LfpLatencyLabelTextEditor("Low Image Threshold")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::subsamplesPerWindowSlider;     // = new LfpLatencyLabelSlider("Subsamples Per Window")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::startingSampleSlider;          // = new LfpLatencyLabelSlider("Starting Sample")
ScopedPointer<LfpLatencyLabelSlider> LfpLatencySpectrogramControlPanel::conductionDistanceSlider;      // = new LfpLatencyLabelSlider("Conduction Distance")
ScopedPointer<LfpLatencyLabelSliderNoTextBox> LfpLatencySpectrogramControlPanel::searchBoxWidthSlider; // = new LfpLatencyLabelSlider("Conduction Distance")

LfpLatencySpectrogramControlPanel::LfpLatencySpectrogramControlPanel(LfpLatencyProcessorVisualizerContentComponent *content)
{
    outline = new GroupComponent("Color control");

    // #TODO: convert to horizontal
    imageThreshold = new LfpLatencyLabelHorizontalSlider("Image Threshold");
    if (content->getExtendedColorScale())
    {
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

    subsamplesPerWindowSlider = new LfpLatencyLabelSlider("Subsamples Per Window");
    int maxSubsample = std::round(DATA_CACHE_SIZE_SAMPLES / SPECTROGRAM_HEIGHT); // TODO: remove constants
    subsamplesPerWindowSlider->setSliderRange(1, maxSubsample, 1);
    subsamplesPerWindowSlider->addSliderListener(content);
    subsamplesPerWindowSlider->setSliderValue(content->getSubsamplesPerWindow()); // TODO: not sure we need this for initialisation

    startingSampleSlider = new LfpLatencyLabelSlider("Starting Sample");
    startingSampleSlider->setSliderRange(0, 30000, 1);
    startingSampleSlider->addSliderListener(content);
    startingSampleSlider->setSliderValue(content->getStartingSample());

    // TODO: conduction distance not used?
    conductionDistanceSlider = new LfpLatencyLabelSlider("Conduction Distance");
    conductionDistanceSlider->setSliderRange(0, 2000, 1);
    conductionDistanceSlider->addSliderListener(content);

    this->searchBoxWidthSlider = new LfpLatencyLabelSliderNoTextBox("Search Box Width");
    this->searchBoxWidthSlider->setSliderRange(1, 1000, 10);
    this->searchBoxWidthSlider->addSliderListener(content);
    this->searchBoxWidthSlider->setSliderValue(3);

    addAndMakeVisible(outline);
    addAndMakeVisible(imageThreshold);
    addAndMakeVisible(highImageThreshold);
    addAndMakeVisible(detectionThreshold);
    addAndMakeVisible(lowImageThreshold);
    addAndMakeVisible(subsamplesPerWindowSlider);
    addAndMakeVisible(startingSampleSlider);
    addAndMakeVisible(searchBoxWidthSlider);
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

    auto sliderWidth = getWidth() / 3;
    imageThreshold->setBounds(area.removeFromRight(sliderWidth));

    auto textItemHeight = area.getHeight() * 0.1;
    // highImageThreshold->setBounds(area.removeFromTop(textItemHeight));
    // detectionThreshold->setBounds(area.removeFromTop(textItemHeight));
    // lowImageThreshold->setBounds(area.removeFromTop(textItemHeight));

    auto sliderItemHeight = area.getWidth() / 3.0;
    subsamplesPerWindowSlider->setBounds(area.removeFromLeft(sliderItemHeight));
    startingSampleSlider->setBounds(area.removeFromLeft(sliderItemHeight));
    this->searchBoxWidthSlider->setBounds(area.removeFromLeft(sliderItemHeight));
}

void LfpLatencySpectrogramControlPanel::setImageThresholdRange(double newMinimum, double newMaximum, double newInterval)
{
    imageThreshold->setSliderRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencySpectrogramControlPanel::setHighImageThresholdText(const String &newText)
{
    highImageThreshold->setTextEditorText(newText);
}

void LfpLatencySpectrogramControlPanel::setDetectionThresholdText(const String &newText)
{
    detectionThreshold->setTextEditorText(newText);
}

void LfpLatencySpectrogramControlPanel::setLowImageThresholdText(const String &newText)
{
    lowImageThreshold->setTextEditorText(newText);
}

void LfpLatencySpectrogramControlPanel::setStartingSampleValue(double newValue)
{
    startingSampleSlider->setSliderValue(newValue);
}

void LfpLatencySpectrogramControlPanel::setSubsamplesPerWindowValue(double newValue)
{
    subsamplesPerWindowSlider->setSliderValue(newValue);
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowValue() const
{
    return subsamplesPerWindowSlider->getSliderValue();
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowMaximum() const
{
    return subsamplesPerWindowSlider->getSliderMaximum();
}

double LfpLatencySpectrogramControlPanel::getSubsamplesPerWindowMinimum() const
{
    return subsamplesPerWindowSlider->getSliderMinimum();
}

void LfpLatencySpectrogramControlPanel::changeSubsamplesPerWindowValue(double deltaValue)
{
    setSubsamplesPerWindowValue(getSubsamplesPerWindowValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getStartingSampleValue() const
{
    return startingSampleSlider->getSliderValue();
}

double LfpLatencySpectrogramControlPanel::getStartingSampleMaximum() const
{
    return startingSampleSlider->getSliderMaximum();
}

double LfpLatencySpectrogramControlPanel::getStartingSampleMinimum() const
{
    return startingSampleSlider->getSliderMinimum();
}

void LfpLatencySpectrogramControlPanel::changeStartingSampleValue(double deltaValue)
{
    setStartingSampleValue(getStartingSampleValue() + deltaValue);
}

double LfpLatencySpectrogramControlPanel::getImageThresholdMaxValue() const
{
    return imageThreshold->getSliderMaxValue();
}
void LfpLatencySpectrogramControlPanel::setDetectionThresholdValue(double newValue)
{
    if (newValue > imageThreshold->getSliderMaxValue())
    {
        imageThreshold->setSliderMaxValue(newValue);
    }
    if (newValue < imageThreshold->getSliderMinValue())
    {
        imageThreshold->setSliderMinValue(newValue);
    }
    imageThreshold->setSliderValue(newValue);
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
void LfpLatencySpectrogramControlPanel::setSearchBoxWidthValue(double newValue)
{
    searchBoxWidthSlider->setSliderValue(newValue);
}

void LfpLatencySpectrogramControlPanel::loadParameters(const std::map<String, String> &newParameters)
{
    imageThreshold->setSliderValue(newParameters.at("Image Threshold").getDoubleValue());
    subsamplesPerWindowSlider->setSliderValue(newParameters.at("Subsamples Per Window").getDoubleValue());
    startingSampleSlider->setSliderValue(newParameters.at("Starting Sample").getDoubleValue());
    conductionDistanceSlider->setSliderValue(newParameters.at("Conduction Distance").getDoubleValue());
    searchBoxWidthSlider->setSliderValue(newParameters.at("Searchbox width").getDoubleValue());
}
