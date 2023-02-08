// LfpLatencySpectrogram.h
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
    void paint(Graphics &g) override;

    int getImageHeight() const;
    int getImageWidth() const;
    const Image &getImage() const;

    void update(LfpLatencyProcessor &processor, const LfpLatencyProcessorVisualizerContentComponent &content);

private:
    Image image;

    void paintAll(Colour colour);
    void drawHot(int x, int y, float lastWindowPeak, const LfpLatencyProcessorVisualizerContentComponent &content, float level);
    void drawHotGrayScale(int x, int y, float level);
};

#endif
