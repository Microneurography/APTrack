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
#pragma once
#include <EditorHeaders.h>
#include <functional>
#include "serial/PulsePal.h"
#include "../LfpLatencyProcessor.h"
#include "serial/UStepper.h"

#define TIMER_PROTOCOL 0
#define TIMER_USTEPPER 1

class LfpLatencyProcessor;
struct protocolDataElement {
	float duration;
	float rate;
	float voltage;
	String comment;
};


class ppController :
	public MultiTimer
{
friend class ppControllerVisualizer;
public:

	ppController(LfpLatencyProcessor *processor);

	~ppController();

	void setStimulusVoltage(float newVoltage);
	float getStimulusVoltage();
	void setMinStimulusVoltage(float newMinVoltage);
	float getMinStimulusVoltage();
	void setMaxStimulusVoltage(float newMaxVoltage);
	float getMaxStimulusVoltage();
	
	bool initializeConnection(); // returns false if connection fails
	bool isProtocolRunning();
private:

	bool protocolRunning;
	int protocolStepNumber;

	int elementCount;

	float stimulusVoltage;
	float minStimulusVoltage = 0;
	float maxStimulusVoltage = 10;

	float protocolDuration;

	int64 protocolStepEndingTime;

	int64 protocolEndingTime;

	std::vector<protocolDataElement> protocolData;
	std::string protocolName = "";

	String formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration);

	LfpLatencyProcessor * processor;

	void loadFile(String file);//, std::vector<protocolDataElement> data);

	void timerCallback(int timerID) override;

	PulsePal* pulsePal;
	uint32_t pulsePalVersion;

	bool pulsePalConnected;

	float RELAY_TTL_delay_s = 0.0005f; // The delay in seconds between 
	void StopCurrentProtocol();
	void StartCurrentProtocol();

	void sendProtocolStepToPulsePal(protocolDataElement protocol);

	std::unique_ptr<UStepper> ustepper;

};