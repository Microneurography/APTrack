// LfpLatencySpectrogram.cpp
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

#include "LfpLatencySpectrogram.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

LfpLatencySpectrogram::LfpLatencySpectrogram(int imageWidth, int imageHeight)
    : image(Image::RGB, imageWidth, imageHeight, true), bmap(tracksAmount, vector<float>(DATA_CACHE_SIZE_SAMPLES))
{
    // Paint image
    paintAll(Colours::yellowgreen);
}

void LfpLatencySpectrogram::paint(Graphics &g)
{
    auto area = getLocalBounds();
    g.drawImageWithin(image,
                      area.getX(), area.getY(),
                      area.getWidth(), area.getHeight(),
                      RectanglePlacement(RectanglePlacement::stretchToFit));
}

int LfpLatencySpectrogram::getImageHeight() const
{
    return image.getHeight();
}

int LfpLatencySpectrogram::getImageWidth() const
{
    return image.getWidth();
}

// The comments here are ideas on how you could save the spectogram
// I haven't made any comments in LfpLatencySpectogram.h, so if you do decide to implement it,
// you will have to create all the variables yourself and handle delteing/deconstructing them.
// You will also need to write LfpLatencySpectrogram::loadImage (It's commented below this function)
// and make some changes in LfpLatencyProcessor::loadRecoveryData, I have added comments there for you.
// I hope this is actually helpful, and I hope you have a nice day!
void LfpLatencySpectrogram::paintAll(Colour colour)
{
    // dataXmlValue = new XmlElement("Data");
    // int savingIndex = 0;
    for (auto ii = 0; ii < getImageWidth(); ii++)
    {
        for (auto jj = 0; jj < getImageHeight(); jj++)
        {
            image.setPixelAt(ii, jj, colour);
            //  dataXmlValue->setAttribute(savingIndex, colour); // I think you might need to convert savingIndex and Colour to a String, but you also may not
            // savingIndex++;
        }
    }
    // LfpLatencyProcessor::saveRecoveryData(dataXmlValue);
    std::cout << "finished paint" << std::endl;
    // Alternatively, there is Image::BitmapData
    // a class that allows you to easily retrieve a section
    // of an image as raw pixel data so it can be read from/written to
    // However, there are warnings in the docs that it should only be used if you know what you're doing
    // And I rarely do.
}

// theorised function the index way
/*
const Image& LfpLatencySpectrogram::loadImage(std::vector newParametersAsVector) // I don't actually know if this is how you pass a vector
{
    // int loadingIndex = 0;
    for (auto ii = 0; ii < getImageWidth(); ii++)
    {
        for (auto jj = 0; jj < getImageHeight(); jj++)
        {
            image.setPixelAt(ii, jj, colour);
            //  xmlElementName->setAttribute(ii, jj, newParametersAsVector[loadingIndex]);
            // loadingIndex++;
        }
    }
    std::cout << "finished loading paint" << std::endl;
}
*/

const Image &LfpLatencySpectrogram::getImage() const
{
    return image;
}

