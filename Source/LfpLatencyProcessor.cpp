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

#include <stdio.h>
#include <string>
#include <mutex>
#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorEditor.h"
#include "LfpLatencySpectrogramControlPanel.h"
// #include "/modules/juce_core/files/juce_File.h"
// #include "C:\\Users\\gsboo\\source\\repos\\plugin-GUI\\JuceLibraryCode\\modules\\juce_core\\misc\\juce_Result.h"
#include <map>
#include <vector>

// If the processor uses a custom editor, it needs its header to instantiate it
// #include "ExampleEditor.h"

std::mutex savingAndLoadingLock;

LfpLatencyProcessor::LfpLatencyProcessor()
    : GenericProcessor("APTrack"), fifoIndex(0), eventReceived(false), samplesPerSubsampleWindow(60), samplesAfterStimulusStart(0), messages(),
      spikeGroups(0), dataCache((DATA_CACHE_SIZE_TRACKS + 1) * DATA_CACHE_SIZE_SAMPLES)

{
    pulsePalController = new ppController(this);
    spikeGroups.reserve(100);
    dataCacheTimestamps.reserve(3000); // resrve 3k timestamps

    // Parameter controlling number of samples per subsample window
    // auto parameter0 = new Parameter ("detectionThreshold", 1, 4000, 1000, 0);
    // parameters.add (parameter0);

    // auto parameter1 = new Parameter ("highThresholdColor", 1.0f, 1000.0f, 300.0f, 0);
    // parameters.add (parameter1);
    //	auto parameter2 = new Parameter ("lowThresholdColor", 1.0f, 1000.0f, 60.0f, 0);
    //   parameters.add (parameter2);

    // Initialize array
    for (auto ii = 0; ii < DATA_CACHE_SIZE_TRACKS * DATA_CACHE_SIZE_SAMPLES; ii++)
    {
        dataCache[ii] = 0.0f;
    }

    // Initialize array
    for (auto ii = 0; ii < DATA_CACHE_SIZE_TRACKS; ii++)
    {
        spikeLocation[ii] = 0.0f;
    }

    currentTrack = 0; // currentTrack increments before adding first row.

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
}

void LfpLatencyProcessor::resetDataChannel()
{
    dataChannel_idx = 0;
}

void LfpLatencyProcessor::resetTriggerChannel()
{
    triggerChannel_idx = 0;
}

/**
  If the processor uses a custom editor, this method must be present.
*/
AudioProcessorEditor *LfpLatencyProcessor::createEditor()
{
    editor = std::make_unique<LfpLatencyProcessorEditor>(this, true);

    // std::cout << "Creating editor." << std::endl;

    return editor.get();
}
void LfpLatencyProcessor::addMessage(std::string message)
{
    messages.push(message);
}

void LfpLatencyProcessor::addSpike(std::string spike)
{
    spikes.push(spike);
}

// TODO: remove these. stimulusVoltage is handled by ppController
float LfpLatencyProcessor::getStimulusVoltage()
{
    return stimulusVoltage;
}
void LfpLatencyProcessor::setStimulusVoltage(float sv)
{
    stimulusVoltage = sv;
}

void LfpLatencyProcessor::updateSettings()
{
    createEventChannels();
}
// create event channel for pulsepal
void LfpLatencyProcessor::createEventChannels()
{

    if (getNumDataStreams() == 0)
    {
        return;
    }

    // #TODO: re-enable - v0.6 genericprocessor breaks custom text streams. see https://github.com/open-ephys/plugin-GUI/issues/547
    // pulsePalEventPtr->addProcessor(processorInfo.get());
    //  eventChannels.add(new EventChannel(
    //      (EventChannel::Settings){
    //          .type=EventChannel::Type::TEXT,
    //          .name="PulsePal Messages",
    //          .identifier="pulsepal.event",
    //          .stream = dataStreams.getLast()
    //      }
    //      ));
    // pulsePalEventPtr = eventChannels.getLast();

    // spikeEventPtr->addProcessor(processorInfo.get());
    //  eventChannels.add(new EventChannel(
    //      (EventChannel::Settings){
    //          .type=EventChannel::Type::TEXT,
    //          .name="Spike Data",
    //          .description="Details of spikes found",
    //          .identifier="spike.event",
    //          .stream = dataStreams.getLast()
    //      }
    //      ));

    // spikeEventPtr = eventChannels.getLast();
    // spikeEventPtr->addProcessor(processorInfo.get());
}

