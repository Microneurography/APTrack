#include"ppController.h"

/*
	General TODO s:
	- Explore the use of iterators to access dataVector
	TODO get duration as relative time to avoid casting back and forth

*/


/*
	Pulse Pal controller
*/
ppController::ppController()
{

	// Init PulsePal
	pulsePal.initialize();
	pulsePal.setDefaultParameters();
	pulsePal.updateDisplay("GUI Connected", "Click for Menu");
	pulsePalVersion = pulsePal.getFirmwareVersion();

	stimulusVoltage = 0.15f;

	param_mAperVolts = 1.0f; // 50mA/5V=10mA/V

	//Send pulses to channel 1
	pulsePal.setBiphasic(1, 0);
	pulsePal.setPhase1Voltage(1, 5);
	pulsePal.setRestingVoltage(1, 0);
	pulsePal.setPhase1Duration(1, 0.0005f); //0.5ms as per Obreja

	//Send TTL to chan 2
	pulsePal.setBiphasic(2, 0);
	pulsePal.setPhase1Voltage(2, 5);
	pulsePal.setRestingVoltage(2, 0);
	pulsePal.setPhase1Duration(2, 0.0005f);

	pulsePal.setContinuousLoop(1, 1);
	pulsePal.setContinuousLoop(2, 1);

	pulsePal.abortPulseTrains();
	pulsePal.currentOutputParams[1].pulseTrainDuration = 1000.0f;
	pulsePal.currentOutputParams[1].interPulseInterval = 2.0f;

	pulsePal.currentOutputParams[2].pulseTrainDuration = 1000.0f;
	pulsePal.currentOutputParams[2].interPulseInterval = 2.0f;
	pulsePal.currentOutputParams[2].phase1Voltage = 5.0f;

	pulsePal.syncAllParams();

	//ulsePal.triggerChannel(1);
	pulsePal.triggerChannels(1, 1, 0, 0);
	
	/**
	float customVoltages1[2] = { 5,5 };
	float customPulseTimes1[2] = { 0};

	pulsePal.sendCustomPulseTrain(1, 1, customPulseTimes1, customVoltages1);
	pulsePal.setCustomTrainID(2, 2);
	pulsePal.setCustomTrainTarget(2, 0);
	pulsePal.setCustomTrainLoop(2, 0);
	//Configure to fire on trigger channel 1 (from ardu pulse delay simulator)
	pulsePal.setTrigger1Link(1, 0);
	pulsePal.setTrigger1Link(2, 1);
	pulsePal.setTrigger1Link(3, 0);
	pulsePal.setTrigger1Link(4, 0);
	//Set trigger mode to normal
	pulsePal.setTriggerMode(2, 0);
	**/


	protocolStepNumber = -1;

	//Get last opened file path 
	lastFilePath = CoreServices::getDefaultUserSaveDirectory();


	//Populate GUI
	addAndMakeVisible(getFileButton = new UtilityButton("F", Font("Small Text", 13, Font::plain)));
	getFileButton->setButtonText("F:");
	getFileButton->addListener(this);

	addAndMakeVisible(fileName_label = new TextEditor("file label",0));
	fileName_label->setReadOnly(true);
	fileName_label->setText("No file selected", dontSendNotification);

	addAndMakeVisible(protocolStepNumber_label = new TextEditor("protocol_step_number", 0));
	protocolStepNumber_label->setReadOnly(true);
	protocolStepNumber_label->setText("-");

	addAndMakeVisible(protocolRate_label = new TextEditor("protocol_rate", 0));
	protocolRate_label->setReadOnly(true);
	protocolRate_label->setText("-");

	addAndMakeVisible(protocolDuration_label = new TextEditor("protocol_duration", 0));
	protocolDuration_label->setReadOnly(true);
	protocolDuration_label->setText("-");


	addAndMakeVisible(protocolVoltage_label = new TextEditor("protocol_Voltage", 0));
	protocolVoltage_label->setReadOnly(true);
	protocolVoltage_label->setText("-");

	addAndMakeVisible(protocolComment_label = new TextEditor("protocol_Comment", 0));
	protocolComment_label->setReadOnly(true);
	protocolComment_label->setText("-");


	addAndMakeVisible(protocolTimeLeft_label = new TextEditor("protocolTimeLeft", 0));
	protocolTimeLeft_label->setReadOnly(true);
	protocolTimeLeft_label->setText("-");

	addAndMakeVisible(stimulusVoltageSlider = new Slider("Stimulus Voltage"));
	stimulusVoltageSlider->setRange(0.0f, 5.0f, 0.01f);
	stimulusVoltageSlider->setSliderStyle(Slider::LinearVertical);
	stimulusVoltageSlider->setTextBoxStyle(Slider::TextBoxAbove, true, 80, 20);
	stimulusVoltageSlider->addListener(this);

	stimulusVoltage = 5.0f;
	stimulusVoltageSlider->setValue(stimulusVoltage);
	

}

