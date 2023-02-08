// LfpLatencySpectrogramControlPanel.h
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

#ifndef LFPLATENCYSPECTROGRAMCONTROLPANEL_H
#define LFPLATENCYSPECTROGRAMCONTROLPANEL_H

#include <EditorHeaders.h>
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencyElements.h"

class LfpLatencyProcessorVisualizerContentComponent;
class LfpLatencyLabelTextEditor;
class LfpLatencyLabelSlider;
class LfpLatencyLabelHorizontalSlider;
class LfpLatencyLabelSliderNoTextBox;

class LfpLatencySpectrogramControlPanel : public Component
{
public:
    LfpLatencySpectrogramControlPanel(LfpLatencyProcessorVisualizerContentComponent* content);

    void resized() override;

    void setImageThresholdRange(double newMinimum, double newMaximum, double newInterval = 0);
    void setHighImageThresholdText(const String& newText);
    void setDetectionThresholdText(const String& newText);
    void setLowImageThresholdText(const String& newText);

    void setStartingSampleValue(double newValue);
    void setSubsamplesPerWindowValue(double newValue);
    double getSubsamplesPerWindowValue() const;
    double getSubsamplesPerWindowMaximum() const;
    double getSubsamplesPerWindowMinimum() const;
    void changeSubsamplesPerWindowValue(double deltaValue);
    double getStartingSampleValue() const;
    double getStartingSampleMaximum() const;
    double getStartingSampleMinimum() const;
    void changeStartingSampleValue(double deltaValue);
    double getImageThresholdMaxValue() const;
    void setImageThresholdMaxValue(double newValue);
    void changeImageThresholdMaxValue(double deltaValue);
    double getImageThresholdMinValue() const;
    void setImageThresholdMinValue(double newValue);
    void changeImageThresholdMinValue(double deltaValue);
    double getImageThresholdMaximum() const;
    double getImageThresholdMinimum() const;
    void setDetectionThresholdValue(double newValue);
    void setSearchBoxWidthValue(double newValue);

    static void loadParameters(const std::map<String, String>& newParameters);

private:
    ScopedPointer<GroupComponent> outline;

    static ScopedPointer<LfpLatencyLabelHorizontalSlider> imageThreshold;

    static ScopedPointer<LfpLatencyLabelTextEditor> highImageThreshold;
    static ScopedPointer<LfpLatencyLabelTextEditor> detectionThreshold;
    static ScopedPointer<LfpLatencyLabelTextEditor> lowImageThreshold;

    static ScopedPointer<LfpLatencyLabelSlider> subsamplesPerWindowSlider;
    static ScopedPointer<LfpLatencyLabelSlider> startingSampleSlider;
    static ScopedPointer<LfpLatencyLabelSlider> conductionDistanceSlider;
    static ScopedPointer<LfpLatencyLabelSliderNoTextBox> searchBoxWidthSlider;
};

#endif