// create chanel for storing spike data
// void LfpLatencyProcessor::createSpikeChannels() {

// pikeChannel* spikechan = new SpikeChannel(SpikeChannel::typeFromNumChannels(), this);
//  SpikeEvent::SpikeBuffer buf = SpikeEvent::SpikeBuffer::SpikeBuffer(spikechan);
// }

// void LfpLatencyProcessor::createSpikeChannels(){
// SpikeChannel* spikes = new SpikeChannel()
// }

void LfpLatencyProcessor::setParameter(int parameterIndex, float newValue)
{
}

// NOTE NOT CURRENTLY USED! Events detected in process() instead
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
void LfpLatencyProcessor::addSpikeGroup(SpikeInfo templateSpike, bool isSelected)
{
    SpikeGroup s = {};
    s.templateSpike = templateSpike;
    s.spikeHistory.reserve(100);
    spikeGroups.push_back(s);
    if (isSelected)
        setSelectedSpike(spikeGroups.size() - 1);
};
void LfpLatencyProcessor::removeSpikeGroup(int i){

    // spikeGroups.erase(std::advance(spikeGroups.begin(),i));
};
SpikeGroup *LfpLatencyProcessor::getSpikeGroup(int i)
{
    const std::lock_guard<std::mutex> lock(spikeGroups_mutex);
    return &spikeGroups[i];
}
int LfpLatencyProcessor::getSpikeGroupCount()
{
    return spikeGroups.size();
}

int LfpLatencyProcessor::getSelectedSpike()
{
    for (int i = 0; i < spikeGroups.size(); i++)
    {
        if (spikeGroups[i].isActive)
        {
            return i;
        }
    }

    return -1;
}

int LfpLatencyProcessor::getTrackingSpike()
{
    for (int i = 0; i < spikeGroups.size(); i++)
    {
        if (spikeGroups[i].isTracking)
        {
            return i;
        }
    }

    return -1;
}
void LfpLatencyProcessor::setSelectedSpike(int i)
{
    // Set the current selected spike. Only one allowed.
    for (int x = 0; x < this->getSpikeGroupCount(); x++)
    {
        auto sg = this->getSpikeGroup(x);
        sg->isActive = x == i;
    }
}

void LfpLatencyProcessor::setSelectedSpikeLocation(int loc)
{
    int i = getSelectedSpike();
    if (i == -1)
    {
        return;
    }
    spikeGroups[i].templateSpike.spikeSampleLatency = loc;
}

void LfpLatencyProcessor::setSelectedSpikeThreshold(float val)
{
    int i = getSelectedSpike();
    if (i == -1)
    {
        return;
    }
    spikeGroups[i].templateSpike.threshold = val;
}

void LfpLatencyProcessor::setSelectedSpikeWindow(int window)
{
    int i = getSelectedSpike();
    if (i == -1)
    {
        return;
    }
    spikeGroups[i].templateSpike.windowSize = window;
}
void LfpLatencyProcessor::setTrackingSpike(int i)
{
    // Set the current tracking spike. Only one allowed.
    for (int x = 0; x < this->getSpikeGroupCount(); x++)
    {
        auto sg = this->getSpikeGroup(x);
        sg->isTracking = x == i;
    }
}

void LfpLatencyProcessor::trackThreshold()
{
    // if the threshold is being tracked
}

