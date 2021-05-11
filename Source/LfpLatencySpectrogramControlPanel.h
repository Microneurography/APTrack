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