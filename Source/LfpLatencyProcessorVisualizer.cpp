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


LfpLatencyProcessorVisualizer::LfpLatencyProcessorVisualizer (LfpLatencyProcessor* processor_pointer):
	content(processor_pointer)
{
    //m_contentLookAndFeel = new LOOKANDFEELCLASSNAME();
    //content.setLookAndFeel (m_contentLookAndFeel);
    addAndMakeVisible (&content);
    
    // Set Visualizer refresh rate
    refreshRate = 20; // 5 Hz default refresh rate
    
    /** NOTE: This should be called by the "beginAnimation()" method, so that is run after the user clicks play.
    However that does not seem to be working (begin/endAnimation() are not called at all?) so its now called in the constructor
    */
    
    //windowSampleCount = 0;
    //lastWindowPeak = 0;
    
    //tracksAmount = 60;

    //pixelsPerTrack = SPECTROGRAM_WIDTH / tracksAmount;
    
    //imageLinePoint = 0;
    
    //samplesAfterStimulus = 0;
    
    startCallbacks(); 
    
    // Store pointer to processor
    processor = processor_pointer;

    

    
    
}


LfpLatencyProcessorVisualizer::~LfpLatencyProcessorVisualizer()
{
	processor = nullptr;
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
	// If channel still availaible, keep selection, otherwise no don't select anything
	// Trigger chanel combobox
	if (content.triggerChannelComboBox->getNumItems() <= last_triggerChannelId)
	{
		content.triggerChannelComboBox->setSelectedId(last_triggerChannelId);
	}
	else
	{
		content.triggerChannelComboBox->setSelectedId(0);
		processor->resetTriggerChannel();

	}
	// data channel combobox
	if (content.dataChannelComboBox->getNumItems() <= last_dataChannelID)
	{
		content.dataChannelComboBox->setSelectedId(last_dataChannelID);
	}
	else
	{
		content.triggerChannelComboBox->setSelectedId(0);
		processor->resetDataChannel();
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
	processor->changeParameter(5, content.rightMiddlePanel->getTriggerThresholdValue()); // pass channel Id -1 = channel index
	
	//Update spectrogram image
	updateSpectrogram();

	if (processor->checkEventReceived())
	{
		processor->resetEventFlag();
		processTrack();
		content.spikeTracker->visibilityChanged();
	}

    
   //Refresh canvas (redraw)
    refresh();

}


void LfpLatencyProcessorVisualizer::updateSpectrogram()
{
	content.spectrogramPanel->updateSpectrogram(*processor, content);
}

void LfpLatencyProcessorVisualizer::processTrack()
{

	
	// Get latency track data of previous row
	float* lastRowData = processor->getdataCacheRow(1);

	// HACK Get searchbox location in absolute units

	int searchBoxLocationAbs = content.startingSample + content.searchBoxLocation* content.subsamplesPerWindow;
	int searchBoxWidthAbs = content.searchBoxWidth * content.subsamplesPerWindow;

	// get spike magnitude
	float maxLevel = FloatVectorOperations::findMaximum(lastRowData + (searchBoxLocationAbs - searchBoxWidthAbs),
		searchBoxWidthAbs * 2 + content.subsamplesPerWindow);

	// get spike location
	int SpikeLocationAbs = std::max_element(lastRowData + (searchBoxLocationAbs - searchBoxWidthAbs),
		lastRowData + (searchBoxLocationAbs + searchBoxWidthAbs)) - lastRowData; // Note we substract lastRowData so that index starts at zero

	int SpikeLocationRel= (SpikeLocationAbs - content.startingSample) / content.subsamplesPerWindow;

	Array <bool> rowsSelected = getRow(*content.spikeTrackerContent, true, true);

	//Keep the spike/threshold location values updated, track them, and delete them if required
	for (int q = 0; q < 4; q++) {
		if (SL[q].isFull == true) 
		{
			updateSpikeInfo(q);
			updateInfo(*content.spikeTrackerContent , SL[q].SLR, SL[q].firingProbability, SL[q].bigStim, q);
		}
		else
		{
			updateSpikeInfo(q);
			updateInfo(*content.spikeTrackerContent, 0, 0.0f, SL[q].bigStim, q);
		}
		if (getSpikeSelect(*content.spikeTrackerContent, q) && SL[q].isFull) 
		{
			content.spikeTracker->selectedRowsChanged(q);
			setConfig(q);
			updateSpikeInfo(q);
			content.spectrogramPanel->setSearchBoxValue(SL[q].SLR);
			content.rightMiddlePanel->setROISpikeMagnitudeText(String(SL[q].MAXLEVEL, 1));
			content.rightMiddlePanel->setROISpikeLatencyText(String(SL[q].SLA / 30.0f, 1));
		}
		if (getThresholdSelect(*content.spikeTrackerContent, q) && SL[q].thresholdFull) 
		{
			updateSpikeInfo(q);
			content.stimulusVoltageSlider->setValue(SL[q].bigStim);
			content.ppControllerComponent->setStimulusVoltage(SL[q].bigStim);
		}
		if (getRowToDelete(*content.spikeTrackerContent, q)) 
		{
			deleteSpikeAndThreshold(*content.spikeTrackerContent, q);
			SL[q] = {};
			lastSearchBoxLocation = 0;
			SL[q].isFull = false;
			SL[q].thresholdFull = false;
			updateInfo(*content.spikeTrackerContent, 0, 0.0f, 0.0f, q);
			std::cout << "Spike " << q << " Deleted" << endl;
			content.rightMiddlePanel->setROISpikeMagnitudeText(String(0));
			content.rightMiddlePanel->setROISpikeLatencyText(String(0));
		}
	
		// If a row of the table is toggled, track any spikes found there
		if (rowsSelected[q]) 
		{

			// Check for spike inside ROI box
			if (maxLevel > content.detectionThreshold)
			{
				content.spikeDetected = true;

				content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);

				//Check if spike is a repeat based on last location, and make sure the current spikeinfo is empty
				if (lastSearchBoxLocation == content.searchBoxLocation) 
				{
					content.newSpikeDetected = false;
				}
				else  if (!SL[q].isFull) 
				{
					//int i = availableSpace[0];
					content.newSpikeDetected = true;
					cout << "Spike Found" << endl;
					SL[q].startingSample = content.startingSample;
					SL[q].searchBoxLocation = content.searchBoxLocation;
					SL[q].subsamples = content.subsamplesPerWindow;
					SL[q].searchBoxWidth = content.searchBoxWidth;
					SL[q].lastRowData = lastRowData;
					SL[q].isFull = true;
					lastSearchBoxLocation = content.searchBoxLocation;
					SL[q].firingNumbers.add(1);
					auto clock = Time::getCurrentTime();
					auto time = clock.toString(false, true, true, false);
					auto string_time = time.toStdString();
					processor->addSpike(string_time +
						" Current Sample Number: " +
						to_string(processor->currentSample) +
						" Current Track Number: " +
						to_string(processor->currentTrack) +
						" SPIKE FOUND Location: " +
						to_string(SL[q].searchBoxLocation) +
						" StartingSample: " +
						to_string(SL[q].startingSample) +
						" Subsamples per Window: " +
						to_string(SL[q].subsamples) +
						" Search Box Width: " +
						to_string(SL[q].searchBoxWidth));
					
					// If threshold column is toggled, start tracking the threshold
					if (rowsSelected[q + 4] == true && !SL[q].thresholdFull)
					{
						// Spike! Increase stimulation
						SL[q].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
						SL[q].bigStim = std::max(SL[q].stimVol, content.stimulusVoltageMin);
						SL[q].thresholdFull = true;
					}
				}

			}
			else
			{
				content.spikeDetected = false;
				content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);

			}
		}
		else if (rowsSelected[q + 4] == true && !SL[q].thresholdFull)
		{
			//No spike, increase stimulation
			SL[q].stimVol = content.stimulusVoltage + std::abs(content.trackSpike_IncreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
			SL[q].bigStim = std::min(SL[q].stimVol, content.stimulusVoltageMax);
			SL[q].thresholdFull = true;;
		}
	}
	
}