float LfpLatencyProcessor::getTrackingIncreaseRate()
{
    return trackingIncreaseRate;
}
void LfpLatencyProcessor::setTrackingIncreaseRate(float sv)
{
    trackingIncreaseRate = sv;
}
float LfpLatencyProcessor::getTrackingDecreaseRate()
{
    return trackingDecreaseRate;
}
void LfpLatencyProcessor::setTrackingDecreaseRate(float sv)
{
    trackingDecreaseRate = sv;
}

void LfpLatencyProcessor::trackSpikes()
{
    const std::lock_guard<std::mutex> lock(spikeGroups_mutex);
    for (int i = 0; i < spikeGroups.size(); i++)
    {
        auto &curSpikeGroup = spikeGroups[i];
        auto &templateSpike = curSpikeGroup.templateSpike;
        if (currentSample != templateSpike.spikeSampleLatency + templateSpike.windowSize ||
            (!curSpikeGroup.spikeHistory.empty() && curSpikeGroup.spikeHistory.back().trackIndex == currentTrack))
        {
            continue;
        }
        auto curTrackBufferLoc = (currentTrack % DATA_CACHE_SIZE_TRACKS) * DATA_CACHE_SIZE_SAMPLES; // /TODO: this should take the current read buffer
        auto windowStartInBuffer = curTrackBufferLoc + templateSpike.spikeSampleLatency - templateSpike.windowSize;
        auto startPtr = dataCache.data() + windowStartInBuffer;
        bool spikeDetected = false;
        auto maxValInWindow = std::max_element(startPtr, startPtr + (2 * templateSpike.windowSize));
        if (*maxValInWindow < templateSpike.threshold) // if there is a value > threshold
        {
            // spike **not** detected
            curSpikeGroup.recentHistory.push_back(false); // add to array
            curSpikeGroup.recentHistory.pop_front();
        }
        else
        {
            // spike detected
            SpikeInfo newSpike = {};
            newSpike.spikeSampleLatency = (maxValInWindow - (dataCache.data() + curTrackBufferLoc)); // scary pointer arithmetic. find the position in array of the max - start of current track
            newSpike.windowSize = templateSpike.windowSize;
            newSpike.threshold = templateSpike.threshold;
            newSpike.stimulusVoltage = this->pulsePalController->getStimulusVoltage();
            newSpike.spikePeakValue = *maxValInWindow;
            newSpike.spikeSampleNumber = this->dataCacheTimestamps[currentTrack] + newSpike.spikeSampleLatency;
            newSpike.trackIndex = currentTrack;
            curSpikeGroup.spikeHistory.push_back(newSpike);
            spikeGroups[i].templateSpike.spikeSampleLatency = newSpike.spikeSampleLatency;
            curSpikeGroup.recentHistory.push_back(true);
            curSpikeGroup.recentHistory.pop_front();
            spikeDetected = true;

            // #TODO: move this to its own function
            stringstream json_out;
            auto s = &newSpike;
            json_out << "{"
                     << "\"spikeSampleLatency\":" << s->spikeSampleLatency
                     << ", \"windowSize\":" << s->windowSize
                     << ", \"threshold\":" << s->threshold
                     << ", \"stimulusVoltage\":" << s->stimulusVoltage
                     << ", \"spikePeakValue\":" << s->spikePeakValue
                     << ", \"spikeSampleNumber\":" << s->spikeSampleNumber
                     << ", \"trackIndex\":" << s->trackIndex
                     << ", \"spikeGroup\":" << i
                     << "}";
            // #TODO: re-enable - v0.6 genericprocessor breaks custom text streams. see https://github.com/open-ephys/plugin-GUI/issues/547
            //  TextEventPtr event = TextEvent::createTextEvent(spikeEventPtr, s->spikeSampleNumber, json_out.str());
            //  addEvent(event, s->spikeSampleLatency);
            broadcastMessage(json_out.str());
        }
        if (curSpikeGroup.isTracking) // threshold tracking
        {
            float sv = this->pulsePalController->getStimulusVoltage();
            if (spikeDetected)
            {
                // request increase
                this->pulsePalController->setStimulusVoltage(sv - trackingDecreaseRate);
            }
            else
            {
                this->pulsePalController->setStimulusVoltage(sv + trackingIncreaseRate);
                // request decrease
            }

            // #TODO: update 50pct threshold if at 50pct
            float curPct = std::count(curSpikeGroup.recentHistory.begin(), curSpikeGroup.recentHistory.end(), true);

            if (curPct == (int)(curSpikeGroup.recentHistory.size() / 2) && curSpikeGroup.spikeHistory.size() >= curSpikeGroup.recentHistory.size())
            {
                float o = 0;
                for (int i = 0; i < curSpikeGroup.recentHistory.size(); i++)
                {
                    SpikeInfo sp = curSpikeGroup.spikeHistory[curSpikeGroup.spikeHistory.size() - i - 1];
                    o += sp.stimulusVoltage;
                }
                o = o / curSpikeGroup.recentHistory.size();
                curSpikeGroup.stimulusVoltage50pct = o;
            }
        }

        // send message on digital channel
    }
}
void LfpLatencyProcessor::process(AudioSampleBuffer &buffer)
{
    int numChannels = buffer.getNumChannels();

    if ((numChannels < 0) || (numChannels <= dataChannel_idx) || (numChannels <= triggerChannel_idx)) // Avoids crashing when no data source connected
    {
        return;
    }

    auto ts = this->getFirstSampleNumberForBlock(getDataStreams()[0]->getStreamId());
    // get num of samples in buffer

    int nSamples = this->getNumSamplesInBlock(getDataStreams()[0]->getStreamId()); // buffer.getNumSamples(); // getNumSamples(dataChannel_idx);

    // Data channel
    const float *bufPtr = buffer.getReadPointer(dataChannel_idx);

    // Trigger channel
    const float *bufPtr_pulses = buffer.getReadPointer(triggerChannel_idx);

    // Debug channel, used to explore scalings
    // const float* bufPtr_test = buffer.getReadPointer(23);
    // float bitvolts = getDataStreams()[0]->getContinuousChannels()[dataChannel_idx]->getGlobalIndex();

    // For each sample in buffer
    for (auto n = 0; n < nSamples; ++n)
    {
        // Read sample from DATA buffer
        float data = *(bufPtr + n) * 1.0f;

        // the value is stored in V for ADC channels (https://open-ephys.github.io/gui-docs/Developer-Guide/Open-Ephys-Plugin-API/Processor-Plugins.html)

        // Read sample from TRIGGER (ADC) buffer
        float data_pulses = *(bufPtr_pulses + n);

        // Read sample from Debug buffer
        // float test_pulses = *(bufPtr_test + n);

        // If data is above threshold, and no event received lately
        if (std::abs(data_pulses) > stimulus_threshold && !eventReceived) // this detects the current tracked spike
        {

            // TODO: this is an antipattern. We should get the global timestamp instead.
            // Set flags
            eventReceived = true;
            // We have a pulse, start refactoy period timer
            startTimer(1, 200); // from 600

            // Reset fifo index (so that buffer overwrites
            fifoIndex = 0;
            currentSample = 0;
            // increment row count
            currentTrack++;

            // clear row
            for (auto ii = 0; ii < DATA_CACHE_SIZE_SAMPLES; ii++)
            {
                dataCache[(currentTrack % DATA_CACHE_SIZE_TRACKS) * DATA_CACHE_SIZE_SAMPLES + ii] = 0.0f;
            }
            // write the timestamps
            dataCacheTimestamps.push_back(ts + n); // #TODO: need to check this aligns with the datacache currentTrack
        }

        if (currentSample < (DATA_CACHE_SIZE_SAMPLES))
        {

            dataCache[(currentTrack % DATA_CACHE_SIZE_TRACKS) * DATA_CACHE_SIZE_SAMPLES + currentSample] = 1.0f * std::abs(data);

            currentSample++;
        }
        trackSpikes(); // TODO: trackSpikes does not use the correct sample number for the message
    }

    trackThreshold();
    while (!messages.empty())
    { // post pulsePal messages
        // #TODO: re-enable - v0.6 genericprocessor breaks custom text streams. see https://github.com/open-ephys/plugin-GUI/issues/547
        // TextEventPtr event = TextEvent::createTextEvent(pulsePalEventPtr, CoreServices::getGlobalTimestamp(), messages.front());
        //  addEvent(event,0);
        // addEvent(pulsePalEventPtr, event, 0);
        broadcastMessage(messages.front());
        messages.pop();
    }
    while (!spikes.empty())
    { // post Spike detected events
        // # TODO: re-enable
        // TextEventPtr event = TextEvent::createTextEvent(spikeEventPtr, CoreServices::getGlobalTimestamp(), spikes.front());
        // addEvent(event, 0);
        broadcastMessage(spikes.front());
        spikes.pop();
    }
}

