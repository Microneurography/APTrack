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
#pragma once
#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"
#include "../LfpLatencyProcessor.h"

#define TIMER_UI 0

class ppControllerVisualizer : public Component,
                               public Button::Listener,
                               public MultiTimer
{
public:
    ppControllerVisualizer(LfpLatencyProcessor *processor);

    ~ppControllerVisualizer();

    void buttonClicked(Button *buttonThatWasClicked) override;

    void paint(Graphics &g) override;

    void paintOverChildren(Graphics &g) override;

    void resized() override;
    void initialize();

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
    ScopedPointer<TextButton> reconnect_button;
    LfpLatencyProcessor *processor;
    ppController *controller;
    File lastFilePath;

    void timerCallback(int timerID) override;

    bool pulsePalConnected;
};