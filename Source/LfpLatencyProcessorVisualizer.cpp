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

#include "LfpLatencyProcessorVisualizer.h"
#include "LfpLatencyProcessor.h"


LfpLatencyProcessorVisualizer::LfpLatencyProcessorVisualizer (LfpLatencyProcessor* processor_pointer)
{
    // Open Ephys Plugin Generator will insert generated code for editor here. Don't edit this section.
    //[OPENEPHYS_EDITOR_PRE_CONSTRUCTOR_SECTION_BEGIN]

    //m_contentLookAndFeel = new LOOKANDFEELCLASSNAME();
    //content.setLookAndFeel (m_contentLookAndFeel);
    addAndMakeVisible (&content);
    
    // Set Visualizer refresh rate
    refreshRate = 20; // 5 Hz default refresh rate
    
    /** NOTE: This should be called by the "beginAnimation()" method, so that is run after the user clicks play.
    However that does not seem to be working (begin/endAnimation() are not called at all?) so its now called in the constructor
    */
    
    windowSampleCount = 0;
    lastWindowPeak = 0;
    
    tracksAmount = 60;

    pixelsPerTrack = SPECTROGRAM_WIDTH / tracksAmount;
    
    imageLinePoint = 0;
    
    samplesAfterStimulus = 0;
    
    startCallbacks(); 
    
    // Store pointer to processor
    processor = processor_pointer;
    

    
    
    //[OPENEPHYS_EDITOR_PRE_CONSTRUCTOR_SECTION_END]
}


LfpLatencyProcessorVisualizer::~LfpLatencyProcessorVisualizer()
{
    stopCallbacks(); //MM For the time being...
}


void LfpLatencyProcessorVisualizer::resized()
{
    std::cout << "LfpLatencyProcessorVisualizer::resized" << std::endl;
    content.setBounds (getLocalBounds());
    
    
}


void LfpLatencyProcessorVisualizer::refreshState()
{
    std::cout << "LfpLatencyProcessorVisualizer::refreshState" << std::endl;
}


void LfpLatencyProcessorVisualizer::update()
{
    std::cout << "LfpLatencyProcessorVisualizer::update2" << std::endl;

	//Get number of availiable channels and update label
  // HACK: harcoded to 24
	int numAvailiableChannels = processor->getTotalDataChannels();

	std::cout << "LfpLatencyProcessorVisualizer::numAvailiableChannels" << numAvailiableChannels << std::endl;


	//Populate combobox with new channels, keep current selection if availiable
	// get current selection
	int last_triggerChannelId = content.triggerChannelComboBox->getSelectedId();
	int last_dataChannelID = content.dataChannelComboBox->getSelectedId();

	//Clear old values and repopulate combobox
	content.triggerChannelComboBox->clear();
	content.dataChannelComboBox->clear();

	content.triggerChannelComboBox->addSectionHeading("Trigger");
	content.dataChannelComboBox->addSectionHeading("Data");

	for (int ii = 0; ii < numAvailiableChannels; ii++)
	{
		//TODO: better implementation first to through channel list and find type, then populate combobox
		if (processor->getDataChannel(ii)->getChannelType() == DataChannel::HEADSTAGE_CHANNEL)
		{
			content.triggerChannelComboBox->addItem("CH" + String(ii + 1), ii + 1);
			content.dataChannelComboBox->addItem("CH" + String(ii + 1), ii + 1);
		}
		else if (processor->getDataChannel(ii)->getChannelType() == DataChannel::ADC_CHANNEL)
		{
			content.triggerChannelComboBox->addItem("ADC" + String(ii + 1), ii + 1);
			content.dataChannelComboBox->addItem("ADC" + String(ii + 1), ii + 1);
		}
	}
	// If channel still availaible, keep selection, otherwise no don't select anything (TODO: and warn?)
	// Trigger chanel combobox
	if (content.triggerChannelComboBox->getNumItems() <= last_triggerChannelId)
	{
		content.triggerChannelComboBox->setSelectedId(last_triggerChannelId);
	}
	else
	{
		content.triggerChannelComboBox->setSelectedId(0); // TODO: Set a "set default trigger channel" method in processor instead of here?
	}
	// data channel combobox
	if (content.dataChannelComboBox->getNumItems() <= last_dataChannelID)
	{
		content.dataChannelComboBox->setSelectedId(last_dataChannelID);
	}
	else
	{
		content.triggerChannelComboBox->setSelectedId(0); // TODO: Set a "set default data channel" method in processor instead of here?
	}
}


