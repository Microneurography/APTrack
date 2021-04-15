// LfpLatencySpectrogramControlPanel.h

#ifndef LFPLATENCYSPECTROGRAMCONTROLPANEL_H
#define LFPLATENCYSPECTROGRAMCONTROLPANEL_H

#include <EditorHeaders.h>

class LfpLatencyLabelTextEditor;
class LfpLatencyLabelSlider;
class LfpLatencyLabelVerticalSlider;

class LfpLatencySpectrogramControlPanel : public Component
{
public:
    LfpLatencySpectrogramControlPanel(LfpLatencyProcessorVisualizerContentComponent* content);

    void resized() override;
    bool keyPressed(const KeyPress& k) override;

    void setImageThresholdRange(double newMinimum, double newMaximum, double newInterval = 0);
    void setHighImageThresholdText(const String& newText);
    void setDetectionThresholdText(const String& newText);
    void setLowImageThresholdText(const String& newText);
private:
    ScopedPointer<GroupComponent> outline;

    ScopedPointer<LfpLatencyLabelVerticalSlider> imageThreshold;

    ScopedPointer<LfpLatencyLabelTextEditor> highImageThreshold;
    ScopedPointer<LfpLatencyLabelTextEditor> detectionThreshold;
    ScopedPointer<LfpLatencyLabelTextEditor> lowImageThreshold;

    ScopedPointer<LfpLatencyLabelSlider> subsamplesPerWindow;
    ScopedPointer<LfpLatencyLabelSlider> startingSample;
    ScopedPointer<LfpLatencyLabelSlider> conductionDistance;
};

#endif
