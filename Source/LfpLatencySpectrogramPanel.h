// LfpLatencySpectrogramPanel.h

#ifndef LFPLATENCYSPECTROGRAMPANEL_H
#define LFPLATENCYSPECTROGRAMPANEL_H

#include <EditorHeaders.h>

#include "LfpLatencyProcessor.h"
#include "LfpLatencyElements.h"

class LfpLatencySpectrogram;
class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencySpectrogramPanel : public Component
{
public:
    LfpLatencySpectrogramPanel(LfpLatencyProcessorVisualizerContentComponent* content);
    void resized() override;

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
