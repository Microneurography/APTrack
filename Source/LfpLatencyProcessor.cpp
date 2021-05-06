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
#include <string>
#include <mutex>
#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorEditor.h"
#include "LfpLatencySpectrogramControlPanel.h"
//#include "/modules/juce_core/files/juce_File.h"
//#include "C:\\Users\\gsboo\\source\\repos\\plugin-GUI\\JuceLibraryCode\\modules\\juce_core\\misc\\juce_Result.h"
#include <map>
#include "semaphore.hpp"

//If the processor uses a custom editor, it needs its header to instantiate it
//#include "ExampleEditor.h"

// declaring the variables for the saving and loading stuff
// loading
std::map<String, String> LfpLatencyProcessor::customParameters;
bool LfpLatencyProcessor::loadRecovery = false;
bool LfpLatencyProcessor::loaded = false;
int LfpLatencyProcessor::j = 0;

// saving
int LfpLatencyProcessor::i = 0;
bool LfpLatencyProcessor::foundCustomParams = false;
bool  LfpLatencyProcessor::docExisted = false;
bool LfpLatencyProcessor::fileOK = false;
bool LfpLatencyProcessor::writtenOK = false;
File LfpLatencyProcessor::recoveryConfigFile;
String LfpLatencyProcessor::workingDirectory;
String LfpLatencyProcessor::value;
String LfpLatencyProcessor::name;
String LfpLatencyProcessor::elementName;
ScopedPointer<XmlElement> LfpLatencyProcessor::recoveryConfig;
ScopedPointer<XmlElement> LfpLatencyProcessor::processor;
ScopedPointer<XmlElement> LfpLatencyProcessor::customParams;

std::mutex savingAndLoadingLock;


LfpLatencyProcessor::LfpLatencyProcessor()
    : GenericProcessor("LfpLatency"), fifoIndex(0), eventReceived(false), samplesPerSubsampleWindow(60), samplesAfterStimulusStart(0)