void LfpLatencyProcessor::saveRecoveryData(std::unordered_map<std::string, juce::String> *valuesMap)
{
    savingAndLoadingLock.lock();
    std::cout << "Trying to save " << std::endl;
    File recoveryConfigFile = CoreServices::getSavedStateDirectory().getChildFile("LastLfpLatencyPluginComponents.cfg");
    std::string cfgText = "";

    std::unordered_map<std::string, juce::String>::iterator it = valuesMap->begin();
    while (it != valuesMap->end())
    {
        cfgText += it->first + "[" + it->second.toStdString() + "]\n";
        it++;
    }

    FileOutputStream output(recoveryConfigFile);

    if (!output.openedOk())
    {
        std::cout << "recoveryConfigFile did not open correctly" << std::endl;
    }
    else
    {
        output.setPosition(0);
        output.truncate();
        output.setNewLineString("\n");
        output.writeText(cfgText, false, false, "\n");
        output.flush();

        if (output.getStatus().failed())
        {
            std::cout << "Error with FileOutoutStream and recoveryConfigFile" << std::endl;
        }
    }

    savingAndLoadingLock.unlock();
}

void LfpLatencyProcessor::loadRecoveryData(std::unordered_map<std::string, juce::String> *valuesMap)
{
    bool load = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Load LfpLatency Configurations?", "Would you like to load previous Lfp Latency Configurations?", "Yes", "No");

    if (!load)
    {
        return;
    }

    savingAndLoadingLock.lock();
    std::cout << "Trying to load " << std::endl;
    File recoveryConfigFile = CoreServices::getSavedStateDirectory().getChildFile("LastLfpLatencyPluginComponents.cfg");

    if (recoveryConfigFile.existsAsFile())
    {
        FileInputStream input(recoveryConfigFile);

        if (!input.openedOk())
        {
            std::cout << "LastLfpLatencyPluginComponents failed to open" << std::endl;
        }
        else
        {
            while (!input.isExhausted())
            {
                juce::String line = input.readNextLine();

                int firstBracket = line.indexOf("[");
                int secondBracket = line.indexOf("]");

                if (!(firstBracket <= 0 || secondBracket <= 0 || firstBracket > secondBracket))
                {
                    std::string itemName = line.substring(0, firstBracket).toStdString();
                    juce::String value = line.substring(firstBracket + 1, secondBracket);

                    (*valuesMap)[itemName] = value;
                }
            }
        }
    }
    else
    {
        std::cout << "Failed to find LastLfpLatencyPluginComponents" << std::endl;
    }

    savingAndLoadingLock.unlock();
}

