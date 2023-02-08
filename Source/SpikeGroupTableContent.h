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
#ifndef __SPIKEGROUPTABLECOMPONENT_HEADER__
#define __SPIKEGROUPTABLECOMPONENT_HEADER__
#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

class LfpLatencyProcessor;

class SpikeGroupTableContent : public TableListBoxModel, public Button::Listener

{
public:
    enum Columns
    {
        spike_id_info = 1,
        location_info = 2,
        firing_probability_info = 3,
        threshold_info = 4,
        track_spike_button = 5,
        threshold_spike_button = 6,
        delete_button = 7,
        pct50stimulus = 8
    };
    const juce::Colour colorWheel[4] = {Colours::lightsteelblue, Colours::lightskyblue, Colours::darkgreen, Colours::orange};
    SpikeGroupTableContent(LfpLatencyProcessor *processor);
    ~SpikeGroupTableContent();

    /* These are all implementations of the functions defined in TableListBoxModel */

    int getNumRows() override;

    void paintRowBackground(Graphics &g, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component *refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component *existingComponetToUpdate) override;
    void buttonClicked(Button *button) override;

    /* This is a custom class used to add custom cells with toggle buttons inside them, the helper functions above help */
    class SelectableColumnComponent : public juce::ToggleButton, public juce::ToggleButton::Listener
    {
    public:
        enum Action
        {
            TRACK_SPIKE,
            ACTIVATE_SPIKE,
            DELETE_SPIKE
        };
        SelectableColumnComponent(SpikeGroupTableContent &tcon, int spikeID, Action action, LfpLatencyProcessor *processor);
        ~SelectableColumnComponent();
        void buttonClicked(juce::Button *b);
        void setSpikeID(int spikeID);

        ScopedPointer<ToggleButton> toggleButton;
        // void ButtonListener::buttonClicked (Button* button) override;
    private:
        LfpLatencyProcessor *processor;
        SpikeGroupTableContent &owner;
        int spikeID;
        Action action;
    };

    /* This a custom class used to add custom cells that display data on tracked spike, with the updateInfo() function handling most of the work*/
    class UpdatingTextColumnComponent : public juce::Label

    {
    public:
        UpdatingTextColumnComponent(SpikeGroupTableContent &tcon, int rowNumber, int columnNumber);
        ~UpdatingTextColumnComponent();

        ScopedPointer<Label> value;

    private:
        SpikeGroupTableContent &owner;
        juce::Colour textColour = juce::Colours::black;
    };
    class DeleteComponent : public juce::TextButton

    {
    public:
        DeleteComponent(SpikeGroupTableContent &tcon);
        ~DeleteComponent();

        ScopedPointer<TextButton> del;

    private:
        SpikeGroupTableContent &owner;
    };

private:
    LfpLatencyProcessor *processor;
};

#endif