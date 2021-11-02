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

LfpLatencyProcessorVisualizer::LfpLatencyProcessorVisualizer(LfpLatencyProcessor *processor_pointer) : content(processor_pointer)
{
	//m_contentLookAndFeel = new LOOKANDFEELCLASSNAME();
	//content.setLookAndFeel (m_contentLookAndFeel);
	addAndMakeVisible(&content);

	// Set Visualizer refresh rate
	refreshRate = 100; // 5 Hz default refresh rate

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
	content.setBounds(getLocalBounds());
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
	processor->changeParameter(3, content.triggerChannelComboBox->getSelectedId() - 1);	 // pass channel Id -1 = channel index
	processor->changeParameter(4, content.dataChannelComboBox->getSelectedId() - 1);	 // pass channel Id -1 = channel index
	processor->changeParameter(5, content.rightMiddlePanel->getTriggerThresholdValue()); // pass channel Id -1 = channel index

	// update spike tracking details
	content.stimulusVoltageSlider->setValue(processor->pulsePalController->getStimulusVoltage(), juce::NotificationType::dontSendNotification);
	content.stimulusVoltage_text->setText(std::to_string(content.stimulusVoltageSlider->getValue()));

	content.stimulusVoltageSlider->setMaxValue(processor->pulsePalController->getMaxStimulusVoltage(), juce::NotificationType::dontSendNotification);
	content.stimulusVoltageMax_text->setText(std::to_string(content.stimulusVoltageSlider->getMaxValue()));

	content.stimulusVoltageSlider->setMinValue(processor->pulsePalController->getMinStimulusVoltage(), juce::NotificationType::dontSendNotification);
	content.stimulusVoltageMin_text->setText(std::to_string(content.stimulusVoltageSlider->getMinValue()));

	content.trackSpike_DecreaseRate_Slider->setValue(processor->getTrackingDecreaseRate(), juce::NotificationType::dontSendNotification);
	content.trackSpike_IncreaseRate_Slider->setValue(processor->getTrackingIncreaseRate(), juce::NotificationType::dontSendNotification);
	//Update spectrogram image
	updateSpectrogram();
	content.spikeTracker->updateContent();
	std::ostringstream ss_ms_latency;
	ss_ms_latency << std::fixed << std::setprecision(2) << (content.getSearchBoxSampleLocation() * 1000) / processor->getSampleRate();
	content.rightMiddlePanel->setROISpikeLatencyText(ss_ms_latency.str());
	//content.rightMiddlePanel->setROISpikeMagnitudeText("NaN");
	int i = processor->getSelectedSpike();
	if (i >= 0)
	{
		// TODO: there should be a method that syncs the UI with the templateSpike.
		auto ts = processor->getSpikeGroup(i)->templateSpike;
		auto spikeVal = ts.spikeSampleLatency;

		content.setSearchBoxSampleLocation(spikeVal);
		content.spectrogramPanel->setSearchBoxWidthValue(ts.windowSize);
		content.spectrogramControlPanel->setDetectionThresholdValue(ts.threshold); //#TODO: this should be getter/setter
	}

	if (processor->checkEventReceived())
	{
		processor->resetEventFlag();
		//processTrack();
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

	// // Get latency track data of previous row
	// float *lastRowData = processor->getdataCacheRow(1);

	// // HACK Get searchbox location in absolute units

	// int searchBoxLocationAbs = content.startingSample + content.searchBoxLocation * content.subsamplesPerWindow;
	// int searchBoxWidthAbs = content.searchBoxWidth * content.subsamplesPerWindow;

	// // get spike magnitude
	// float maxLevel = FloatVectorOperations::findMaximum(lastRowData + (searchBoxLocationAbs - searchBoxWidthAbs),
	// 													searchBoxWidthAbs * 2 + content.subsamplesPerWindow);

	// // get spike location
	// int SpikeLocationAbs = std::max_element(lastRowData + (searchBoxLocationAbs - searchBoxWidthAbs),
	// 										lastRowData + (searchBoxLocationAbs + searchBoxWidthAbs)) -
	// 					   lastRowData; // Note we substract lastRowData so that index starts at zero

	// int SpikeLocationRel = (SpikeLocationAbs - content.startingSample) / content.subsamplesPerWindow;

	// Array<bool> rowsSelected = getRow(*content.spikeTrackerContent, true, true);

	// //Keep the spike/threshold location values updated, track them, and delete them if required
	// for (int q = 0; q < 4; q++)
	// {
	// 	if (SL[q].isFull == true)
	// 	{
	// 		updateSpikeInfo(q);
	// 		//updateInfo(*content.spikeTrackerContent, SL[q].SearchLocationRelative, SL[q].firingProbability, SL[q].bigStim, q);
	// 	}
	// 	else
	// 	{
	// 		updateSpikeInfo(q);
	// 		updateInfo(*content.spikeTrackerContent, 0, 0.0f, SL[q].bigStim, q);
	// 	}
	// 	if (getSpikeSelect(*content.spikeTrackerContent, q) && SL[q].isFull)
	// 	{
	// 		content.spikeTracker->selectedRowsChanged(q);
	// 		setConfig(q);
	// 		updateSpikeInfo(q);
	// 		content.spectrogramPanel->setSearchBoxValue(SL[q].SearchLocationRelative);
	// 		content.rightMiddlePanel->setROISpikeMagnitudeText(String(SL[q].MAXLEVEL, 1));
	// 		content.rightMiddlePanel->setROISpikeLatencyText(String(SL[q].SearchLocationAbsolute / 30.0f, 1));
	// 	}
	// 	if (getThresholdSelect(*content.spikeTrackerContent, q) && SL[q].thresholdFull)
	// 	{
	// 		updateSpikeInfo(q);
	// 		content.stimulusVoltageSlider->setValue(SL[q].bigStim);
	// 		content.ppControllerComponent->setStimulusVoltage(SL[q].bigStim);
	// 	}
	// 	if (getRowToDelete(*content.spikeTrackerContent, q))
	// 	{
	// 		// TODO: this should be performed by the UI rather than checking if delete is held.
	// 		deleteSpikeAndThreshold(*content.spikeTrackerContent, q);
	// 		SL[q] = {};
	// 		lastSearchBoxLocation = 0;
	// 		SL[q].isFull = false;
	// 		SL[q].thresholdFull = false;
	// 		updateInfo(*content.spikeTrackerContent, 0, 0.0f, 0.0f, q);
	// 		std::cout << "Spike " << q << " Deleted" << endl;
	// 		content.rightMiddlePanel->setROISpikeMagnitudeText(String(0));
	// 		content.rightMiddlePanel->setROISpikeLatencyText(String(0));
	// 	}

	// 	// If a row of the table is toggled, track any spikes found there
	// 	if (rowsSelected[q])
	// 	{

	// 		// Check for spike inside ROI box
	// 		if (maxLevel > content.detectionThreshold)
	// 		{
	// 			content.spikeDetected = true;

	// 			content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);

	// 			//Check if spike is a repeat based on last location, and make sure the current spikeinfo is empty
	// 			if (lastSearchBoxLocation == content.searchBoxLocation)
	// 			{
	// 				content.newSpikeDetected = false;
	// 			}
	// 			else if (!SL[q].isFull)
	// 			{
	// 				//int i = availableSpace[0];
	// 				content.newSpikeDetected = true;
	// 				cout << "Spike Found" << endl;
	// 				SL[q].startingSample = content.startingSample;
	// 				SL[q].searchBoxLocation = content.searchBoxLocation;
	// 				SL[q].subsamples = content.subsamplesPerWindow;
	// 				SL[q].searchBoxWidth = content.searchBoxWidth;
	// 				SL[q].lastRowData = lastRowData;
	// 				SL[q].isFull = true;
	// 				lastSearchBoxLocation = content.searchBoxLocation;
	// 				SL[q].firingNumbers.add(1);
	// 				auto clock = Time::getCurrentTime();
	// 				auto time = clock.toString(false, true, true, false);
	// 				auto string_time = time.toStdString();
	// 				processor->addSpike(string_time +
	// 									" Current Sample Number: " +
	// 									to_string(processor->currentSample) +
	// 									" Current Track Number: " +
	// 									to_string(processor->currentTrack) +
	// 									" SPIKE FOUND Location: " +
	// 									to_string(SL[q].searchBoxLocation) +
	// 									" StartingSample: " +
	// 									to_string(SL[q].startingSample) +
	// 									" Subsamples per Window: " +
	// 									to_string(SL[q].subsamples) +
	// 									" Search Box Width: " +
	// 									to_string(SL[q].searchBoxWidth));

	// 				// If threshold column is toggled, start tracking the threshold
	// 				if (rowsSelected[q + 4] == true && !SL[q].thresholdFull)
	// 				{
	// 					// Spike! Increase stimulation
	// 					SL[q].stimVol = content.stimulusVoltage - std::abs(content.trackSpike_DecreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
	// 					SL[q].bigStim = std::max(SL[q].stimVol, content.stimulusVoltageMin);
	// 					SL[q].thresholdFull = true;
	// 				}
	// 			}
	// 		}
	// 		else
	// 		{
	// 			content.spikeDetected = false;
	// 			content.spectrogramPanel->spikeIndicatorTrue(content.spikeDetected);
	// 		}
	// 	}
	// 	else if (rowsSelected[q + 4] == true && !SL[q].thresholdFull)
	// 	{
	// 		//No spike, increase stimulation
	// 		SL[q].stimVol = content.stimulusVoltage + std::abs(content.trackSpike_IncreaseRate); //call with abs since rate does not have sign. Avoids fat finger error
	// 		SL[q].bigStim = std::min(SL[q].stimVol, content.stimulusVoltageMax);
	// 		SL[q].thresholdFull = true;
	// 		;
	// 	}
	// }
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

void LfpLatencyProcessorVisualizer::setParameter(int parameter, float newValue)
{
}

void LfpLatencyProcessorVisualizer::setParameter(int parameter, int val1, int val2, float newValue)
{
}
