// LfpLatencyRightMiddlePanel.cpp
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

#include "LfpLatencyRightMiddlePanel.h"



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
