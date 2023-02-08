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
#include "ppController.h"

/*
	General TODO s:
	- Explore the use of iterators to access dataVector
	TODO get duration as relative time to avoid casting back and forth

*/

/*
	Pulse Pal controller
*/
ppController::ppController(LfpLatencyProcessor *processor)
{
	this->processor = processor;
	protocolStepNumber = -1;
	protocolDuration = 0.0f;

	stimulusVoltage = 0.0f;
	pulsePalConnected = false;
}

ppController::~ppController()
{
	//Disconnect pulsePal
	pulsePal->abortPulseTrains(); //Make sure we stop stimulating!
	pulsePal->updateDisplay("Disconnected", "Click for menu");
	pulsePal->end();
}

bool ppController::initializeConnection()
{
	ustepper = std::unique_ptr<UStepper>(new UStepper());
	ustepper->initialize();
	if (ustepper->isConnected()){
		startTimer(TIMER_USTEPPER, 500);
	}

	pulsePal = new PulsePal();
	pulsePal->initialize();
	pulsePal->setDefaultParameters();
	pulsePal->updateDisplay("GUI Connected", "Click for Menu");
	pulsePalVersion = pulsePal->getFirmwareVersion();


	if ((pulsePalVersion == 0))
	{

		return false;
	}




	pulsePalConnected = true;

	std::cout << "pulsePalVersion " << pulsePalVersion << std::endl;

	// abort pulse trains (if any)
	pulsePal->abortPulseTrains();

	// configure pulse pal
	pulsePal->currentOutputParams[1].isBiphasic = 0;
	pulsePal->currentOutputParams[1].restingVoltage = 0.0f;
	pulsePal->currentOutputParams[1].phase1Duration = 0.0005f;
	pulsePal->currentOutputParams[1].pulseTrainDuration = 1000.0f;
	pulsePal->currentOutputParams[1].interPulseInterval = 2.0f;
	pulsePal->currentOutputParams[1].phase1Voltage = 0.0f;
	pulsePal->currentOutputParams[1].phase2Voltage = 0.0f;

	pulsePal->currentOutputParams[2].isBiphasic = 0;
	pulsePal->currentOutputParams[2].restingVoltage = 0.0f;
	pulsePal->currentOutputParams[2].phase1Duration = 0.0005f;
	pulsePal->currentOutputParams[2].pulseTrainDuration = 1000.0f;
	pulsePal->currentOutputParams[2].interPulseInterval = 2.0f;
	pulsePal->currentOutputParams[2].phase1Voltage = 0.0f;
	pulsePal->currentOutputParams[2].phase2Voltage = 0.0f;

	pulsePal->currentOutputParams[3].isBiphasic = 0;
	pulsePal->currentOutputParams[3].restingVoltage = 0.0f;
	pulsePal->currentOutputParams[3].phase1Duration = 0.0005f + (RELAY_TTL_delay_s * 2); //
	pulsePal->currentOutputParams[3].pulseTrainDuration = 1000.0f;
	pulsePal->currentOutputParams[3].interPulseInterval = 2.0f;
	pulsePal->currentOutputParams[3].phase1Voltage = 0.0f;
	pulsePal->currentOutputParams[3].phase2Voltage = 0.0f;

	pulsePal->syncAllParams();
	return true;
}

void ppController::setMinStimulusVoltage(float newMinVoltage)
{
	if (newMinVoltage<0){
		newMinVoltage=0;
	}
	minStimulusVoltage = newMinVoltage;
	if (stimulusVoltage<newMinVoltage){
		setStimulusVoltage(newMinVoltage);
	}
}
float ppController::getMinStimulusVoltage()
{
	return minStimulusVoltage;
}
void ppController::setMaxStimulusVoltage(float newMaxVoltage)
{
	if (newMaxVoltage > 10)
	{ // #TODO: set as compiler macro
		newMaxVoltage = 10;
	}

	maxStimulusVoltage = newMaxVoltage;
	if (newMaxVoltage<stimulusVoltage){
		setStimulusVoltage(newMaxVoltage);
	}
}
float ppController::getMaxStimulusVoltage()
{
	return maxStimulusVoltage;
}

