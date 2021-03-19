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

#include <stdio.h>

#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorEditor.h"

//If the processor uses a custom editor, it needs its header to instantiate it
//#include "ExampleEditor.h"

LfpLatencyProcessor::LfpLatencyProcessor()
    : GenericProcessor("LfpLatency"), fifoIndex(0), eventReceived(false), samplesPerSubsampleWindow(60), samplesAfterStimulusStart(0)

{
    setProcessorType(PROCESSOR_TYPE_SINK);

    // Open Ephys Plugin Generator will insert generated code for parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SECTION_BEGIN]
    //[OPENEPHYS_PARAMETERS_SECTION_END]

    //Parameter controlling number of samples per subsample window
    //auto parameter0 = new Parameter ("detectionThreshold", 1, 4000, 1000, 0);
    //parameters.add (parameter0);

    // auto parameter1 = new Parameter ("highThresholdColor", 1.0f, 1000.0f, 300.0f, 0);
    //parameters.add (parameter1);
    //	auto parameter2 = new Parameter ("lowThresholdColor", 1.0f, 1000.0f, 60.0f, 0);
    //   parameters.add (parameter2);

    //Initialize array
    for (auto ii = 0; ii < DATA_CACHE_SIZE_TRACKS * DATA_CACHE_SIZE_SAMPLES; ii++)
    {
        dataCache[ii] = 0.0f;
    }

    //Initialize array
    for (auto ii = 0; ii < DATA_CACHE_SIZE_TRACKS; ii++)
    {
        spikeLocation[ii] = 0.0f;
    }

    currentTrack = 0;

    // Set default channels
    dataChannel_idx = 0;
    triggerChannel_idx = 0;

    // Set default stimulus threshold
    stimulus_threshold = 2.5f;
}

void LfpLatencyProcessor::timerCallback(int timerID)
{
    if (timerID == 1)
    {
        eventReceived = false;
        stopTimer(1);
    }
}

LfpLatencyProcessor::~LfpLatencyProcessor()
{
    //TODO: destructor
}

/**
  If the processor uses a custom editor, this method must be present.
*/
AudioProcessorEditor *LfpLatencyProcessor::createEditor()
{
    editor = new LfpLatencyProcessorEditor(this, true);

    //std::cout << "Creating editor." << std::endl;

    return editor;
}

void LfpLatencyProcessor::setParameter(int parameterIndex, float newValue)
{
    GenericProcessor::setParameter(parameterIndex, newValue);
    editor->updateParameterButtons(parameterIndex);

    // Save parameter value
    //TODO: use switch(parameterIndex) statement

    if (parameterIndex == 0)
    {
        peakThreshold = newValue;
        std::cout << "New value for peakThreshold: " << peakThreshold << std::endl;
    }
}

//NOTE NOT CURRENTLY USED! Events detected in process() instead
/**
void LfpLatencyProcessor::handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum)
{
    //If TTL event received
    if (Event::getEventType(event) == EventChannel::TTL)
    {
        //Deserialize (decode) event
        TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);

        const int eventState         = ttl->getState();
        const int eventChannel    = ttl->getChannel();

        //std::cout << "Received event on channel " << eventChannel
        //           << " with value " << eventState << std::endl;

        if (eventState == 5) //JumpLine
        {
            //Set flags
            eventReceived = true;
            fifoIndex = 0;
            lastWindowPeak = 0;
            windowSampleCount = 0;

            //clear buffer
            for (size_t i = 0; i < FIFO_BUFFER_SIZE;++i) {
    	        fifo[i] = 0.0;
            }

        }
    }
}
*/

void LfpLatencyProcessor::process(AudioSampleBuffer &buffer)
{
    int numChannels = buffer.getNumChannels();

    if (numChannels > 0 && (dataChannel_idx <= numChannels) && (triggerChannel_idx <= numChannels)) // Avoids crashing when no data source connected
    {
        // get num of samples in buffer
        int nSamples = getNumSamples(0);

        //Data channel
        const float *bufPtr = buffer.getReadPointer(dataChannel_idx);

        // Trigger channel
        const float *bufPtr_pulses = buffer.getReadPointer(triggerChannel_idx);

        // Debug channel, used to explore scalings
        //const float* bufPtr_test = buffer.getReadPointer(23);

        //For each sample in buffer
        for (auto n = 0; n < nSamples; ++n)
        {
            //Read sample from DATA buffer
            float data = *(bufPtr + n) * 1.0f;

            //Read sample from TRIGGER (ADC) buffer
            float data_pulses = *(bufPtr_pulses + n);

            //Read sample from Debug buffer
            //float test_pulses = *(bufPtr_test + n);

            //If data is above threshold, and no event received lately
            if (std::abs(data_pulses) > stimulus_threshold && !eventReceived)
            {
                //DEBUG
                //lastReceivedDACPulse=test_pulses;

                //Print to console
                //std::cout << "Peak with amplitude: " << data_pulses << std::endl;

                //Set flags
                eventReceived = true;
                // We have a pulse, start refactoery period timer
                startTimer(1, 200); //from 600

                //Reset fifo index (so that buffer overwrites
                fifoIndex = 0;
                currentSample = 0;

                //increment row count
                currentTrack < (DATA_CACHE_SIZE_TRACKS - 1) ? currentTrack++ : currentTrack = 0;

                //clear row
                for (auto ii = 0; ii < DATA_CACHE_SIZE_SAMPLES; ii++)
                {
                    dataCache[currentTrack * DATA_CACHE_SIZE_SAMPLES + ii] = 0.0f;
                }
            }

            if (currentSample < (DATA_CACHE_SIZE_SAMPLES))
            {

                dataCache[currentTrack * DATA_CACHE_SIZE_SAMPLES + currentSample] = 1.0f * std::abs(data);

                currentSample++;
            }
        }
    }
}

