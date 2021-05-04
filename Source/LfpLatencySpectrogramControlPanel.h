// LfpLatencySpectrogramControlPanel.h

#ifndef LFPLATENCYSPECTROGRAMCONTROLPANEL_H
#define LFPLATENCYSPECTROGRAMCONTROLPANEL_H

#include <EditorHeaders.h>

class LfpLatencyProcessorVisualizerContentComponent;
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
    static void loadParameters(const std::map<String, String>& newParameters);

private:
    ScopedPointer<GroupComponent> outline;

    static ScopedPointer<LfpLatencyLabelVerticalSlider> imageThreshold;

    static ScopedPointer<LfpLatencyLabelTextEditor> highImageThreshold;
    static ScopedPointer<LfpLatencyLabelTextEditor> detectionThreshold;
    static ScopedPointer<LfpLatencyLabelTextEditor> lowImageThreshold;

    static ScopedPointer<LfpLatencyLabelSlider> subsamplesPerWindow;
    static ScopedPointer<LfpLatencyLabelSlider> startingSample;
    static ScopedPointer<LfpLatencyLabelSlider> conductionDistance;
};

#endif
