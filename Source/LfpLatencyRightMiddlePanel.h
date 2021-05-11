// LfpLatencyRightMiddlePanel.h

#ifndef LFPLATENCYRIGHTMIDDLEPANEL_H
#define LFPLATENCYRIGHTMIDDLEPANEL_H

#include <EditorHeaders.h>

class LfpLatencyLabelTextEditor;
class LfpLatencyLabelSlider;
class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencyRightMiddlePanel : public Component
{
public:
    LfpLatencyRightMiddlePanel(LfpLatencyProcessorVisualizerContentComponent* content);
    void resized() override;

    /* Just pass in the value string, " ms" will be auto added. */
    void setROISpikeLatencyText(const String& newText);

    /* Just pass in the value string, " uV" will be auto added. */
    void setROISpikeValueText(const String& newText);

    double getTriggerThresholdValue() const;
private:
    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeLatency;
    ScopedPointer<LfpLatencyLabelTextEditor> ROISpikeValue; // or ROISpikeMagnitude in some code. TODO: make them consistent
    ScopedPointer<LfpLatencyLabelSlider> triggerThreshold;
};

#endif