void LfpLatencyProcessor::saveCustomParametersToXml(XmlElement *parentElement)
{
	printf("Trying to save\n");
	
    XmlElement *mainNode = new XmlElement("LfpLatencyProcessor");
	printf("made the main node\n");
    // Open Ephys Plugin Generator will insert generated code to save parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_SAVE_SECTION_BEGIN]
	mainNode->addChildElement(parentElement);
	printf("Added the parent element\n");
	//XmlElement *TracksToXML = mainNode->createNewChildElement("Tracks");
	//uint64 timeStamp = getTimestamp(LfpLatencyProcessorVisualizerContentComponent.dataChannelComboBox->getSelectedId());
	//TracksToXML->setAttribute("Track1", timeStamp)
	// save file??

    //[OPENEPHYS_PARAMETERS_SAVE_SECTION_END]
}

void LfpLatencyProcessor::loadCustomParametersFromXml()
{
	printf("Trying to load\n");
    if (parametersAsXml == nullptr) // prevent double-loading
        return;

    // Open Ephys Plugin Generator will insert generated code to load parameters here. Don't edit this section.
    //[OPENEPHYS_PARAMETERS_LOAD_SECTION_BEGIN]
   forEachXmlChildElement(*parametersAsXml, mainNode)
    {
        if (mainNode->hasTagName("LfpLatencyProcessor"))
        {
            int parameterIdx = -1;
			// use parametersAsXml to restore state
            forEachXmlChildElement(*mainNode, parameterNode)
            {
                if (parameterNode->hasTagName("Components"))
                {
                    ++parameterIdx;
                   /* String parameterType = parameterNode->getStringAttribute("type");
                    if (parameterType == "Boolean")
                        setParameter(parameterIdx, parameterNode->getBoolAttribute("value"));
                    else if (parameterType == "Continuous" || parameterType == "Numerical")
                        setParameter(parameterIdx, parameterNode->getDoubleAttribute("value"));
                    else if (parameterType == "Discrete")
                        setParameter(parameterIdx, parameterNode->getIntAttribute("value"));*/
                }
            }
        }
    }
    //[OPENEPHYS_PARAMETERS_LOAD_SECTION_END]
}

bool LfpLatencyProcessor::checkEventReceived()
{
    return eventReceived;
}

void LfpLatencyProcessor::resetEventFlag()
{
    //eventReceived = false;
}

float *LfpLatencyProcessor::getdataCacheLastRow()
{
    float *rowPtr = (dataCache + currentTrack * DATA_CACHE_SIZE_SAMPLES);
    return rowPtr;
}

float *LfpLatencyProcessor::getdataCacheRow(int track)
{
    uint32_t colIndex = (((currentTrack - track) % DATA_CACHE_SIZE_TRACKS) + DATA_CACHE_SIZE_TRACKS) % DATA_CACHE_SIZE_TRACKS;

    float *rowPtr = (dataCache + colIndex * DATA_CACHE_SIZE_SAMPLES);

    return rowPtr;
}

int LfpLatencyProcessor::getLatencyData(int track)
{
    int data = spikeLocation[(((currentTrack - track) % DATA_CACHE_SIZE_TRACKS) + DATA_CACHE_SIZE_TRACKS) % DATA_CACHE_SIZE_TRACKS];
    return data;
}

void LfpLatencyProcessor::pushLatencyData(int latency)
{
    spikeLocation[currentTrack] = latency;
}

float *LfpLatencyProcessor::getdataCache()
{
    //float* rowPtr = (dataCache+currentTrack*DATA_CACHE_SIZE_SAMPLES);
    return dataCache;
}

void LfpLatencyProcessor::changeParameter(int parameterID, int value)
{
    if (parameterID == 1)
    {
        samplesPerSubsampleWindow = value;
    }
    if (parameterID == 2)
    {
        samplesAfterStimulusStart = value;
    }
    if (parameterID == 3)
    {
        // change current trigger chan
        if (value >= 0 && value < 25)
        {
            triggerChannel_idx = value;
        }
    }
    if (parameterID == 4)
    {
        // change current trigger chan
        if (value >= 0 && value < 25)
        {
            dataChannel_idx = value;
        }
    }
    if (parameterID == 5)
    {
        // change current strimulus detection threshold
        if (value >= 0)
        {
            stimulus_threshold = value;
        }
    }
}

int LfpLatencyProcessor::getParameterInt(int parameterID)
{
    int value = -1;

    if (parameterID == 1)
    {
        value = triggerChannel_idx;
    }
    if (parameterID == 2)
    {
        value = dataChannel_idx;
    }

    return value;
}

float LfpLatencyProcessor::getParameterFloat(int parameterID)
{
    float value = -1;

    if (parameterID == 1)
    {
        value = lastReceivedDACPulse;
    }
    return value;
}