void LfpLatencyProcessorVisualizer::updateSpikeInfo(int i) 
{
	
	// If there is a spike being tracked, update data, using a new row of data from cache
	if (SL[i].isFull) 
	{
		SL[i].lastRowData = processor->getdataCacheRow(1);
		SL[i].SBLA = SL[i].startingSample + SL[i].searchBoxLocation * SL[i].subsamples;
		SL[i].SBWA = SL[i].searchBoxWidth * SL[i].subsamples;
		SL[i].MAXLEVEL = FloatVectorOperations::findMaximum(SL[i].lastRowData + (SL[i].SBLA - SL[i].SBWA), SL[i].SBWA * 2 + SL[i].subsamples);
		if (SL[i].MAXLEVEL > content.detectionThreshold) 
		{
			SL[i].firingNumbers.add(1);
			if (getThresholdSelect(*content.spikeTrackerContent, i)) 
			{
				if (!SL[i].thresholdFull)
				{
					SL[i].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate);
					SL[i].bigStim = std::max(SL[i].stimVol, content.stimulusVoltageMin);
					SL[i].thresholdFull = true;
				}
				else if (SL[i].thresholdFull)
				{
					SL[i].stimVol = SL[i].bigStim - std::abs(content.trackSpike_DecreaseRate);
					SL[i].bigStim = std::max(SL[i].stimVol, content.stimulusVoltageMin);
					SL[i].thresholdFull = true;
				}
			}
		}
		else if (SL[i].MAXLEVEL <= content.detectionThreshold && (SL[i].thresholdFull && getThresholdSelect(*content.spikeTrackerContent, i))) 
		{
			SL[i].firingNumbers.add(0);
			if (getThresholdSelect(*content.spikeTrackerContent, i))
			{
				if (!SL[i].thresholdFull) 
				{
					SL[i].stimVol = content.stimulusVoltage + std::abs(content.trackSpike_IncreaseRate);
					SL[i].bigStim = std::min(SL[i].stimVol, content.stimulusVoltageMax);
					SL[i].thresholdFull = true;
				}
				else if (SL[i].thresholdFull)
				{
					SL[i].stimVol = SL[i].bigStim + std::abs(content.trackSpike_IncreaseRate);
					SL[i].bigStim = std::min(SL[i].stimVol, content.stimulusVoltageMax);
					SL[i].thresholdFull = true;
				}
			}
		}
		SL[i].SLA = std::max_element(SL[i].lastRowData + (SL[i].SBLA - SL[i].SBWA), SL[i].lastRowData + (SL[i].SBLA + SL[i].SBWA)) - SL[i].lastRowData;
		SL[i].SLR = (SL[i].SLA - SL[i].startingSample) / SL[i].subsamples;
		auto clock = Time::getCurrentTime();
		auto time = clock.toString(false, true, true, false);
		auto string_time = time.toStdString();
		processor->addSpike(string_time + 
			" Spike " +
			to_string(i) +
			" Search Box Location Absolute: " +
			to_string(SL[i].SBLA) +
			" Search Box Location Width: " +
			to_string(SL[i].SBWA) +
			" Max Level: " +
			to_string(SL[i].MAXLEVEL));
		if (SL[i].firingNumbers.size() < content.stimuli) 
		{
			SL[i].firingNumbers.add(SL[i].firingNumber);
			SL[i].firingNumber = 0;
		}
		else if (SL[i].firingNumbers.size() >= content.stimuli) 
		{
			for (int x = 0; x < content.stimuli; x++)
			{
				SL[i].firingNumber += SL[i].firingNumbers[x];
			}
			SL[i].firingProbability = SL[i].firingNumber / content.stimuli;
			SL[i].firingNumber = 0;
			SL[i].firingNumbers.remove(0);
		}
	}
	else if (getThresholdSelect(*content.spikeTrackerContent, i)) 
	{
		if (!SL[i].thresholdFull)
		{
			SL[i].stimVol = content.stimulusVoltage + std::abs(content.trackSpike_IncreaseRate);
			SL[i].bigStim = std::min(SL[i].stimVol, content.stimulusVoltageMax);
		}
		else if (SL[i].thresholdFull)
		{
			SL[i].stimVol = SL[i].bigStim + std::abs(content.trackSpike_IncreaseRate);
			SL[i].bigStim = std::min(SL[i].stimVol, content.stimulusVoltageMax);
		}
	}

}

// Sets config to one used when spike was first found, TODO: Get rid of this, allow adjustment of settings while tracking?
void LfpLatencyProcessorVisualizer::setConfig(int i) 
{

	if (SL[i].isFull) 
	{
		content.spectrogramControlPanel->setStartingSampleValue(SL[i].startingSample);
		content.spectrogramControlPanel->setSubsamplesPerWindowValue(SL[i].subsamples);
		content.spectrogramPanel->setSearchBoxWidthValue(SL[i].searchBoxWidth);
	}

}


void LfpLatencyProcessorVisualizer::setParameter (int parameter, float newValue)
{
}


void LfpLatencyProcessorVisualizer::setParameter (int parameter, int val1, int val2, float newValue)
{
}

