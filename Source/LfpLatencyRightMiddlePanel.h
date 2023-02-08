// LfpLatencyRightMiddlePanel.h
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

#ifndef LFPLATENCYRIGHTMIDDLEPANEL_H
#define LFPLATENCYRIGHTMIDDLEPANEL_H

#include <EditorHeaders.h>
#include "LfpLatencyElements.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

class LfpLatencyLabelTextEditor;
class LfpLatencyLabelSlider;
class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencyRightMiddlePanel : public Component
{
public:
    LfpLatencyRightMiddlePanel(LfpLatencyProcessorVisualizerContentComponent *content);
    void resized() override;

    /* Just pass in the value string, " ms" will be auto added. */
    void setROISpikeLatencyText(const String &newText);

    /* Just pass in the value string, " uV" will be auto added. */
    void setROISpikeMagnitudeText(const String &newText);

    double getTriggerThresholdValue() const;

private:
    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeLatency;
    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeMagnitude;
    ScopedPointer<LfpLatencyLabelSlider> triggerThreshold;
};

#endif