void LfpLatencyProcessorVisualizer::refresh()
{
    //std::cout << "LfpLatencyProcessorVisualizer::refresh" << std::endl;
    repaint();
}


void LfpLatencyProcessorVisualizer::beginAnimation()
{
    std::cout << "LfpLatencyProcessorVisualizer::beginAnimation" << std::endl;
    startCallbacks();
}


void LfpLatencyProcessorVisualizer::endAnimation()
{
    std::cout << "LfpLatencyProcessorVisualizer::endAnimation" << std::endl;
    stopCallbacks();
}

void LfpLatencyProcessorVisualizer::timerCallback()
{

    //std::cout << "LfpLatencyProcessorVisualizer::timerCallback" << std::endl;
    processor->changeParameter(1, content.subsamplesPerWindow);
    processor->changeParameter(2, content.startingSample);
	processor->changeParameter(3, content.triggerChannelComboBox->getSelectedId()-1); // pass channel Id -1 = channel index
	processor->changeParameter(4, content.dataChannelComboBox->getSelectedId()-1); // pass channel Id -1 = channel index
	processor->changeParameter(5, content.trigger_threshold_Slider->getValue()); // pass channel Id -1 = channel index
	
	//Update spectrogram image
	updateSpectrogram();

	if (processor->checkEventReceived())
	{
		processor->resetEventFlag();

		processTrack();

	}

    
   //Refresh canvas (redraw)
    refresh();
}


void LfpLatencyProcessorVisualizer::updateSpectrogram()
{
    return;

    for (int track=0; track < tracksAmount; track++)
    {
        //Get image dimension
        draw_imageHeight = content.spectrogramImage.getHeight();
        draw_rightHandEdge = content.spectrogramImage.getWidth()-track*pixelsPerTrack;
        imageLinePoint= 0;

        //Get data array
        float* dataToPrint = processor->getdataCacheRow(track);
        
        //Reset subsampling flags
        samplesAfterStimulus = 0;
        lastWindowPeak = 0.0f;
        windowSampleCount = 0;
        for (auto ii=0; ii< (DATA_CACHE_SIZE_SAMPLES); ii++)
        {
            if (samplesAfterStimulus > content.startingSample)
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
                if (windowSampleCount >= content.subsamplesPerWindow)//76
                {
                    //If fifo is full, print warning to console
                    if (imageLinePoint== SPECTROGRAM_HEIGHT)
                    {
                        //std::cout << "Spectrogram Full!" << std::endl;
                    }
                    
                    //If fifo not full, store peak into fifo
                    if (imageLinePoint < SPECTROGRAM_HEIGHT)
                    {
                        //Update spectrogram with selected color scheme
                        switch (content.colorStyleComboBox->getSelectedId()) {
                            case 1:
                                //WHOT
                                level = (jmap (lastWindowPeak, content.lowImageThreshold, content.highImageThreshold, 0.0f, 1.0f));
                                for (auto jj = 0; jj< pixelsPerTrack; jj++)
                                {
                                    if (lastWindowPeak > content.detectionThreshold && lastWindowPeak < content.highImageThreshold)
                                    {
                                        //Detected peak
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colours::yellowgreen);
                                    }
                                    else if (lastWindowPeak > content.highImageThreshold)
                                    {
                                        //Excessive peak
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colours::red);
                                    }
                                    else
                                    {
                                        //grayscale
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colour::fromFloatRGBA(level, level, level, 1.0f));
                                    }
                                }
                                break;
                            case 2:
                                //BHOT
                                level = 1.0f- (jmap (lastWindowPeak, content.lowImageThreshold, content.highImageThreshold, 0.0f, 1.0f));
                                for (auto jj = 0; jj< pixelsPerTrack; jj++)
                                {
                                    if (lastWindowPeak > content.detectionThreshold && lastWindowPeak < content.highImageThreshold)
                                    {
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colours::darkgreen);
                                    }
                                    else if (lastWindowPeak > content.highImageThreshold)
                                    {
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colours::red);
                                    }
                                    else
                                    {
                                        content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colour::fromFloatRGBA(level, level, level, 1.0f));
                                    }
                                }
                                break;
                            case 3:
                                //WHOT, only grayscale
                                level = (jmap (lastWindowPeak, content.lowImageThreshold, content.highImageThreshold, 0.0f, 1.0f));
                                for (auto jj = 0; jj< pixelsPerTrack; jj++)
                                {
                                    content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colour::fromFloatRGBA(level, level, level, 1.0f));
                                }
                                break;
                            case 4:
                                //BHOT, only grayscale
                                level = 1.0f- (jmap (lastWindowPeak, content.lowImageThreshold, content.highImageThreshold, 0.0f, 1.0f));
                                for (auto jj = 0; jj< pixelsPerTrack; jj++)
                                {
                                    content.spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-imageLinePoint, Colour::fromFloatRGBA(level, level, level, 1.0f));
                                }
                                break;
                            default:
                                break;
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