void LfpLatencyProcessor::saveCustomParametersToXml(XmlElement *parentElement)
{
    // #TODO: re-enable
    // XmlElement *mainNode = parentElement->createNewChildElement("LfpLatencyProcessor");
    // mainNode->setAttribute("numParameters", getNumParameters());

    // for (int i = 0; i < getNumParameters(); ++i)
    // {
    //     XmlElement *parameterNode = mainNode->createNewChildElement("Parameter");

    //     auto parameter = getParameterObject(i);
    //     parameterNode->setAttribute("name", parameter->getName());
    //     parameterNode->setAttribute("type", parameter->getParameterTypeString());

    //     auto parameterValue = getParameterVar(i, currentChannel);

    //     if (parameter->isBoolean())
    //         parameterNode->setAttribute("value", (int)parameterValue);
    //     else if (parameter->isContinuous() || parameter->isDiscrete() || parameter->isNumerical())
    //         parameterNode->setAttribute("value", (double)parameterValue);
    // }
}

void LfpLatencyProcessor::loadCustomParametersFromXml(XmlElement *customParamsXml)
{
}
//     if (parametersAsXml == nullptr) // prevent double-loading
//         return;

//     // use parametersAsXml to restore state

//     forEachXmlChildElement(*parametersAsXml, mainNode)
//     {
//         if (mainNode->hasTagName("LfpLatencyProcessor"))
//         {
//             int parameterIdx = -1;

