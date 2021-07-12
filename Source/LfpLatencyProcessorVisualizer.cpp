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
		//processor->resetTriggerChannel();
	}
	// data channel combobox
	if (content.dataChannelComboBox->getNumItems() <= last_dataChannelID)
	{
		content.dataChannelComboBox->setSelectedId(last_dataChannelID);
	}
	else
	{
		content.triggerChannelComboBox->setSelectedId(0);
		//processor->resetDataChannel();
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

	//Keep the spike location values updated
	for (int q = 0; q < 4; q++) {
		if (spikeLocations[q].isFull == true) {
			updateSpikeInfo(q);
			tc.updateInfo(spikeLocations[q].SLR, spikeLocations[q].firingProbability, spikeLocations[q].bigStim, q);
			tc.spikeFound = true;
			std::cout << tc.info[q].location << "  " << tc.info[q].firingProb << endl;
			content.updateTable(q);
			content.spikeTracker->visibilityChanged();
			content.spikeTracker->updateContent();
			content.spikeTracker->repaint();
			//content.spikeTracker->getModel()->refreshComponentForCell(q, 2, false, )
			
			//content.locations[q]->setText(String(spikeLocations[q].SLR));
			//content.fps[q]->setText(String(spikeLocations[q].firingProbability));
			//content.ts[q]->setText(String(spikeLocations[q].bigStim));
		}
		/*if (content.deletes[q] == true) {
			content.follows[q]->setToggleState(false, sendNotification);
			spikeLocations[q] = {};
			spikeLocations[q].isFull = false;
			content.locations[q]->setText("0");
			content.fps[q]->setText("0");
			content.dels[q]->setToggleState(false, sendNotification);
			std::cout << "Spike " << q << " Deleted" << endl;
			content.deletes[q] = false;
			availableSpace.add(q);
			availableThresholdSpace.add(q);
		}
		if (content.follows[q]->getToggleState() == true) {
			for (int n = 0; n < 4; n++)
				if (n == q) continue;
				else content.follows[n]->setToggleState(false, sendNotification);
			content.trackSpike_button->setToggleState(false, sendNotification);
			content.spikeTracker->selectedRowsChanged(q);
			setConfig(q);
			updateSpikeInfo(q);
			content.spectrogramPanel->setSearchBoxValue(spikeLocations[q].SLR);
			content.rightMiddlePanel->setROISpikeMagnitudeText(String(spikeLocations[q].MAXLEVEL, 1));
			content.rightMiddlePanel->setROISpikeLatencyText(String(spikeLocations[q].SLA / 30.0f, 1));
		}
		if (content.thresholds[q]->getToggleState() == true) {
			for (int n = 0; n < 4; n++)
				if (n == q) continue;
				else content.thresholds[n]->setToggleState(false, sendNotification);
			content.trackThreshold_button->setToggleState(false, sendNotification);
			updateSpikeInfo(q);
			content.stimulusVoltageSlider->setValue(spikeLocations[q].bigStim);
			content.ppControllerComponent->setStimulusVoltage(spikeLocations[q].bigStim);
		}*/
	}

	//display values
	//content.rightMiddlePanel->setROISpikeMagnitudeText(String(maxLevel, 1));
	//content.rightMiddlePanel->setROISpikeLatencyText(String(SpikeLocationAbs / 30.0f, 1));

	// If we have enabled spike tracking the track spike
	if (content.trackSpike_button->getToggleState() == true) {

		// Check for spike inside ROI box
		if (maxLevel > content.detectionThreshold)
		{
			content.spikeDetected = true;
			content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);
			

			//Check if spike is a repeat based on last location, and make sure the current spikeinfo is empty
			if (lastSearchBoxLocation == content.searchBoxLocation) {
				content.newSpikeDetected = false;
			}
			else  if (!availableSpace.isEmpty()) {
				int i = availableSpace[0];
				content.newSpikeDetected = true;
				cout << "Spike Found" << endl;
				spikeLocations[i].startingSample = content.startingSample;
				spikeLocations[i].searchBoxLocation = content.searchBoxLocation;
				spikeLocations[i].subsamples = content.subsamplesPerWindow;
				spikeLocations[i].searchBoxWidth = content.searchBoxWidth;
				spikeLocations[i].lastRowData = lastRowData;
				spikeLocations[i].isFull = true;
				lastSearchBoxLocation = content.searchBoxLocation;
				spikeLocations[i].firingNumber++;
				spikeLocations[i].firingNumbers.add(spikeLocations[i].firingNumber);
						
				
				if (content.trackThreshold_button->getToggleState() == true && !availableThresholdSpace.isEmpty())
				{
					int p = availableThresholdSpace[0];
					spikeLocations[p].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
					spikeLocations[p].bigStim = std::max(spikeLocations[i].stimVol, content.stimulusVoltageMin);
					spikeLocations[p].thresholdFull = true;
					//content.stimulusVoltageSlider->setValue(spikeLocations[q].bigStim);
					//content.ppControllerComponent->setStimulusVoltage(spikeLocations[q].bigStim);
				}
				availableSpace.remove(0);
				availableThresholdSpace.remove(0);
			}
			if (availableSpace.isEmpty()) {
				cout << "NO SPACE FOR NEW SPIKE IN TABLE" << endl;
			}
			if (availableThresholdSpace.isEmpty()) {
				cout << "NO SPACE FOR THRESHOLD IN TABLE" << endl;
			}
			//Reset spike array counter when it reaches four, allows for new spikes to be found once old ones are deleted

		}
		else
		{
			content.spikeDetected = false;
			content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);

			// If we have enabled threshold tracking then update threshold:
			if (content.trackThreshold_button->getToggleState() == true && !availableThresholdSpace.isEmpty())
			{
				//No spike, increase stimulation
				int i = availableThresholdSpace[0];
				spikeLocations[i].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
				spikeLocations[i].bigStim = std::min(spikeLocations[i].stimVol, content.stimulusVoltageMax);
				spikeLocations[i].thresholdFull = true;
				availableThresholdSpace.remove(0);

			}
		}
	}
	
}