void LfpLatencySpectrogram::update(LfpLatencyProcessor &processor, const LfpLatencyProcessorVisualizerContentComponent &content)
{
    // TODO: Following comments beside variable represent it's original source in visualizer: field variable in class.
    //       But most of them are not used in other places, so potentially some of them can be removed from the class definition.
    int comboBoxSelectedId = content.getColorStyleComboBoxSelectedId();
    int tracksAmount2 = tracksAmount;
    if (comboBoxSelectedId == 5)
    {
        tracksAmount2 = max(tracksAmount2 / 2, 1);
    }

    int pixelsPerTrack = getImageWidth() / tracksAmount2; // LfpLatencyProcessorVisualizer.pixelsPerTrack = SPECTROGRAM_WIDTH / tracksAmount;

    for (int track = 0; track < tracksAmount2; track++)
    {
        // Get image dimension
        int draw_imageHeight = getImageHeight();                           // LfpLatencyProcessorVisualizer.draw_imageHeight;
        int draw_rightHandEdge = getImageWidth() - track * pixelsPerTrack; // LfpLatencyProcessorVisualizer.draw_rightHandEdge;
        int imageLinePoint = 0;

        // Get data array
        float *dataToPrint = processor.getdataCacheRow(track);

        // Reset subsampling flags
        int samplesAfterStimulus = content.getStartingSample(); // LfpLatencyProcessorVisualizer.samplesAfterStimulus = 0;
        float lastWindowPeak = 0.0f;                            // LfpLatencyProcessorVisualizer.lastWindowPeak = 0.0f;
        int windowSampleCount = 0;                              // LfpLatencyProcessorVisualizer.windowSampleCount = 0;
        for (auto ii = content.getStartingSample(); ii < (DATA_CACHE_SIZE_SAMPLES); ii++)
        {

            auto sample = dataToPrint[ii];

            // If current sample is larger than previously stored peak, store sample as new peak
            if (sample > lastWindowPeak)
            {
                lastWindowPeak = sample;
            }

            // Increment window sample counter
            ++windowSampleCount;

            // If window is full, push window's peak into fifo
            if (windowSampleCount >= content.getSubsamplesPerWindow()) // 76
            {

                // If fifo not full, store peak into fifo
                if (imageLinePoint < getImageHeight())
                {

                    float wLevel = (jmap(lastWindowPeak, content.getLowImageThreshold(), content.getHighImageThreshold(), 0.0f, 1.0f)); // LfpLatencyProcessorVisualizer.level;
                    float bLevel = 1.0f - wLevel;
                    bmap[track][imageLinePoint] = wLevel;
                    int comboBoxSelectedId = content.getColorStyleComboBoxSelectedId();
                    for (auto jj = 0; jj < pixelsPerTrack; jj++)
                    {
                        int x = draw_rightHandEdge - jj - 1;           // x in [draw_rightHandEdge-pixelsPerTrack, ..., draw_rightHandEdge-1]
                        int y = draw_imageHeight - imageLinePoint - 1; // y in [0, ..., getImageHeight]
                        // Update spectrogram with selected color scheme
                        switch (comboBoxSelectedId)
                        {
                        case 1:
                            // WHOT
                            drawHot(x, y, lastWindowPeak, content, wLevel);
                            break;
                        case 2:
                            // BHOT
                            drawHot(x, y, lastWindowPeak, content, bLevel);
                            break;
                        case 3:
                            // WHOT, only grayscale
                            drawHotGrayScale(x, y, wLevel);
                            break;
                        case 4:
                            // BHOT, only grayscale
                            drawHotGrayScale(x, y, bLevel);
                            break;
                        default:
                            break;
                        }
                    }
                    // Go to next line
                    imageLinePoint++;
                }
                // Reset subsampling flags
                lastWindowPeak = 0.0f;
                windowSampleCount = 0;
            }

            samplesAfterStimulus++;
        }
    }

    auto detectionThreshold_scaled = jmap(content.getDetectionThreshold(), content.getLowImageThreshold(), content.getHighImageThreshold(), 0.0f, 1.0f);
    if (comboBoxSelectedId == 5)
    {

        // draw lines instead
        juce::Graphics g(image);
        g.fillAll(juce::Colours::black);
        g.setOpacity(1);
        // g.setFillType(juce::FillType(juce::Colours::orange));

        for (int x = 0; x < tracksAmount2; x++)
        {
            auto xOffset = getImageWidth() - ((x + 2) * pixelsPerTrack);
            juce::Path path;
            juce::Path highlightPath;
            bool isHighlighting = false;
            path.startNewSubPath(getImageWidth() - ((x + 2) * pixelsPerTrack), getImageHeight());
            for (int y = 0; y < getImageHeight(); y++)
            {
                auto curX = xOffset + ((1 - bmap[x][y]) * (pixelsPerTrack));
                auto curY = getImageHeight() - y;
                path.lineTo(curX, curY);

                // highlight where threshold crossings occur
                if ((bmap[x][y]) > detectionThreshold_scaled)
                {
                    juce::Path p;
                    p.startNewSubPath(getImageWidth() - ((x + 2) * pixelsPerTrack), curY);
                    p.lineTo(getImageWidth() - ((x + 2 - 1) * pixelsPerTrack), curY);
                    g.setColour(juce::Colours::red);
                    g.strokePath(p, juce::PathStrokeType(0.5));
                    if (!isHighlighting)
                    {
                        highlightPath.startNewSubPath(curX, curY);
                        isHighlighting = true;
                    }
                    else
                    {
                        highlightPath.lineTo(curX, curY);
                    }
                }
                else
                {
                    if (isHighlighting)
                    {
                        highlightPath.lineTo(curX, curY);
                        isHighlighting = false;
                    }
                }
            }
            g.setColour(juce::Colours::white);
            g.strokePath(path, juce::PathStrokeType(0.5));
            g.setColour(juce::Colours::red);
            // g.strokePath(highlightPath, juce::PathStrokeType(0.5));
        }
        juce::Path refPath;
        auto curX = getImageWidth() - (2 * pixelsPerTrack) + ((1 - detectionThreshold_scaled) * (pixelsPerTrack));
        refPath.startNewSubPath(curX, 0);
        refPath.lineTo(curX, getImageHeight());
        g.setColour(juce::Colours::green);
        g.strokePath(refPath, juce::PathStrokeType(0.5));
    }
    // // draw search box
    // juce::Graphics g (image);
    // g.setColour(juce::Colours::red);
    // auto box_x = getImageWidth() - pixelsPerTrack;
    // auto sbl = content.getSearchBoxInfo();
    // g.drawLine(box_x,get<1>(sbl),box_x+pixelsPerTrack,get<1>(sbl));
}

void LfpLatencySpectrogram::drawHot(int x, int y, float lastWindowPeak, const LfpLatencyProcessorVisualizerContentComponent &content, float level)
{
    if (lastWindowPeak > content.getDetectionThreshold() && lastWindowPeak < content.getHighImageThreshold())
    {
        // Detected peak
        image.setPixelAt(x, y, Colours::yellowgreen);
    }
    else if (lastWindowPeak > content.getHighImageThreshold())
    {
        // Excessive peak
        image.setPixelAt(x, y, Colours::red);
    }
    else
    {
        // grayscale
        image.setPixelAt(x, y, Colour::fromFloatRGBA(level, level, level, 1.0f));
    }
}

void LfpLatencySpectrogram::drawHotGrayScale(int x, int y, float level)
{
    image.setPixelAt(x, y, Colour::fromFloatRGBA(level, level, level, 1.0f));
}