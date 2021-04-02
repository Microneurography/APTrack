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

/*
 TODO: Spike detected indicator in GUI
 TODO: Spike threshold in GUI
 TODO:  ..
 */

#ifndef LFPLATENCYPROCESSOR_H_INCLUDED
#define LFPLATENCYPROCESSOR_H_INCLUDED

#ifdef _WIN32


#define NOGDI
#define NOMINMAX


#include <Windows.h>
#endif

#include <ProcessorHeaders.h>
#include <functional>

//fifo buffer size. height in pixels of spectrogram image
#define FIFO_BUFFER_SIZE 30000

//Width in pixels of spectrogram image
//300 pixels = 300 tracks approx 5 min
#define SPECTROGRAM_WIDTH 600

#define SPECTROGRAM_HEIGHT 600

#define EVENT_DETECTION_THRESHOLD 1500

#define DATA_CACHE_SIZE_SAMPLES 30000

#define DATA_CACHE_SIZE_TRACKS 300

//for debug
#define SEARCH_BOX_WIDTH 3

/**
    This class serves as a template for creating new processors.

    If this were a real processor, this comment section would be used to
    describe the processor's function.

    @see GenericProcessor
*/
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
    AudioProcessorEditor* createEditor() override;

    /** Optional method that informs the GUI if the processor is ready to function. If false acquisition cannot start. Defaults to true */
    //bool isReady();
    
    /** Convenient interface for responding to incoming events. */
    //NOTE NOT CURRENTLY USED! Events detected in process() instead
    //void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;

    /** Defines the functionality of the processor.
        The process method is called every time a new data buffer is available.
    */
    void process (AudioSampleBuffer& buffer) override;
    
    /** The method that standard controls on the editor will call.
        It is recommended that any variables used by the "process" function
        are modified only through this method while data acquisition is active. */
    void setParameter (int parameterIndex, float newValue) override;

    /** Saving custom settings to XML. */
    virtual void saveCustomParametersToXml (XmlElement* parentElement) override;

    /** Load custom settings from XML*/
    virtual void loadCustomParametersFromXml() override;

    /** Optional method called every time the signal chain is refreshed or changed in any way.

        Allows the processor to handle variations in the channel configuration or any other parameter
        passed down the signal chain. The processor can also modify here the settings structure, which contains
        information regarding the input and output channels as well as other signal related parameters. Said
        structure shouldn't be manipulated outside of this method.
    */
    //void updateSettings();
    
    
     /** Method to allow the visualizer to check eventDetection flag*/ 
    bool checkEventReceived();
    
    /** Method to allow the visualizer to reset the eventDetection flag*/ 
    void resetEventFlag();
    
    /**
     Returns pointer to first element of latency track data of last (second most current) track. Equivalent to getdataCacheRow(1)
     - Returns: pointer to first element of last (second most current) latency track
     */
    float* getdataCacheLastRow();
    
    /**
     Returns pointer to first lement of raw circular array.

     - Returns: pointer to raw circular array
     */
    float* getdataCache();
    
    /**
     Returns pointer stored latency track data, one track at a time
     - Parameter track: index of track (0 being current (earliest) track)
     
     - Throws: 'std::out_of_range'
                if 'track' index is out of range
     - Returns: pointer to first element of latency track
     
     */
    float* getdataCacheRow(int track);
    
    void changeParameter(int parameterID, int value);

	int getParameterInt(int parameterID);
    
    void pushLatencyData(int latency);
    
    int getLatencyData(int track);
/*
    int windowSampleCount;
    
    float lastWindowPeak;
    
    int samplesAfterStimulus;
 */
    
    int fifoIndex;

	//debug
	float getParameterFloat(int parameterID);


private:
	bool foundPlugin;
	bool foundElement;
	bool docExisted;
	File recoveryConfigFile;
	String value;
	String name;
	XmlElement *thisPlugin;
	XmlElement *theElement;
	XmlElement *recoveryConfig;
	//debug
	float lastReceivedDACPulse;

	int dataChannel_idx;
	int triggerChannel_idx;

	int triggerChannel_threshold;

	void timerCallback(int timerID) override;
    
    float dataCache[DATA_CACHE_SIZE_TRACKS*DATA_CACHE_SIZE_SAMPLES];
    
    int spikeLocation[DATA_CACHE_SIZE_TRACKS];
    
    bool eventReceived;
    
    int currentSample;
    
    
    int samplesPerSubsampleWindow;

    
    int currentTrack;
    
    int peakThreshold;

    int samplesAfterStimulusStart;

	float stimulus_threshold;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfpLatencyProcessor);
};


#endif  // LFPLATENCYPROCESSOR_H_INCLUDED