void ppController::setStimulusVoltage(float newVoltage)
{
	
	std::cout << "moved to pp controller\n";
	//TODO: Set/sync min/max stim voltages
	if (newVoltage > maxStimulusVoltage)
	{
		newVoltage = maxStimulusVoltage;
	}
	if (newVoltage < minStimulusVoltage)
	{
		newVoltage = minStimulusVoltage;
	}

	// update the uStepper (TODO.. only do this every second...)
	//ustepper->setRelativePosition(newVoltage-stimulusVoltage);

	stimulusVoltage = newVoltage;

	//Update channel voltages
	std::cout << "New stimulus voltage " << stimulusVoltage << std::endl;
	if (!this->pulsePalConnected){
		
		return;
	}
	pulsePal->currentOutputParams[1].phase1Voltage = stimulusVoltage;
	std::cout << "Updating stimulus voltage in pp \n";
	pulsePal->currentOutputParams[2].phase1Voltage = 10.0f;
	std::cout << "updating max voltage in pp\n";
	pulsePal->currentOutputParams[3].phase1Voltage = 3.0f;

	pulsePal->syncAllParams();
	std::cout << "synced all params in pp\n";
	processor->addMessage("setStimVoltage:" + std::to_string(newVoltage));

	
}
float ppController::getStimulusVoltage()
{
	return stimulusVoltage;
}
bool ppController::isProtocolRunning()
{
	return protocolRunning;
}
void ppController::StartCurrentProtocol()
{
	protocolRunning = true;
	protocolStepNumber = 0;

	// send message to openephys (when playing)
	this->processor->addMessage("starting stimulus protocol " + protocolName);

	// Start Protocol Timer
	startTimer(TIMER_PROTOCOL, static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));

	// send to pulse pal
	sendProtocolStepToPulsePal(protocolData[protocolStepNumber]);

	// Calculate end times
	// End time is current time plus duration in ms
	int64 millisecondsNow = Time::getMillisecondCounter();

	protocolEndingTime = millisecondsNow + static_cast<int>(protocolDuration * 1000.0f);
	protocolStepEndingTime = millisecondsNow + static_cast<int>(protocolData[protocolStepNumber].duration * 1000.0f);

	std::cout << "protocolDuration " << protocolDuration << std::endl;
	std::cout << "msNow " << millisecondsNow << std::endl;
	std::cout << "protocolEndingTime " << protocolEndingTime << std::endl;
}

void ppController::StopCurrentProtocol()
{
	// TODO: There may be a race condition here.
	// We want to stop the timer first to prevent further triggers, then abort the trains
	stopTimer(TIMER_PROTOCOL); // protocol timer
	//abort pulse train
	pulsePal->abortPulseTrains();
	protocolRunning = false;
	protocolStepNumber = -1;
}

void ppController::timerCallback(int timerID)
{
	if (timerID == TIMER_USTEPPER){
		ustepper->setPosition(stimulusVoltage);
	}
	if (timerID == TIMER_PROTOCOL) // Protocol step timer
	{
		//Stop old Timer
		stopTimer(TIMER_PROTOCOL);

		//PulsePal Specific
		pulsePal->abortPulseTrains();

		// Increment counter if there are any remaining steps in protocol
		if (protocolStepNumber < (elementCount - 1))
		{
			protocolStepNumber++;

			// Start next timer
			startTimer(TIMER_PROTOCOL, static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));

			// Update protocol step end time
			int64 millisecondsNow = Time::getMillisecondCounter();

			protocolStepEndingTime = millisecondsNow + static_cast<int>(protocolData[protocolStepNumber].duration * 1000.0f);

			//send to pulse pal
			sendProtocolStepToPulsePal(protocolData[protocolStepNumber]);
		}
		else
		{
			// If no more steps then stop timer
			StopCurrentProtocol();
		}
	}
}

