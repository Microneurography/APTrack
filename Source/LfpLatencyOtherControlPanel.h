// LfpLatencyOtherControlPanel.h
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

// TODO: the commented out code in this component was originally wrote 
//       for the old design of the top right panel. Kept them for potential 
//       future use when extracting the popup box into a seperate component/

#ifndef LFPLATENCYOTHERCONTROLPANEL_H
#define LFPLATENCYOTHERCONTROLPANEL_H

#include <EditorHeaders.h>
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencyElements.h"

class LfpLatencyLabelComboBox;
class LfpLatencyLabelToggleButton;

class LfpLatencyOtherControlPanel : public Component
{
public:
    LfpLatencyOtherControlPanel(LfpLatencyProcessorVisualizerContentComponent* content);
    void resized() override;

    /** Get options button bounds in this panel's parent, which should be LfpLatencyProcessorVisualizerContentComponent.
        This method could be used for positioning of setup box.
    */
    Rectangle<int> getOptionsBoundsInPanelParent() const;

    /** Get setup button bounds in this panel's parent, which should be LfpLatencyProcessorVisualizerContentComponent.
        This method could be used for positioning of setup box.
    */
    Rectangle<int> getSetupBoundsInPanelParent() const;

    std::map<String, Rectangle<int>> getTableBounds() const;

    /*
    int getColorStyleSelectedId() const;

    int getTriggerChannelSelectedId() const;
    void setTriggerChannelSelectedId(int newItemId);
    void clearTriggerChannel();
    void addTriggerChannelSectionHeading(const String& headingName);
    void addTriggerChannelItem(const String& newItemText, int newItemId);
    int getTriggerChannelNumItems() const;

    int getDataChannelSelectedId() const;
    void setDataChannelSelectedId(int newItemId);
    void clearDataChannel();
    void addDataChannelSectionHeading(const String& headingName);
    void addDataChannelItem(const String& newItemText, int newItemId);
    int getDataChannelNumItems() const;

    bool getTrackSpikeToggleState() const;
    bool getTrackThresholdToggleState() const;
    */

    /* If shouldBeEnabled set to false, will also toggle the button to false. */
    //void setTrackThresholdEnabled(bool shouldBeEnabled);
private:
    ScopedPointer<GroupComponent> outline;

    ScopedPointer<TextButton> setup;
    ScopedPointer<TextButton> options;

    Rectangle<int> getBoundsInPanelParent(const Rectangle<int>& bounds) const;

    /*
    ScopedPointer<LfpLatencyLabelComboBox> colorStyle;
    ScopedPointer<LfpLatencyLabelToggleButton> extendedColorScale;
    ScopedPointer<LfpLatencyLabelComboBox> triggerChannel;
    ScopedPointer<LfpLatencyLabelComboBox> dataChannel;
    ScopedPointer<LfpLatencyLabelToggleButton> trackSpike;
    ScopedPointer<LfpLatencyLabelToggleButton> trackThreshold;
    */
};

#endif
