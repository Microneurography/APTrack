// LfpLatencySpectrogram.h

#ifndef LFPLATENCYSPECTROGRAM_H
#define LFPLATENCYSPECTROGRAM_H

#include <EditorHeaders.h>

#include "LfpLatencyProcessor.h"

class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencySpectrogram : public Component
{
public:
    LfpLatencySpectrogram(int imageWidth = SPECTROGRAM_WIDTH, int imageHeight = SPECTROGRAM_HEIGHT);
    ~LfpLatencySpectrogram();

    void paint(Graphics& g) override;
    void resized() override;

    // TODO: might change to override getHeight/getWidth
    int getImageHeight();
    int getImageWidth();
    const Image &getImage();

    void update(LfpLatencyProcessor& processor, const LfpLatencyProcessorVisualizerContentComponent& content);


private:
    Image image;
    friend class LfpLatencyProcessorVisualizer;
    friend class LfpLatencyProcessorVisualizerContentComponent;

    void paintAll(Colour colour);
    void drawHot(int x, int y, float lastWindowPeak, const LfpLatencyProcessorVisualizerContentComponent& content, float level);
    void drawHotGrayScale(int x, int y, float level);
};

#endif