//             forEachXmlChildElement(*mainNode, parameterNode)
//             {
//                 if (parameterNode->hasTagName("Parameter"))
//                 {
//                     ++parameterIdx;

//                     String parameterType = parameterNode->getStringAttribute("type");
//                     if (parameterType == "Boolean")
//                         setParameter(parameterIdx, parameterNode->getBoolAttribute("value"));
//                     else if (parameterType == "Continuous" || parameterType == "Numerical")
//                         setParameter(parameterIdx, parameterNode->getDoubleAttribute("value"));
//                     else if (parameterType == "Discrete")
//                         setParameter(parameterIdx, parameterNode->getIntAttribute("value"));
//                 }
//             }
//         }
//     }
// }

bool LfpLatencyProcessor::checkEventReceived()
{
    return eventReceived;
}

void LfpLatencyProcessor::resetEventFlag()
{
    // eventReceived = false;
}

float *LfpLatencyProcessor::getdataCacheLastRow()
{
    float *rowPtr = (dataCache.data() + (currentTrack % DATA_CACHE_SIZE_TRACKS) * DATA_CACHE_SIZE_SAMPLES);
    return rowPtr;
}

float *LfpLatencyProcessor::getdataCacheRow(int track)
{
    uint32_t colIndex = (((currentTrack - track) % DATA_CACHE_SIZE_TRACKS) + DATA_CACHE_SIZE_TRACKS) % DATA_CACHE_SIZE_TRACKS;

    float *rowPtr = (dataCache.data() + colIndex * DATA_CACHE_SIZE_SAMPLES);

    return rowPtr;
}

int LfpLatencyProcessor::getLatencyData(int track)
{
    int data = spikeLocation[(((currentTrack - track) % DATA_CACHE_SIZE_TRACKS) + DATA_CACHE_SIZE_TRACKS) % DATA_CACHE_SIZE_TRACKS];
    return data;
}

void LfpLatencyProcessor::pushLatencyData(int latency)
{
    spikeLocation[currentTrack % DATA_CACHE_SIZE_TRACKS] = latency;
}

float *LfpLatencyProcessor::getdataCache()
{
    // float* rowPtr = (dataCache+currentTrack*DATA_CACHE_SIZE_SAMPLES);
    return dataCache.data();
}
int LfpLatencyProcessor::getSamplesPerSubsampleWindow()
{
    return samplesPerSubsampleWindow;
}

void LfpLatencyProcessor::changeParameter(int parameterID, float value)
{
    switch (parameterID)
    {
    case 1:
        samplesPerSubsampleWindow = value;
        break;
    case 2:
        samplesAfterStimulusStart = value;
        break;
    case 3:
        // change current trigger chan
        if (value >= 0 && value < 25)
            triggerChannel_idx = value;
        break;
    case 4:
        // change current trigger chan
        if (value >= 0 && value < 25)
            dataChannel_idx = value;
        break;
    case 5:
        // change current strimulus detection threshold
        if (value >= 0)
            stimulus_threshold = value;
        break;
    }
    /*if (parameterID == 1)
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
    }*/
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
