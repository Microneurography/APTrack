/*
   ------------------------------------------------------------------

   This file is part of the Open Ephys GUI
   Copyright (C) 2016 Open Ephys

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

#ifndef LFPLATENCYPROCESSORVISUALIZER_H_INCLUDED
#define LFPLATENCYPROCESSORVISUALIZER_H_INCLUDED

#include <VisualizerEditorHeaders.h>
#include <AllLookAndFeels.h>

#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

/**
    Class for displaying data in any subclasses of VisualizerEditor either in the tab or separate window.

    @see Visualizer, LfpDisplayCanvas, SpikeDisplayCanvas
*/
class LfpLatencyProcessorVisualizer : public Visualizer
{
public:
    /** The class constructor, used to initialize any members. */
    LfpLatencyProcessorVisualizer (LfpLatencyProcessor* processor);

    /** The class destructor, used to deallocate memory */
    ~LfpLatencyProcessorVisualizer();

    /** Called every time when canvas is resized or moved. */
    void resized() override;

    /** Called when the component's tab becomes visible again.*/
    void refreshState() override;

    /** Called when parameters of underlying data processor are changed.*/
    void update() override;

    /** Called instead of "repaint" to avoid redrawing underlying components if not necessary.*/
    void refresh() override;

    /** Called when data acquisition is active.*/
    void beginAnimation() override;

    /** Called when data acquisition ends.*/
    void endAnimation() override;

    /** Called by an editor to initiate a parameter change.*/
    void setParameter (int, float) override;

    /** Called by an editor to initiate a parameter change.*/
    void setParameter (int, int, int, float) override;
    
    /** Updates spectrogram*/
    void timerCallback() override;
    
    /** Shuffle spectrogram to left*/ 
    void updateSpectrogram(); 

	/** Process new track*/
	void processTrack();
    
    
    /** Fill in rightmost edge of spectrogram with up to date data*/
    /*
     //DEPRECATED
    void newLineOfSpectrogram();
    */
    
    
private:

    
    float level;
    
    int pixelsPerTrack;
    
    int imageLinePoint;
    
    int samplesAfterStimulus;
    
    int missCounter;
    
    int hitCounter;
    
    int prevLocation;
    
    float lastWindowPeak;
    int windowSampleCount;
    
    int draw_imageHeight;
    int draw_rightHandEdge;
    
    //Pointer to processor
    LfpLatencyProcessor* processor;
    
    friend class LfpLatencyProcessorVisualizerContentComponent;
    
    // This component contains all components and graphics that were added using Projucer.
    // It's bounds initially have same bounds as the canvas itself.
    LfpLatencyProcessorVisualizerContentComponent content;
    //
    //ScopedPointer<LookAndFeel> m_contentLookAndFeel;

    // ========================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfpLatencyProcessorVisualizer);
};


#endif // LFPLATENCYPROCESSORVISUALIZER_H_INCLUDED
