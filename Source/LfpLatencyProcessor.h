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

#ifndef LFPLATENCYPROCESSOR_H_INCLUDED
#define LFPLATENCYPROCESSOR_H_INCLUDED

#ifdef _WIN32

#define NOGDI
#define NOMINMAX

#include <Windows.h>
#endif
#include <string>
#include <ProcessorHeaders.h>
#include <functional>
#include <map>
#include <unordered_map>
#include <queue>
#include <mutex>
#include "pulsePalController/ppController.h"

//fifo buffer size. height in pixels of spectrogram image
#define FIFO_BUFFER_SIZE 30000

//Width in pixels of spectrogram image
//300 pixels = 300 tracks approx 5 min
#define SPECTROGRAM_WIDTH 300

#define SPECTROGRAM_HEIGHT 300

#define PPCONTROLLER_WIDTH 305

#define PPCONTROLLER_HEIGHT 130

#define EVENT_DETECTION_THRESHOLD 1500

#define DATA_CACHE_SIZE_SAMPLES 30000

#define DATA_CACHE_SIZE_TRACKS 300

//for debug
#define SEARCH_BOX_WIDTH 3

class ppController;
struct SpikeInfo
{
    int spikeSampleNumber;  // the recording sample number of the spike
    int spikeSampleLatency; // the spike time relative to the stimulus
    float spikePeakValue;   // the peak value
    int windowSize = 30;    // the number of samples used to identify spike
    float threshold;        // the threshold value for the spike, used to detect
    float stimulusVoltage;  // the stimulus voltage used to illicit the spike
    int trackIndex;         // the track index for the stimulus (currentTrack)
};


class SpikeGroup
{
public:
    SpikeGroup() : spikeHistory(), recentHistory(10), templateSpike(), isTracking(false), isActive(false)
    {
        spikeHistory.reserve(1000);
    };
    // ~SpikeGroup();

    std::vector<SpikeInfo> spikeHistory;
    std::deque<bool> recentHistory;
    SpikeInfo templateSpike; // the information used to determine the spike
    bool isTracking;         // is the stimulus volt being tracked?
    bool isActive;           // is this spike currently active
    float stimulusVoltage50pct = -1; // the last known 50pct firing voltage
    //const uint16 uid; // Unique identifier for the spike group #TODO: create uid
};
class LfpLatencyProcessor : public GenericProcessor, public MultiTimer

{
public:
    /** The class constructor, used to initialize any members. */
    LfpLatencyProcessor();

    /** The class destructor, used to deallocate memory */
    ~LfpLatencyProcessor();

    /** Indicates if the processor has a custom editor. Defaults to false */
    //bool hasEditor() const { return true; }

    /** If the processor has a custom editor, this method must be defined to instantiate it. */
    AudioProcessorEditor *createEditor() override;

    /** Optional method that informs the GUI if the processor is ready to function. If false acquisition cannot start. Defaults to true */
    //bool isReady();

    /** Convenient interface for responding to incoming events. */
    //NOTE NOT CURRENTLY USED! Events detected in process() instead
    //void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;

    /** Defines the functionality of the processor.
        The process method is called every time a new data buffer is available.
    */
    void process(AudioSampleBuffer &buffer) override;

    /** The method that standard controls on the editor will call.
        It is recommended that any variables used by the "process" function
        are modified only through this method while data acquisition is active. */
    void setParameter(int parameterIndex, float newValue) override;

    /** This method is a critical section, protected a mutex lock. Allows you to save slider values, and maybe
	some data if you wanted in a file called LastLfpLatencyPluginComponents */
    static void saveRecoveryData(std::unordered_map<std::string, juce::String> *valuesMap);

    /** Starts by asking the user if they would like to load data from LastLfpLatencyPluginComponents, 
	the rest is a critical section protected by the same mutex lock as saveRecoveryData. */
    static void loadRecoveryData(std::unordered_map<std::string, juce::String> *valuesMap);

    /** Saving custom settings to XML. */
    virtual void saveCustomParametersToXml(XmlElement *parentElement) override;

    /** Load custom settings from XML*/
    virtual void loadCustomParametersFromXml() override;