void LfpLatencyProcessorVisualizer::updateSpikeInfo(int i) {
	
	if (spikeLocations[i].isFull) {
		spikeLocations[i].lastRowData = processor->getdataCacheRow(1);
		spikeLocations[i].SBLA = spikeLocations[i].startingSample + spikeLocations[i].searchBoxLocation * spikeLocations[i].subsamples;
		spikeLocations[i].SBWA = spikeLocations[i].searchBoxWidth * spikeLocations[i].subsamples;
		spikeLocations[i].MAXLEVEL = FloatVectorOperations::findMaximum(spikeLocations[i].lastRowData + (spikeLocations[i].SBLA - spikeLocations[i].SBWA), spikeLocations[i].SBWA * 2 + spikeLocations[i].subsamples);
		if (spikeLocations[i].MAXLEVEL > content.detectionThreshold) {
			spikeLocations[i].firingNumber++;
		}
		spikeLocations[i].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate);
		spikeLocations[i].bigStim = std::max(spikeLocations[i].stimVol, content.stimulusVoltageMin);
		spikeLocations[i].SLA = std::max_element(spikeLocations[i].lastRowData + (spikeLocations[i].SBLA - spikeLocations[i].SBWA), spikeLocations[i].lastRowData + (spikeLocations[i].SBLA + spikeLocations[i].SBWA)) - spikeLocations[i].lastRowData;
		spikeLocations[i].SLR = (spikeLocations[i].SLA - spikeLocations[i].startingSample) / spikeLocations[i].subsamples;
		if (spikeLocations[i].firingNumbers.size() != content.stimuli) {
			spikeLocations[i].firingNumbers.add(spikeLocations[i].firingNumber);
		}
		else if (spikeLocations[i].firingNumbers.size() == content.stimuli) {
			spikeLocations[i].firingProbability = spikeLocations[i].firingNumbers[content.stimuli - 1] / content.stimuli;
			spikeLocations[i].firingNumbers.clear();
			spikeLocations[i].firingNumber = 0;
		}
	}
	else {
		spikeLocations[i].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate);
		spikeLocations[i].bigStim = std::min(spikeLocations[i].stimVol, content.stimulusVoltageMax);
	}

}

void LfpLatencyProcessorVisualizer::setConfig(int i) {
	
	if (spikeLocations[i].isFull) {
		content.spectrogramControlPanel->setStartingSampleValue(spikeLocations[i].startingSample);
		content.spectrogramControlPanel->setSubsamplesPerWindowValue(spikeLocations[i].subsamples);
		content.spectrogramPanel->setSearchBoxWidthValue(spikeLocations[i].searchBoxWidth);
	}

}


void LfpLatencyProcessorVisualizer::setParameter (int parameter, float newValue)
{
}


void LfpLatencyProcessorVisualizer::setParameter (int parameter, int val1, int val2, float newValue)
{
}

