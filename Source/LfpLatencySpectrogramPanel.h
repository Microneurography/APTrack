// LfpLatencySpectrogramPanel.h
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

#ifndef LFPLATENCYSPECTROGRAMPANEL_H
#define LFPLATENCYSPECTROGRAMPANEL_H

#include <EditorHeaders.h>

#include "LfpLatencySpectrogram.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencyProcessor.h"
#include "LfpLatencyElements.h"

class LfpLatencySpectrogram;
class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencySpectrogramPanel : public Component
{
public:
    LfpLatencySpectrogramPanel(LfpLatencyProcessorVisualizerContentComponent* content);
    void resized() override;
    void paint(Graphics& g) override;

    void updateSpectrogram(LfpLatencyProcessor& processor, const LfpLatencyProcessorVisualizerContentComponent& content);
    void setSearchBoxValue(double newValue);
    double getSearchBoxValue() const;
    void changeSearchBoxValue(double deltaValue);
    void setSearchBoxWidthValue(double newValue);
    void spikeIndicatorTrue(bool spikeFound);
    int getImageHeight() const;
    int getImageWidth() const;
private:
    ScopedPointer<GroupComponent> outline;
    ScopedPointer<LfpLatencySpectrogram> spectrogram;
    ScopedPointer<LfpLatencyLabelLinearVerticalSliderNoTextBox> searchBox;
    ScopedPointer<LfpLatencyLabelSliderNoTextBox> searchBoxWidth;
    ScopedPointer<LfpLatencySearchBox> searchBoxRectangle;
    ScopedPointer<Label> spikeIndicator;

    const LfpLatencyProcessorVisualizerContentComponent& content;
};

#endif