void LfpLatencyProcessorVisualizer::processTrack()
{

	// Get latency track data of previous row
	float* lastRowData = processor->getdataCacheRow(1);

	// HACK Get searchbox location in absolute units

	int serachBoxLocationAbs = content.startingSample + content.searchBoxLocation* content.subsamplesPerWindow;
	int searchBoxWidthAbs = content.searchBoxWidth * content.subsamplesPerWindow;

	// get spike magnitude
	float maxLevel = FloatVectorOperations::findMaximum(lastRowData + (serachBoxLocationAbs - searchBoxWidthAbs),
		searchBoxWidthAbs * 2 + content.subsamplesPerWindow);

	// get spike location
	int SpikeLocationAbs = std::max_element(lastRowData + (serachBoxLocationAbs - searchBoxWidthAbs),
		lastRowData + (serachBoxLocationAbs + searchBoxWidthAbs)) - lastRowData; // Note we substract lastRowData so that index starts at zero

	int SpikeLocationRel= (SpikeLocationAbs - content.startingSample) / content.subsamplesPerWindow;

	//display values
	content.ROISpikeMagnitude->setText(String(maxLevel,1) + " uV");
	content.ROISpikeLatency->setText(String(SpikeLocationAbs/30.0f,1)+" ms"); //Convert abs position in samples to ms 30kSamp/s=30Samp/ms TODO: get actual sample size from processor

	// If we have enabled spike tracking the track spike
	if (content.trackSpike_button->getToggleState() == true) {

		// Check for spike inside ROI box
		if (maxLevel > content.detectionThreshold)
		{
			content.spikeDetected = true;
			content.searchBoxSlider->setValue(SpikeLocationRel);

			// If we have enabled threshold tracking then update threshold:
			// Spike, decrease stimulation
			if (content.trackThreshold_button->getToggleState() == true)
			{
				float newStimulus = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
				content.stimulusVoltageSlider->setValue(std::max(newStimulus, content.stimulusVoltageMin));
				content.ppControllerComponent->setStimulusVoltage(std::max(newStimulus, content.stimulusVoltageMin));
			}
		}
		else
		{
			content.spikeDetected = false;

			// If we have enabled threshold tracking then update threshold:
			if (content.trackThreshold_button->getToggleState() == true)
			{
				//No spike, increase stimulation
				float newStimulus = content.stimulusVoltage + std::abs(content.trackSpike_IncreaseRate);
				content.stimulusVoltageSlider->setValue(std::min(newStimulus, content.stimulusVoltageMax));
				content.ppControllerComponent->setStimulusVoltage(std::min(newStimulus, content.stimulusVoltageMax));
			}

		}
	}

	
}

void LfpLatencyProcessorVisualizer::setParameter (int parameter, float newValue)
{
}


void LfpLatencyProcessorVisualizer::setParameter (int parameter, int val1, int val2, float newValue)
{
}