{
    setProcessorType(PROCESSOR_TYPE_SINK);

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

void LfpLatencyProcessor::saveRecoveryData(XmlElement *parentElement)
{
	// Sometimes the XML file becomes blank. I thought originally it was because of threads,
	// i.e. if two second threads accessed the file at the same time, the file would get corrupted.
	// I thought the mutex lock would fix it, but it didn't.
	// Due to safeguarding written in this function, a blank xml file does not cause a crash,
	// but it's still loosing data, which is a problem when it's whole purpose is to save data
	savingAndLoadingLock.lock();
	name = parentElement->getAttributeName(0);
	value = parentElement->getAttributeValue(0);
	elementName = parentElement->getTagName();
	std::cout << "Trying to save " << name << std::endl;
	foundCustomParams = false;
	docExisted = false;
	fileOK = false;
	writtenOK = false;
	workingDirectory = File::getCurrentWorkingDirectory().getFullPathName();
	workingDirectory += "\\LastLfpLatencyPluginComponents.xml";
	recoveryConfigFile = File(workingDirectory);
	std::cout << "Loaded XML" << std::endl;
	if (recoveryConfigFile.exists())
	{
		docExisted = true;
		recoveryConfig = XmlDocument::parse(recoveryConfigFile);
		std::cout << "Parsed file" << std::endl;
		if (recoveryConfig != NULL) // if there was no error with parsing
		{
			forEachXmlChildElementWithTagName(*recoveryConfig, customParams, elementName)
			{
				foundCustomParams = true;
				customParams->setAttribute(name, value);
			}
			if (foundCustomParams == false)
			{
				customParams = new XmlElement(elementName);
				customParams->setAttribute(name, value); // I tried writing this line just once, but it caused a crash because of customParams not being defined
			}
		}
		else // there was an error with parsing it, it's probably a blank file.
		{
			std::cout << "FAILURE: The file was unable to be openend due to an unknown error" << std::endl;
			std::cout << "All previous data written to LastLfpLatencyPluginComponents.XML has been lost." << std::endl;
			std::cout << "Lfp Latency Plugin will now delete the old file, and create a new one." << std::endl;
			docExisted = false;
			recoveryConfigFile.deleteFile();

		}
	}

	if (!docExisted) // the file doesn't exist, so we have to make it and its main element
	{
		std::cout << "Unable to find LastLfpLatencyPluginComponents.XML" << std::endl;
		std::cout << "Making the XML" << std::endl;
		Result res = Result(recoveryConfigFile.create());
		if (Result::ok())
		{
			std::cout << "File Created OK" << std::endl;
			fileOK = true;
			recoveryConfig = new XmlElement("LastLfpLatencyPluginComponents");
			customParams = new XmlElement(elementName);
			customParams->setAttribute(name, value);
			recoveryConfig->addChildElement(customParams);
		}
		else // The docuent didn't exist and we were unable to make it. Simply report to user and continue.
		{
			std::cout << "WARNING: The file was unable to be saved due to error " << res.getErrorMessage() << std::endl;
			std::cout << "The plugin will keep attempting to make the file every time you change a configuration." << std::endl;
			std::cout << "Please be aware that this error message means that " << name << " at " << value << " WAS NOT saved, and will not be saved." << std::endl;
		}
	}

	// This is commented out because I changed the hierarchy when I switched xml files, but I didn't want to delete it just yet
	//// if thisPlugin wasn't found but we managed to make the file or it existed, we need to make it.
	//if (foundPlugin == false && (fileOK || docExisted)) // need to do the or first otherwise logic doesn't work
	//{ 
	//	// if we found a processor, but it wasn't *the* processor we wanted, we'll be here.
	//	customParams = new XmlElement(elementName);
	//	processor->addChildElement(customParams);
	//	customParams->setAttribute(name, value);
	//}

	// if the file now exists, so we can save it
	if (fileOK || docExisted) {
		writtenOK = recoveryConfig->writeToFile(recoveryConfigFile, "", "UTF-8", 60);
		if (writtenOK == false) // The document was unable to save
		{
			std::cout << "WARNING: The file was unable to be saved due to an error with writing to the xml file" << std::endl;
			std::cout << "Please be aware that this error message means that " << name << " at " << value << " WAS NOT saved, and will not be saved." << std::endl;
		}
		else
		{
			std::cout << name << " saved successfully." << std::endl;
		}
	}
	// This needs to be added at some other place. This function is for generically adding things to the xml under this plugins name
	//XmlElement *TracksToXML = mainNode->createNewChildElement("Tracks");
	//uint64 timeStamp = getTimestamp(LfpLatencyProcessorVisualizerContentComponent.dataChannelComboBox->getSelectedId());
	//TracksToXML->setAttribute("Track1", timeStamp)

	// deleting everything
	// Don't need to delete the XmlElements because they're ScopedPointers 
	//recoveryConfigFile.~File();
	workingDirectory.~String();
	value.~String();
	name.~String();
	elementName.~String();
	savingAndLoadingLock.unlock();
}

void LfpLatencyProcessor::loadRecoveryData()
{
	// this function will be called from the same one that asks if pulse pal could be loaded, because I think that's the entry point
	loaded = false;
	loadRecovery = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Load LfpLatency Configurations?", "Would you like to load previous Lfp Latency Configurations?", "Yes", "No");
	// doing the same iteration sequence as before, but this time with no checks because 
	// if it doesn't exist at this point just leave it, report to user (using loaded) fall out
	// it doesn't make sense to create new things when you're trying to load them in
	if (loadRecovery)
	{
		savingAndLoadingLock.lock();
		workingDirectory = File::getCurrentWorkingDirectory().getFullPathName();
		workingDirectory += "\\LastLfpLatencyPluginComponents.xml";
		recoveryConfigFile = File(workingDirectory);
		std::cout << "Loaded XML" << std::endl;
		if (recoveryConfigFile.exists())
		{
			recoveryConfig = XmlDocument::parse(recoveryConfigFile);
			std::cout << "Parsed file" << std::endl;
			if (recoveryConfig != NULL) // if there was no error with parsing
			{
				loaded = true;
				forEachXmlChildElement(*recoveryConfig, customParams) // for every child
				{
					j = 0;
					while (j < customParams->getNumAttributes())  // for all the attributes
					{
						name = customParams->getAttributeName(j);
						value = customParams->getAttributeValue(j);
						customParameters.insert(std::make_pair(name, value));  // save in the map
						j++;
					}
				}
				LfpLatencySpectrogramControlPanel::loadParameters(customParameters);
				// This section of comments is what you would need to change here in order to implement data saving
				// see https://stackoverflow.com/questions/4599792/accessing-stdmap-keys-and-values 
				// as my reason for doing it this way
				// for (map<K, V>::iterator itr = customParameters.begin(); itr != customParameters.end(); ++itr) {
					// spectrogramAsVecor.append(itr->second);
				// }
				// LfpLatencySpectrogram::loadImage(spectrogramAsVecor);
			}
		}
		if (loaded == false)
		{
			AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Error Loading LfpLatency Configurations", "Could not open the file due to unknown reasons. Configurations could not be loaded.", "OK");
		}
		savingAndLoadingLock.unlock();
	}
}

void LfpLatencyProcessor::saveCustomParametersToXml(XmlElement *parentElement)
{
	XmlElement *mainNode = parentElement->createNewChildElement("LfpLatencyProcessor");
    mainNode->setAttribute("numParameters", getNumParameters());

    for (int i = 0; i < getNumParameters(); ++i)
    {
        XmlElement *parameterNode = mainNode->createNewChildElement("Parameter");

        auto parameter = getParameterObject(i);
        parameterNode->setAttribute("name", parameter->getName());
        parameterNode->setAttribute("type", parameter->getParameterTypeString());

        auto parameterValue = getParameterVar(i, currentChannel);

        if (parameter->isBoolean())
            parameterNode->setAttribute("value", (int)parameterValue);
        else if (parameter->isContinuous() || parameter->isDiscrete() || parameter->isNumerical())
            parameterNode->setAttribute("value", (double)parameterValue);
    }
}

void LfpLatencyProcessor::loadCustomParametersFromXml()
{
    if (parametersAsXml == nullptr) // prevent double-loading
        return;

    // use parametersAsXml to restore state

    forEachXmlChildElement(*parametersAsXml, mainNode)
    {
        if (mainNode->hasTagName("LfpLatencyProcessor"))
        {
            int parameterIdx = -1;

            forEachXmlChildElement(*mainNode, parameterNode)
            {
                if (parameterNode->hasTagName("Parameter"))
                {
                    ++parameterIdx;

                    String parameterType = parameterNode->getStringAttribute("type");
                    if (parameterType == "Boolean")
                        setParameter(parameterIdx, parameterNode->getBoolAttribute("value"));
                    else if (parameterType == "Continuous" || parameterType == "Numerical")
                        setParameter(parameterIdx, parameterNode->getDoubleAttribute("value"));
                    else if (parameterType == "Discrete")
                        setParameter(parameterIdx, parameterNode->getIntAttribute("value"));
                }
            }
        }
    }
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
