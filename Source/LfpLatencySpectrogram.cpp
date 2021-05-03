// LfpLatencySpectrogram.cpp

#include "LfpLatencySpectrogram.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

LfpLatencySpectrogram::LfpLatencySpectrogram(int imageWidth, int imageHeight)
: image(Image::RGB, imageWidth, imageHeight, true)
{
    std::cout << "Pre" << std::endl;
    std::cout << "Med" << std::endl;

    //Paint image
    paintAll(Colours::yellowgreen);

    std::cout << "Post" << std::endl;
}

LfpLatencySpectrogram::~LfpLatencySpectrogram()
{

}

void LfpLatencySpectrogram::paint(Graphics& g)
{

}

void LfpLatencySpectrogram::resized()
{

}

int LfpLatencySpectrogram::getImageHeight()
{
    return image.getHeight();
}

int LfpLatencySpectrogram::getImageWidth()
{
    return image.getWidth();
}

void LfpLatencySpectrogram::paintAll(Colour colour)
{
    for (auto ii = 0; ii < getImageWidth(); ii++)
    {
        for (auto jj = 0; jj < getImageHeight(); jj++)
        {
            image.setPixelAt(ii, jj, colour);
        }
    }
    std::cout << "finished paint" << std::endl;
}

const Image& LfpLatencySpectrogram::getImage()
{
    return image;
}

void LfpLatencySpectrogram::update(LfpLatencyProcessor& processor, const LfpLatencyProcessorVisualizerContentComponent& content)
{
    // TODO: Following comments beside variable represent it's original source in visualizer: field variable in class. 
    //       But most of them are not used in other places, so potentially some of them can be removed from the class definition.
    int tracksAmount = 60; // LfpLatencyProcessorVisualizer.tracksAmount = 60;
    int pixelsPerTrack = getImageWidth() / tracksAmount; // LfpLatencyProcessorVisualizer.pixelsPerTrack = SPECTROGRAM_WIDTH / tracksAmount;
    for (int track = 0; track < tracksAmount; track++)
    {
        //Get image dimension
        int draw_imageHeight = getImageHeight(); // LfpLatencyProcessorVisualizer.draw_imageHeight;
        int draw_rightHandEdge = getImageWidth() - track * pixelsPerTrack; // LfpLatencyProcessorVisualizer.draw_rightHandEdge;
        int imageLinePoint = 0;

        //Get data array
        float* dataToPrint = processor.getdataCacheRow(track);

        //Reset subsampling flags
        int samplesAfterStimulus = 0; // LfpLatencyProcessorVisualizer.samplesAfterStimulus = 0;
        float lastWindowPeak = 0.0f; // LfpLatencyProcessorVisualizer.lastWindowPeak = 0.0f;
        int windowSampleCount = 0; // LfpLatencyProcessorVisualizer.windowSampleCount = 0;
        for (auto ii = 0; ii < (DATA_CACHE_SIZE_SAMPLES); ii++)
        {
            if (samplesAfterStimulus > content.getStartingSample())
            {
                auto sample = dataToPrint[ii];

                //If current sample is larger than previously stored peak, store sample as new peak
                if (sample > lastWindowPeak)
                {
                    lastWindowPeak = sample;
                }

                //Increment window sample counter
                ++windowSampleCount;

                //If window is full, push window's peak into fifo
                if (windowSampleCount >= content.getSubsamplesPerWindow())//76
                {
                    //If fifo is full, print warning to console
                    if (imageLinePoint == getImageHeight())
                    {
                        //std::cout << "Spectrogram Full!" << std::endl;
                    }

                    //If fifo not full, store peak into fifo
                    if (imageLinePoint < getImageHeight())
                    {
                        float wLevel = (jmap(lastWindowPeak, content.getLowImageThreshold(), content.getHighImageThreshold(), 0.0f, 1.0f)); // LfpLatencyProcessorVisualizer.level;
                        float bLevel = 1.0f - wLevel;
                        int comboBoxSelectedId = content.getColorStyleComboBoxSelectedId();
                        for (auto jj = 0; jj < pixelsPerTrack; jj++)
                        {
                            int x = draw_rightHandEdge - jj - 1; // x in [draw_rightHandEdge-pixelsPerTrack, ..., draw_rightHandEdge-1]
                            int y = draw_imageHeight - imageLinePoint - 1; // y in [0, ..., getImageHeight]
                            //Update spectrogram with selected color scheme
                            switch (comboBoxSelectedId) {
                            case 1:
                                //WHOT
                                drawHot(x, y, lastWindowPeak, content, wLevel);
                                break;
                            case 2:
                                //BHOT
                                drawHot(x, y, lastWindowPeak, content, bLevel);
                                break;
                            case 3:
                                //WHOT, only grayscale
                                drawHotGrayScale(x, y, wLevel);
                                break;
                            case 4:
                                //BHOT, only grayscale
                                drawHotGrayScale(x, y, bLevel);
                                break;
                            default:
                                break;
                            }
                        }
                        //Go to next line
                        imageLinePoint++;
                    }
                    //Reset subsampling flags
                    lastWindowPeak = 0.0f;
                    windowSampleCount = 0;
                }
            }
            samplesAfterStimulus++;
        }

    }

}

void LfpLatencySpectrogram::drawHot(int x, int y, float lastWindowPeak, const LfpLatencyProcessorVisualizerContentComponent& content, float level)
{
    if (lastWindowPeak > content.getDetectionThreshold() && lastWindowPeak < content.getHighImageThreshold())
    {
        //Detected peak
        image.setPixelAt(x, y, Colours::yellowgreen);
    }
    else if (lastWindowPeak > content.getHighImageThreshold())
    {
        //Excessive peak
        image.setPixelAt(x, y, Colours::red);
    }
    else
    {
        //grayscale
        image.setPixelAt(x, y, Colour::fromFloatRGBA(level, level, level, 1.0f));
    }
}

void LfpLatencySpectrogram::drawHotGrayScale(int x, int y, float level)
{
    image.setPixelAt(x, y, Colour::fromFloatRGBA(level, level, level, 1.0f));
}