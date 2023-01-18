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
	// m_contentLookAndFeel = new LOOKANDFEELCLASSNAME();
	// content.setLookAndFeel (m_contentLookAndFeel);
	addAndMakeVisible(&content);

	// Set Visualizer refresh rate
	refreshRate = 100; // 5 Hz default refresh rate


	startCallbacks();

	// Store pointer to processor
	processor = processor_pointer;
}

LfpLatencyProcessorVisualizer::~LfpLatencyProcessorVisualizer()
{
	processor = nullptr;
	stopCallbacks(); // MM For the time being...
}

void LfpLatencyProcessorVisualizer::resized()
{
	std::cout << "LfpLatencyProcessorVisualizer::resized" << std::endl;
	content.setBounds(getLocalBounds());
}

void LfpLatencyProcessorVisualizer::refreshState()
{
}

void LfpLatencyProcessorVisualizer::update()
{

	// Get number of availiable channels and update label
	int numAvailiableChannels = processor->getTotalContinuousChannels();//processor->getTotalDataChannels();

	std::cout << "LfpLatencyProcessorVisualizer::numAvailiableChannels" << numAvailiableChannels << std::endl;

	// Populate combobox with new channels, keep current selection if availiable
	//  get current selection
	int last_triggerChannelId = content.triggerChannelComboBox->getSelectedId();
	int last_dataChannelID = content.dataChannelComboBox->getSelectedId();

	// Clear old values and repopulate combobox
	content.triggerChannelComboBox->clear();
	content.dataChannelComboBox->clear();

	content.triggerChannelComboBox->addSectionHeading("Trigger");
	content.dataChannelComboBox->addSectionHeading("Data");

	for (int ii = 0; ii < numAvailiableChannels; ii++)
	{
		auto datastream = processor->getContinuousChannel(ii);
		content.triggerChannelComboBox->addItem(datastream->getName(),ii+1);
		content.dataChannelComboBox->addItem(datastream->getName(),ii+1);
		// if (processor->getDataStream(ii)->get->getChannelType() == DataChannel::HEADSTAGE_CHANNEL)
		// {
		// 	content.triggerChannelComboBox->addItem("CH" + String(ii + 1), ii + 1);
		// 	content.dataChannelComboBox->addItem("CH" + String(ii + 1), ii + 1);
		// }
		// else if (processor->getDataChannel(ii)->getChannelType() == DataChannel::ADC_CHANNEL)
		// {
		// 	content.triggerChannelComboBox->addItem("ADC" + String(ii + 1), ii + 1);
		// 	//content.dataChannelComboBox->addItem("ADC" + String(ii + 1), ii + 1);
		// }
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
	repaint();
}

void LfpLatencyProcessorVisualizer::beginAnimation()
{
	startCallbacks();
}

void LfpLatencyProcessorVisualizer::endAnimation()
{
	stopCallbacks();
}

void LfpLatencyProcessorVisualizer::timerCallback()
{

	// #TODO: properly define the parameters (for easy save/load)
	// #TODO: Why is this calle deach time the page refreshes?
	// #TODO: move away from this methodology.
	
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
	// Update spectrogram image
	updateSpectrogram();
	content.spikeTracker->updateContent();
	std::ostringstream ss_ms_latency;
	ss_ms_latency << std::fixed << std::setprecision(2) << (content.getSearchBoxSampleLocation() * 1000) / CoreServices::getGlobalSampleRate();
	content.rightMiddlePanel->setROISpikeLatencyText(ss_ms_latency.str());
	// content.rightMiddlePanel->setROISpikeMagnitudeText("NaN");
	int i = processor->getSelectedSpike();
	if (i >= 0)
	{
		// TODO: there should be a method that syncs the UI with the templateSpike.
		auto ts = processor->getSpikeGroup(i)->templateSpike;
		auto spikeVal = ts.spikeSampleLatency;

		content.setSearchBoxSampleLocation(spikeVal);
		
		content.spectrogramControlPanel->setSearchBoxWidthValue(ts.windowSize);
		content.spectrogramControlPanel->setDetectionThresholdValue(ts.threshold); //#TODO: this should be getter/setter
	}

	if (processor->checkEventReceived())
	{
		processor->resetEventFlag();
		// processTrack();
		content.spikeTracker->visibilityChanged();
	}

	// Refresh canvas (redraw)
	refresh();
}

void LfpLatencyProcessorVisualizer::updateSpectrogram()
{
	content.spectrogramPanel->updateSpectrogram(*processor, content);
}


// Sets config to one used when spike was first found, TODO: Get rid of this, allow adjustment of settings while tracking?
void LfpLatencyProcessorVisualizer::setConfig(int i)
{

	if (SL[i].isFull)
	{
		content.spectrogramControlPanel->setStartingSampleValue(SL[i].startingSample);
		content.spectrogramControlPanel->setSubsamplesPerWindowValue(SL[i].subsamples);
		content.spectrogramControlPanel->setSearchBoxWidthValue(SL[i].searchBoxWidth);
	}
}

void LfpLatencyProcessorVisualizer::loadCustomParametersFromXml(XmlElement* xml) { }