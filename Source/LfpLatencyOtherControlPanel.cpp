// LfpLatencyOtherControlPanel.cpp
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
#include "LfpLatencyOtherControlPanel.h"

LfpLatencyOtherControlPanel::LfpLatencyOtherControlPanel(LfpLatencyProcessorVisualizerContentComponent *content)
{
    outline = new GroupComponent("Other control");
    options = new TextButton("Options");
    // setup = new TextButton("Setup");
    // colorStyle = new LfpLatencyLabelComboBox("Color Style Combination");
    // extendedColorScale = new LfpLatencyLabelToggleButton("Extended Scale");
    // triggerChannel = new LfpLatencyLabelComboBox("Trigger Channel");
    // dataChannel = new LfpLatencyLabelComboBox("Data Channel");
    // trackSpike = new LfpLatencyLabelToggleButton("Track Spike");
    // trackThreshold = new LfpLatencyLabelToggleButton("Track Threshold");

    options->addListener(content);
    options->setColour(TextButton::ColourIds::buttonColourId, Colours::lightgrey);

    // setup->addListener(content);
    // setup->setColour(TextButton::ColourIds::buttonColourId, Colours::lightgrey);

    // colorStyle->addComboBoxItem("WHOT", 1);
    // colorStyle->addComboBoxItem("BHOT", 2);
    // colorStyle->addComboBoxItem("WHOT,PLAIN", 3);
    // colorStyle->addComboBoxItem("BHOT,PLAIN", 4);
    // colorStyle->setComboBoxSelectedId(1);

    // extendedColorScale->addToggleButtonListener(content);

    // triggerChannel->setComboBoxTextWhenNothingSelected("None");
    // triggerChannel->addComboBoxSectionHeading("Trigger");

    // dataChannel->setComboBoxTextWhenNothingSelected("None");
    // dataChannel->addComboBoxSectionHeading("Data");

    // trackSpike->addToggleButtonListener(content);

    // trackThreshold->addToggleButtonListener(content);
    // trackThreshold->setEnabled(getTrackSpikeToggleState());

    addAndMakeVisible(outline);
    addAndMakeVisible(options);
    addAndMakeVisible(setup);
    // addAndMakeVisible(colorStyle);
    // addAndMakeVisible(extendedColorScale);
    // addAndMakeVisible(triggerChannel);
    // addAndMakeVisible(dataChannel);
    // addAndMakeVisible(trackSpike);
    // addAndMakeVisible(trackThreshold);
}

void LfpLatencyOtherControlPanel::resized()
{
    auto area = getLocalBounds();
    outline->setBounds(area);

    auto borderWidth = 20;
    area = area.withSizeKeepingCentre(getWidth() - 2 * borderWidth, getHeight() - 2 * borderWidth);

    auto buttonHeight = 24;
    auto buttonWidth = 120;
    auto buttonArea = area.removeFromTop(buttonHeight);
    auto optionsArea = buttonArea.withSizeKeepingCentre(buttonWidth, buttonHeight);
    options->setBounds(optionsArea);
    // auto setupArea = buttonArea.withSizeKeepingCentre(buttonWidth, buttonHeight);
    // setup->setBounds(setupArea);

    /*
    auto rightSideArea = area.removeFromRight(area.getWidth() / 2);

    // Left
    auto itemHeight = 24;
    auto itemVerticalSpace = 5;
    colorStyle->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    extendedColorScale->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    triggerChannel->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    dataChannel->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    trackSpike->setBounds(area.removeFromTop(itemHeight));
    area.removeFromTop(itemVerticalSpace);
    trackThreshold->setBounds(area.removeFromTop(itemHeight));
    */
}

Rectangle<int> LfpLatencyOtherControlPanel::getOptionsBoundsInPanelParent() const
{
    return getBoundsInPanelParent(options->getBounds());
}

Rectangle<int> LfpLatencyOtherControlPanel::getSetupBoundsInPanelParent() const
{
    return getBoundsInPanelParent(setup->getBounds());
}

std::map<String, Rectangle<int>> LfpLatencyOtherControlPanel::getTableBounds() const
{
    auto area = getLocalBounds();
    auto borderWidth = 20;
    area = area.withSizeKeepingCentre(getWidth() - 2 * borderWidth, getHeight() - 2 * borderWidth);
    auto buttonHeight = 24;
    area.removeFromTop(buttonHeight);

    std::map<String, Rectangle<int>> bounds;

    auto verticalSpaceBetween = 5;
    area.removeFromTop(verticalSpaceBetween);

    bounds["spikeTracker"] = area;

    for (auto it = bounds.begin(); it != bounds.end(); it++)
    {
        it->second = getBoundsInPanelParent(it->second);
    }
    return bounds;
}

Rectangle<int> LfpLatencyOtherControlPanel::getBoundsInPanelParent(const Rectangle<int> &bounds) const
{
    auto panelBounds = getBounds();
    auto result = bounds;

    result.setX(bounds.getX() + panelBounds.getX());
    result.setY(bounds.getY() + panelBounds.getY());

    return result;
}

/*
int LfpLatencyOtherControlPanel::getColorStyleSelectedId() const
{
    return colorStyle->getComboBoxSelectedId();
}

int LfpLatencyOtherControlPanel::getTriggerChannelSelectedId() const
{
    return triggerChannel->getComboBoxSelectedId();
}

void LfpLatencyOtherControlPanel::setTriggerChannelSelectedId(int newItemId)
{
    triggerChannel->setComboBoxSelectedId(newItemId);
}

void LfpLatencyOtherControlPanel::clearTriggerChannel()
{
    triggerChannel->clearComboBox();
}

void LfpLatencyOtherControlPanel::addTriggerChannelSectionHeading(const String& headingName)
{
    triggerChannel->addComboBoxSectionHeading(headingName);
}

void LfpLatencyOtherControlPanel::addTriggerChannelItem(const String& newItemText, int newItemId)
{
    triggerChannel->addComboBoxItem(newItemText, newItemId);
}

int LfpLatencyOtherControlPanel::getTriggerChannelNumItems() const
{
    return triggerChannel->getComboBoxNumItems();
}

int LfpLatencyOtherControlPanel::getDataChannelSelectedId() const
{
    return dataChannel->getComboBoxSelectedId();
}

void LfpLatencyOtherControlPanel::setDataChannelSelectedId(int newItemId)
{
    dataChannel->setComboBoxSelectedId(newItemId);
}

void LfpLatencyOtherControlPanel::clearDataChannel()
{
    dataChannel->clearComboBox();
}

void LfpLatencyOtherControlPanel::addDataChannelSectionHeading(const String& headingName)
{
    dataChannel->addComboBoxSectionHeading(headingName);
}

void LfpLatencyOtherControlPanel::addDataChannelItem(const String& newItemText, int newItemId)
{
    dataChannel->addComboBoxItem(newItemText, newItemId);
}

int LfpLatencyOtherControlPanel::getDataChannelNumItems() const
{
    return dataChannel->getComboBoxNumItems();
}

bool LfpLatencyOtherControlPanel::getTrackSpikeToggleState() const
{
    return trackSpike->getToggleButtonState();
}

bool LfpLatencyOtherControlPanel::getTrackThresholdToggleState() const
{
    return trackThreshold->getToggleButtonState();
}

void LfpLatencyOtherControlPanel::setTrackThresholdEnabled(bool shouldBeEnabled)
{
    trackThreshold->setEnabled(shouldBeEnabled);
    if (!shouldBeEnabled)
    {
        trackThreshold->setToggleButtonState(shouldBeEnabled, sendNotification);
    }
}
*/