ppController::~ppController()
{
	fileName_label = nullptr;
	getFileButton = nullptr;

	//Disconnect pulsePal
	pulsePal.abortPulseTrains(); //Make sure we stop stimulating!
	pulsePal.updateDisplay("Disconnected", "Click for menu");
	pulsePal.end();
}

void ppController::paint(Graphics& g)
{
	g.setColour(Colours::darkslategrey);
	//g.fillEllipse(getLocalBounds().toFloat());
	g.drawRoundedRectangle(0, 0, 300, 250, 5, 2);
}

void ppController::resized()
{

	getFileButton->setBounds(5, 5, 30, 20);

	fileName_label->setBounds(40, 5, 150, 20);

	//flashingComponentDemo->setBounds(200, 5, 30, 20);

	protocolStepNumber_label->setBounds(5, 25, 200, 20);

	protocolRate_label->setBounds(5, 50, 90, 20);
	protocolDuration_label->setBounds(5, 75, 90, 20);

	protocolVoltage_label->setBounds(5, 100, 90, 20);
	protocolComment_label->setBounds(5, 125, 90, 20);
	protocolTimeLeft_label->setBounds(5, 150, 200, 20);

	//stimulusVoltageSlider->setBounds(250, 5, 55, 200);


}

void ppController::sliderValueChanged(Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		stimulusVoltage = sliderThatWasMoved->getValue();

		pulsePal.setPhase1Voltage(3, stimulusVoltage);
		//pulsePal.currentOutputParams[3].phase1Voltage = stimulusVoltage;
		//pulsePal.syncAllParams();

		/**
		float customVoltages2[2] = { stimulusVoltage };
		float customPulseTimes2[2] = { 0 };
		pulsePal.sendCustomPulseTrain(2, 1, customPulseTimes2, customVoltages2);
		pulsePal.setCustomTrainID(2, 2);
		**/
	}
}

void ppController::setStimulusVoltage(float newVoltage)
{
	//TODO: Check bounds
	stimulusVoltage = newVoltage;

	pulsePal.currentOutputParams[1].phase1Voltage = stimulusVoltage;
	pulsePal.currentOutputParams[2].phase1Voltage = 5.0f;
	pulsePal.syncAllParams();

	//pulsePal.currentOutputParams[3].phase1Voltage = stimulusVoltage;
	//pulsePal.syncAllParams();

	/**
	float customVoltages2[2] = { stimulusVoltage };
	float customPulseTimes2[2] = { 0 };
	pulsePal.sendCustomPulseTrain(2, 1, customPulseTimes2, customVoltages2);
	pulsePal.setCustomTrainID(2, 2);
	**/
}

void ppController::timerCallback(int timerID)
{
	// TODO: Cleanup code
	if (timerID == 1)
	{
		//Stop old Timer
		stopTimer(1);

		//PulsePal Specific
		pulsePal.abortPulseTrains();

		// Increment counter if there are any remaining steps in protocol
		if (protocolStepNumber < (elementCount - 1))
		{
			protocolStepNumber++;

			// Update Labels
			//protocolStepNumber_label->setText(String(protocolStepNumber + 1));
			protocolStepNumber_label->setText(String(protocolStepNumber + 1) + "/" + String(elementCount)
				+ " at " + String(protocolData[protocolStepNumber].rate) + " Hz, "
				+ String(protocolData[protocolStepNumber].voltage) + " V");




			protocolRate_label->setText(String(protocolData[protocolStepNumber].rate));


			protocolVoltage_label->setText(String(protocolData[protocolStepNumber].voltage));
			protocolComment_label->setText(protocolData[protocolStepNumber].comment);

			// Start next timer
			startTimer(1,static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));

			//Set new end Time time
			endingTime = Time::getMillisecondCounter() + static_cast<int>(protocolData[protocolStepNumber].duration * 1000.0f);
			protocolDuration_label->setText(String(protocolData[protocolStepNumber].duration));


			//PulsePalSpecific

			// Get pulse period in s
			float pulsePeriod = (1.0f / protocolData[protocolStepNumber].rate);

			//Send to pulsePal
			//pulsePal.setPulseTrainDuration(1, protocolData[protocolStepNumber].duration); //Max 3600s->1h
			//pulsePal.setInterPulseInterval(1,pulsePeriod);

			pulsePal.abortPulseTrains();
			pulsePal.currentOutputParams[1].pulseTrainDuration = protocolData[protocolStepNumber].duration;
			pulsePal.currentOutputParams[1].interPulseInterval = pulsePeriod;
			pulsePal.syncAllParams();
			pulsePal.triggerChannel(1); //trigger

		}
		else
		{
			// If no more to draw then stop timer
			stopTimer(0);
		}

		//

		//Repaint
		repaint();
	}
	else if (timerID == 0)
	{
		// Calculate time left
		secondsLeft = RelativeTime::milliseconds(endingTime - Time::getMillisecondCounter());

		// Update label
		protocolTimeLeft_label->setText(formatTimeLeftToString(secondsLeft, protocolData[protocolStepNumber].duration));
			
		//Repaint
		repaint();
	}
}

