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
#include "SpikeGroupTableContent.h"

int SpikeGroupTableContent::getNumRows()
{
    return this->processor->getSpikeGroupCount();
}

SpikeGroupTableContent::SpikeGroupTableContent(LfpLatencyProcessor *processor)
{
    this->processor = processor;
}

SpikeGroupTableContent::~SpikeGroupTableContent()
{
}
// void SpikeGroupTableContent::ButtonListener::buttonClicked (Button* button){};
void SpikeGroupTableContent::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{

    g.setColour(Colours::black); // [5]
    Font font = 12.0f;
    g.setFont(font);

    if (columnId == Columns::spike_id_info)
    {
        auto text = std::to_string(rowNumber + 1); // #TODO this should get ID from spikeGroup

        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true); // [6]
    }

    g.setColour(Colours::transparentWhite);
    g.fillRect(width - 1, 0, 1, height);
}

void SpikeGroupTableContent::paintRowBackground(Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{

    if (rowIsSelected)
    {
        g.fillAll(colorWheel[rowNumber % 4]);
    }
    else
    {
        g.fillAll(Colours::lightgrey);
    }
}

Component *SpikeGroupTableContent::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component *existingComponentToUpdate)
{
    if (rowNumber < getNumRows())
    {
        auto spikeGroup = processor->getSpikeGroup(rowNumber);
        ;
        if (columnId == Columns::delete_button)
        {
            auto *deleteButton = static_cast<DeleteComponent *>(existingComponentToUpdate);

            if (deleteButton == nullptr)
            {
                deleteButton = new DeleteComponent(*this);
            }
            return deleteButton;
        }
        if (columnId == Columns::track_spike_button)
        {
            auto *selectionBox = static_cast<SelectableColumnComponent *>(existingComponentToUpdate);

            if (selectionBox == nullptr)
            {
                selectionBox = new SelectableColumnComponent(*this, rowNumber, SpikeGroupTableContent::SelectableColumnComponent::Action::ACTIVATE_SPIKE, processor);
            }
            selectionBox->setSpikeID(rowNumber);
            selectionBox->setToggleState(spikeGroup->isActive, juce::NotificationType::dontSendNotification);
            return selectionBox;
        }
        if (columnId == Columns::threshold_spike_button)
        {
            auto *selectionBox = static_cast<SelectableColumnComponent *>(existingComponentToUpdate);
            if (selectionBox == nullptr)
            {
                selectionBox = new SelectableColumnComponent(*this, rowNumber, SpikeGroupTableContent::SelectableColumnComponent::Action::TRACK_SPIKE, processor);
            }
            selectionBox->setSpikeID(rowNumber);
            selectionBox->setToggleState(spikeGroup->isTracking, juce::NotificationType::dontSendNotification);
            return selectionBox;
        }
        if (columnId == Columns::location_info)
        {
            auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponentToUpdate);

            if (label == nullptr)
            {
                label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
            }
            std::ostringstream ss_ms_latency;

            ss_ms_latency << std::fixed << std::setprecision(2) << (spikeGroup->templateSpike.spikeSampleLatency * 1000) / this->processor->getSampleRate(0);
            label->setText(ss_ms_latency.str() + "ms", juce::NotificationType::dontSendNotification);
            label->repaint();
            return label;
        }
        if (columnId == Columns::firing_probability_info)
        {
            auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponentToUpdate);

            if (label == nullptr)
            {
                label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
            }
            label->setText(std::to_string(100 * std::count(spikeGroup->recentHistory.begin(), spikeGroup->recentHistory.end(), true) / spikeGroup->recentHistory.size()), juce::NotificationType::dontSendNotification);
            label->repaint();

            return label;
        }
        if (columnId == Columns::threshold_info)
        {
            auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponentToUpdate);

            if (label == nullptr)
            {
                label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
            }
            std::ostringstream ss_threshold;
            ss_threshold << std::fixed << std::setprecision(2) << spikeGroup->templateSpike.threshold;
            label->setText(ss_threshold.str(), juce::NotificationType::dontSendNotification);
            label->repaint();

            return label;
        }
        if (columnId == Columns::pct50stimulus)
        {
            auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponentToUpdate);

            if (label == nullptr)
            {
                label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
            }
            if (spikeGroup->stimulusVoltage50pct != -1)
            {
                std::ostringstream ss_threshold;
                ss_threshold << std::fixed << std::setprecision(2) << spikeGroup->stimulusVoltage50pct << " mA";
                label->setText(ss_threshold.str(), juce::NotificationType::dontSendNotification);
            }
            else
            {
                label->setText("...", juce::NotificationType::dontSendNotification);
            }
            label->repaint();
            return label;
        }
    }
    // jassert(existingComponentToUpdate == nullptr);
    return nullptr;
}

void SpikeGroupTableContent::buttonClicked(juce::Button *button)
{
    button->getToggleState();
}
SpikeGroupTableContent::UpdatingTextColumnComponent::UpdatingTextColumnComponent(SpikeGroupTableContent &tcon, int rowNumber, int columnNumber) : owner(tcon)
{
    addAndMakeVisible(value = new juce::Label("_"));
}

SpikeGroupTableContent::UpdatingTextColumnComponent::~UpdatingTextColumnComponent()
{
    value = nullptr;
}

SpikeGroupTableContent::SelectableColumnComponent::SelectableColumnComponent(SpikeGroupTableContent &tcon, int spikeID, Action action, LfpLatencyProcessor *processor) : owner(tcon), spikeID(spikeID), action(action)
{
    this->processor = processor;
    addAndMakeVisible(toggleButton = new ToggleButton);
    this->addListener(this);
    // toggleButton->addListener(this);
}
void SpikeGroupTableContent::SelectableColumnComponent::buttonClicked(juce::Button *b)
{
    auto newSpikeID = -1;
    if (b->getToggleState())
        newSpikeID = spikeID;

    if (action == Action::ACTIVATE_SPIKE)
        processor->setSelectedSpike(newSpikeID);
    else if (action == Action::TRACK_SPIKE)
        processor->setTrackingSpike(newSpikeID);
}
void SpikeGroupTableContent::SelectableColumnComponent::setSpikeID(int spikeID)
{
    this->spikeID = spikeID;
}
SpikeGroupTableContent::SelectableColumnComponent::~SelectableColumnComponent()
{
    toggleButton = nullptr;
}

SpikeGroupTableContent::DeleteComponent::DeleteComponent(SpikeGroupTableContent &tcon) : owner(tcon)
{
    addAndMakeVisible(del = new TextButton);
    del->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
}

SpikeGroupTableContent::DeleteComponent::~DeleteComponent()
{
    del = nullptr;
}
// void SpikeGroupTableContent::DeleteComponent::buttonClicked(juce::Button *b){

// 	processor->removeSpikeGroup(spikeID);
// }