void ppController::loadFile(String file) //, std::vector<protocolDataElement> csvData)
{
	File fileToRead(file);
	//lastFilePath = fileToRead.getParentDirectory();
	// fileName_text->setText(fileToRead.getFileName(), dontSendNotification);

	// TODO: Read line by line
	StringArray protocolData_raw;
	fileToRead.readLines(protocolData_raw);

	//TODO: Check if header matches what was expected

	/*

	Files should be formated as csv, with the following structure:
	Note " " around text already done when saving as csv in excel
	
	Duration,Voltage,Rate,Comment <-header
	number,number,number,"text"	  <-data entry(s)
	...
	*/

	//Remove headers
	protocolData_raw.remove(0);

	// Remove empty strings
	protocolData_raw.removeEmptyStrings();

	//Get number of elements in string
	elementCount = protocolData_raw.size();

	// Stop and clear current protocol in case exists.
	StopCurrentProtocol();
	protocolData.clear();

	// store new protocol
	for (int ii = 0; ii < elementCount; ii++)
	{

		StringArray tempData;
		protocolDataElement tempObj;

		// parse step
		tempData = StringArray::fromTokens(protocolData_raw[ii], ",", "\"");

		// store step parameters
		tempObj.duration = tempData[0].getFloatValue();
		tempObj.voltage = tempData[1].getFloatValue();
		tempObj.rate = tempData[2].getFloatValue();
		tempObj.comment = tempData[3];

		// increment protocol duration
		protocolDuration = protocolDuration + tempObj.duration;

		//store protocol step
		protocolData.push_back(tempObj);
	}
	protocolStepNumber = 0;

	protocolName = fileToRead.getFileName().toStdString();
}

void ppController::sendProtocolStepToPulsePal(protocolDataElement protocolDataStep)
{
	std::stringstream ss;
	ss << "voltage:" << protocolDataStep.voltage << "; rate:" << protocolDataStep.rate << "; duration:" << protocolDataStep.duration;
	this->processor->addMessage(ss.str()); // Send message to openephys stack.
	if (!pulsePalConnected)				   // Debug mode - no pulsepal connected.
		return;
	if (protocolDataStep.rate == 0)
	{
		// if rate is 0 the treat as pause and abort pulse trains
		pulsePal->abortPulseTrains();
	}
	else
	{
		// if rate is nonzero then calculate pulse period and send to pulsepal
		float pulsePeriod = (1.0f / protocolDataStep.rate);
		// HACK: currently we are providing a TTL to open/close relay on output.
		// Set channel 3 high Xms, delay the others by the response

		pulsePal->abortPulseTrains();
		// Channel 1 - to the stimulator
		pulsePal->currentOutputParams[1].pulseTrainDuration = protocolDataStep.duration; // in sec
		pulsePal->currentOutputParams[1].interPulseInterval = pulsePeriod;
		pulsePal->currentOutputParams[1].pulseTrainDelay = RELAY_TTL_delay_s; // in sec
		// Channel 2 - feedback to TTL on acquisition
		pulsePal->currentOutputParams[2].pulseTrainDuration = protocolDataStep.duration; // in sec
		pulsePal->currentOutputParams[2].interPulseInterval = pulsePeriod;				 // in sec
		pulsePal->currentOutputParams[2].pulseTrainDelay = RELAY_TTL_delay_s;
		// Channel 3 - open/close relay
		pulsePal->currentOutputParams[3].pulseTrainDuration = protocolDataStep.duration; // duration of the TTL should start before the stimulus, and end after the stimulus
		pulsePal->currentOutputParams[3].interPulseInterval = pulsePeriod - RELAY_TTL_delay_s * 2;
		pulsePal->currentOutputParams[3].pulseTrainDelay = 0;

		pulsePal->syncAllParams();

		pulsePal->triggerChannels(1, 1, 1, 0);
	}
}

String ppController::formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration)
{
	//Format mm:ss/mm:ss
	// TODO: cleanup?
	return (String(static_cast<int>(std::floor(step_secondsRemaining.inMinutes())) % 60).paddedLeft('0', 2) + ":" + String(static_cast<int>(std::floor(step_secondsRemaining.inSeconds())) % 60).paddedLeft('0', 2) + "/" + String(static_cast<int>(std::floor(RelativeTime(step_duration).inMinutes())) % 60).paddedLeft('0', 2) + ":" + String(static_cast<int>(std::floor(RelativeTime(step_duration).inSeconds())) % 60).paddedLeft('0', 2));
}