void ppController::buttonClicked(Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == getFileButton) {
		//If loading new file

		//TODO: Think about how this button should behave when there is already a file loaded.
		// TODO: implement drag/drop?
		// TODO: check extensions eg StringArray extensions hardcode for now
		String supportedExtensions = "*.csv";

		FileChooser chooseProtocolFile("Please select protocol file that you want to load",
			lastFilePath,
			supportedExtensions);

		if (chooseProtocolFile.browseForFileToOpen()) {
			loadFile(chooseProtocolFile.getResult().getFullPathName());
		}
	}
}

void ppController::loadFile(String file)//, std::vector<protocolDataElement> csvData)
{
	File fileToRead(file);
	//lastFilePath = fileToRead.getParentDirectory();
	fileName_label->setText(fileToRead.getFileName(), dontSendNotification);

	// TODO: Read line by line
	StringArray protocolData_raw;
	fileToRead.readLines(protocolData_raw);


	//csvData.clear();

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

	for (int ii = 0; ii < elementCount; ii++) {

		StringArray tempData;
		protocolDataElement tempObj;

		tempData = StringArray::fromTokens(protocolData_raw[ii], ",", "\"");


		tempObj.duration = tempData[0].getFloatValue();
		tempObj.voltage = tempData[1].getFloatValue();
		tempObj.comment = tempData[3];


		protocolData.push_back(tempObj);
	}

	//Display info prompt 
	//AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,"Stimulation protocol","File Loaded!");

	protocolStepNumber = 0;


	// Start UI refresh timer
	startTimer(0, 500);

	// Start Protocol Timer
	startTimer(1,static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));


	//PulsePalSpecific
	// Get pulse period in s
	float pulsePeriod = (1.0f / protocolData[protocolStepNumber].rate);

	//Send to pulsePal
	pulsePal.setPulseTrainDuration(1, protocolData[protocolStepNumber].duration); //Max 3600s->1h
	pulsePal.setInterPulseInterval(1, pulsePeriod);
	pulsePal.triggerChannel(1); //trigger

	// Update labels
	protocolStepNumber_label->setText(String(protocolStepNumber + 1)+"/"+String(elementCount)
		+" at "+ String(protocolData[protocolStepNumber].rate)+" Hz, "
		+ String(protocolData[protocolStepNumber].voltage)+" V");

	protocolRate_label->setText(String(protocolData[protocolStepNumber].rate));
	protocolDuration_label->setText(String(protocolData[protocolStepNumber].duration));


	protocolVoltage_label->setText(String(protocolData[protocolStepNumber].voltage));
	protocolComment_label->setText(protocolData[protocolStepNumber].comment);

	// Handle time left
	// End time is current time plus duration in ms
	endingTime = Time::getMillisecondCounter() + static_cast<int>(protocolData[protocolStepNumber].duration * 1000.0f);
}

String ppController::formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration) 
{
	//Format mm:ss/mm:ss
	// TODO: cleanup?
	return (String(static_cast<int>(std::floor(step_secondsRemaining.inMinutes())) % 60).paddedLeft('0', 2)
		+ ":" + String(static_cast<int>(std::floor(step_secondsRemaining.inSeconds())) % 60).paddedLeft('0', 2)
		+ "/" + String(static_cast<int>(std::floor(RelativeTime::RelativeTime(step_duration).inMinutes())) % 60).paddedLeft('0', 2)
		+ ":" + String(static_cast<int>(std::floor(RelativeTime::RelativeTime(step_duration).inSeconds())) % 60).paddedLeft('0', 2));
}