    virtual void createEventChannels() override;

    //virtual void createSpikeChannels() override;

    /** Optional method called every time the signal chain is refreshed or changed in any way.

        Allows the processor to handle variations in the channel configuration or any other parameter
        passed down the signal chain. The processor can also modify here the settings structure, which contains
        information regarding the input and output channels as well as other signal related parameters. Said
        structure shouldn't be manipulated outside of this method.
    */
    //void updateSettings();

    // Channel used for the recording of spike data
    //virtual void createSpikeChannels() override;

    /** Method to allow the visualizer to check eventDetection flag*/
    bool checkEventReceived();

    /** Method to allow the visualizer to reset the eventDetection flag*/
    void resetEventFlag();

    /**
     Returns pointer to first element of latency track data of last (second most current) track. Equivalent to getdataCacheRow(1)
     - Returns: pointer to first element of last (second most current) latency track
     */
    float *getdataCacheLastRow();

    /**
     Returns pointer to first lement of raw circular array.

     - Returns: pointer to raw circular array
     */
    float *getdataCache();

    //Sets data channel back to default
    void resetDataChannel();

    //Sets trigger channle to default
    void resetTriggerChannel();

    /**
     Returns pointer stored latency track data, one track at a time
     - Parameter track: index of track (0 being current (earliest) track)
     
     - Throws: 'std::out_of_range'
                if 'track' index is out of range
     - Returns: pointer to first element of latency track
     
     */
    float *getdataCacheRow(int track);

    void changeParameter(int parameterID, float value);

    int getParameterInt(int parameterID);

    int getSamplesPerSubsampleWindow();

    void pushLatencyData(int latency);

    int getLatencyData(int track);
    /*
    int windowSampleCount;
    
    float lastWindowPeak;
    
    int samplesAfterStimulus;
 */

    int fifoIndex;

    uint32_t currentTrack;
    int currentSample;

    //debug
    float getParameterFloat(int parameterID);
    //Result makingFile;

    //Functions used to save data
    void addMessage(std::string message);

    void addSpike(std::string spike);

    void addSpikeGroup(SpikeInfo templateSpike, bool isSelected=false);
    void removeSpikeGroup(int i);

    SpikeGroup *getSpikeGroup(int i);
    int getSpikeGroupCount();
    int getSelectedSpike();
    void setSelectedSpike(int i);
    void setSelectedSpikeLocation(int loc);
    void setSelectedSpikeThreshold(float val);
    void setSelectedSpikeWindow(int window);

    int getTrackingSpike();
    void setTrackingSpike(int i);

    float getStimulusVoltage();
    void setStimulusVoltage(float sv);

    float getTrackingIncreaseRate();
    void setTrackingIncreaseRate(float sv);

    float getTrackingDecreaseRate();
    void setTrackingDecreaseRate(float sv);

    ppController *pulsePalController;

private:
    friend class ppController;
    float stimulusVoltage = 0;

    float trackingIncreaseRate = 0.01;
    float trackingDecreaseRate = 0.01;
    //debug
    float lastReceivedDACPulse;

    int dataChannel_idx;
    int triggerChannel_idx;

    int triggerChannel_threshold;

    void timerCallback(int timerID) override;

    void trackSpikes(); // updates the currently tracked spike group
    void trackThreshold();

    std::vector<SpikeGroup> spikeGroups; // The groups of spikes that have been traced
    std::mutex spikeGroups_mutex;

    float dataCache[(DATA_CACHE_SIZE_TRACKS + 1) * DATA_CACHE_SIZE_SAMPLES]; // TODO convert to vector.
    std::vector<int> dataCacheTimestamps;
    int spikeLocation[DATA_CACHE_SIZE_TRACKS];

    bool eventReceived;

    EventChannel *pulsePalEventPtr;
    EventChannel *spikeEventPtr;

    //int currentSample;

    int samplesPerSubsampleWindow;

    //int currentTrack;

    int peakThreshold;

    int samplesAfterStimulusStart;

    float stimulus_threshold;

    std::queue<String> messages;
    std::queue<String> spikes;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessor);
};

#endif // LFPLATENCYPROCESSOR_H_INCLUDED
