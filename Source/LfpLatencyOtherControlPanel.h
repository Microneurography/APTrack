// LfpLatencyOtherControlPanel.h

// TODO: the commented out code in this component was originally wrote 
//       for the old design of the top right panel. Kept them for potential 
//       future use when extracting the popup box into a seperate component/

#ifndef LFPLATENCYOTHERCONTROLPANEL_H
#define LFPLATENCYOTHERCONTROLPANEL_H

#include <EditorHeaders.h>

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

    //double getTriggerThresholdValue() const;

    /* Just pass in the value string, " ms" will be auto added. */
    //void setROISpikeLatencyText(const String& newText);

    /* Just pass in the value string, " uV" will be auto added. */
    //void setROISpikeValueText(const String& newText);

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
    ScopedPointer<LfpLatencyLabelSlider> triggerThreshold;

    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeLatency;
    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeValue; // or ROISpikeMagnitude in some code. TODO: make them consistent
    */
};

#endif
