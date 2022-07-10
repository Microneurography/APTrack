// LfpLatencySpectrogram.h

#ifndef LFPLATENCYSPECTROGRAM_H
#define LFPLATENCYSPECTROGRAM_H
#define tracksAmount 60

#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

class LfpLatencyProcessorVisualizerContentComponent;

class LfpLatencySpectrogram : public Component
{
public:
    LfpLatencySpectrogram(int imageWidth = SPECTROGRAM_WIDTH, int imageHeight = SPECTROGRAM_HEIGHT);
    void paint(Graphics& g) override;

    int getImageHeight() const;
    int getImageWidth() const;
    const Image &getImage() const;

    void update(LfpLatencyProcessor& processor, const LfpLatencyProcessorVisualizerContentComponent& content);
private:
    Image image;

    void paintAll(Colour colour);
    void drawHot(int x, int y, float lastWindowPeak, const LfpLatencyProcessorVisualizerContentComponent& content, float level);
    void drawHotGrayScale(int x, int y, float level);
};

#endif
