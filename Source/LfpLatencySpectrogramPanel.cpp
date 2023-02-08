// LfpLatencySpectrogramPanel.cpp
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

#include "LfpLatencySpectrogramPanel.h"

LfpLatencySpectrogramPanel::LfpLatencySpectrogramPanel(LfpLatencyProcessorVisualizerContentComponent *content)
    : content(*content)
{
    outline = new GroupComponent("Spectrogram");
    spectrogram = new LfpLatencySpectrogram();
    searchBox = new LfpLatencyLabelLinearVerticalSliderNoTextBox("Search Box");
    searchBoxWidth = new LfpLatencyLabelSliderNoTextBox("Search Box Width");
    searchBoxRectangle = new LfpLatencySearchBox(*content, *spectrogram);
    spikeIndicator = new Label("Spike Found");

    spikeIndicator->setText("Spike Tracked", sendNotification);
    spikeIndicator->setColour(Label::ColourIds::textColourId, Colours::grey);

    searchBox->setSliderRange(0, spectrogram->getImageHeight(), 1);
    searchBox->addSliderListener(content);
    searchBox->setSliderValue(10);

    searchBoxWidth->setSliderRange(1, 1000, 10);
    searchBoxWidth->addSliderListener(content);
    searchBoxWidth->setSliderValue(3);

    addAndMakeVisible(outline);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(searchBox);
    addAndMakeVisible(searchBoxRectangle);
    addAndMakeVisible(spikeIndicator);
}

void LfpLatencySpectrogramPanel::resized()
{
    auto area = getLocalBounds();
    outline->setBounds(area);

    auto borderWidth = 20;
    area = area.withSizeKeepingCentre(getWidth() - 2 * borderWidth, getHeight() - 2 * borderWidth);

    auto sliderHeight = 20;

    auto widthOfSearchBox = 15;
    auto searchBoxArea = area.removeFromRight(widthOfSearchBox);
    auto offset = 5;
    // searchBoxArea.setX(searchBoxArea.getX() - offset); //disabled as the 'bobble' overlays the main viewer.
    searchBoxArea.setWidth(searchBoxArea.getWidth() + offset);
    searchBoxArea.setHeight(searchBoxArea.getHeight() + 16); // #HACK! this is to keep search box in line with the margin of the scrollbar :(
    searchBoxArea.setY(searchBoxArea.getY() - 8);

    searchBox->setBounds(searchBoxArea);

    spectrogram->setBounds(area);

    // spikeIndicator->setBounds(searchBoxWidthArea.removeFromLeft(area.getWidth()));

    searchBoxRectangle->setBounds(area);
}

void LfpLatencySpectrogramPanel::paint(Graphics &g)
{
    auto area = getLocalBounds();

    auto widthOfSearchBox = 80;
    auto searchBoxArea = area.removeFromRight(widthOfSearchBox);
    searchBoxArea.setX(searchBoxArea.getX() - 25);
    searchBoxArea.setWidth(searchBoxArea.getWidth() - 78);
    searchBoxArea.setHeight(area.getHeight() - 104);
    searchBoxArea.setY(area.getY() + 20);

    g.setColour(Colours::white);
    g.fillRect(searchBoxArea);

    // auto starting_sample = (this->content.getStartingSample()/this->content.getSubsamplesPerWindow());
    // for (int y = searchBoxArea.getY(); y <= searchBoxArea.getY()+searchBoxArea.getHeight(); y += CoreServices::getGlobalSampleRate()/10/ this->content.getSubsamplesPerWindow())
    // {
    //     g.fillRect(searchBoxArea.getX(), y, 14, 2); // #TODO: this is upside down, and needs to be offset by starting sample, not sure this is possible. the scaling will alter the number of y-pixels per track.
    //     // TODO: refactor into LfpLatencySpectrogram?
    //     //g.drawText(to_string(y), searchBoxArea.getX() + 25, y, 25, 25, Justification::centredRight);
    // }
}

void LfpLatencySpectrogramPanel::updateSpectrogram(LfpLatencyProcessor &processor, const LfpLatencyProcessorVisualizerContentComponent &content)
{
    spectrogram->update(processor, content);
}

void LfpLatencySpectrogramPanel::setSearchBoxValue(double newValue)
{
    searchBox->setSliderValue(newValue);
}

double LfpLatencySpectrogramPanel::getSearchBoxValue() const
{
    return searchBox->getSliderValue();
}

void LfpLatencySpectrogramPanel::changeSearchBoxValue(double deltaValue)
{
    setSearchBoxValue(getSearchBoxValue() + deltaValue);
}

void LfpLatencySpectrogramPanel::setSearchBoxWidthValue(double newValue)
{
    searchBoxWidth->setSliderValue(newValue);
}

int LfpLatencySpectrogramPanel::getImageHeight() const
{
    return spectrogram->getImageHeight();
}

void LfpLatencySpectrogramPanel::spikeIndicatorTrue(bool spikeFound)
{
    if (spikeFound == true)
        spikeIndicator->setColour(Label::ColourIds::textColourId, Colours::green);
    else if (spikeFound == false)
        spikeIndicator->setColour(Label::ColourIds::textColourId, Colours::grey);
}

int LfpLatencySpectrogramPanel::getImageWidth() const
{
    return spectrogram->getImageWidth();
}
