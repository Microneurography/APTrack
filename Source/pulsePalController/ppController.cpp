#include "ppController.h"

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

	// Initialise variables
	protocolStepNumber = -1;
	protocolDuration = 0.0f;

	stimulusVoltage = 0.0f;
	pulsePalConnected = false;

	while (AlertWindow::showOkCancelBox(juce::AlertWindow::AlertIconType::WarningIcon, "PulsePal not connected", "A PulsePal could not be found", "Search again", "Continue without PulsePal"))
	{

		pulsePal.initialize();
		pulsePal.setDefaultParameters();
		pulsePal.updateDisplay("GUI Connected", "Click for Menu");
		pulsePalVersion = pulsePal.getFirmwareVersion();

		if (!(pulsePalVersion == 0))
		{
			pulsePalConnected = true;
			break;
		}
	}

	std::cout << "pulsePalVersion " << pulsePalVersion << std::endl;

	// abort pulse trains (if any)
	pulsePal.abortPulseTrains();

	// configure pulse pal
	pulsePal.currentOutputParams[1].isBiphasic = 0;
	pulsePal.currentOutputParams[1].phase1Voltage = 5.0f;
	pulsePal.currentOutputParams[1].restingVoltage = 0.0f;
	pulsePal.currentOutputParams[1].phase1Duration = 0.0005f;
	pulsePal.currentOutputParams[1].pulseTrainDuration = 1000.0f;
	pulsePal.currentOutputParams[1].interPulseInterval = 2.0f;
	pulsePal.currentOutputParams[1].phase1Voltage = 0.0f;

	pulsePal.currentOutputParams[2].isBiphasic = 0;
	pulsePal.currentOutputParams[2].phase1Voltage = 5.0f;
	pulsePal.currentOutputParams[2].restingVoltage = 0.0f;
	pulsePal.currentOutputParams[2].phase1Duration = 0.0005f;
	pulsePal.currentOutputParams[2].pulseTrainDuration = 1000.0f;
	pulsePal.currentOutputParams[2].interPulseInterval = 2.0f;
	pulsePal.currentOutputParams[2].phase1Voltage = 0.0f;

	pulsePal.syncAllParams();

	//Get last opened file path
	lastFilePath = CoreServices::getDefaultUserSaveDirectory();

	//Populate GUI
	addAndMakeVisible(getFileButton = new UtilityButton("F", Font("Small Text", 13, Font::plain)));
	getFileButton->setButtonText("F:");
	getFileButton->addListener(this);

	if (pulsePalConnected)
	{
		getFileButton->setEnabled(true);
	}
	else
	{
		getFileButton->setEnabled(false);
	}

	addAndMakeVisible(fileName_text = new TextEditor("fileName_text", 0));
	fileName_text->setReadOnly(true);
	fileName_text->setText("No file selected", dontSendNotification);

	addAndMakeVisible(protocolStepSummary_text = new TextEditor("protocolStepSummary_text", 0));
	protocolStepSummary_text->setReadOnly(true);
	protocolStepSummary_text->setText("-", dontSendNotification);

	addAndMakeVisible(protocolStepSummary_label = new Label("protocolStepSummary_label"));
	protocolStepSummary_label->setText("Step summary", dontSendNotification);

	addAndMakeVisible(protocolTimeLeft_text = new TextEditor("protocolTimeLeft_text", 0));
	protocolTimeLeft_text->setReadOnly(true);
	protocolTimeLeft_text->setText("-", dontSendNotification);

	addAndMakeVisible(protocolTimeLeft_label = new Label("protocolTimeLeft_label"));
	protocolTimeLeft_label->setText("Total time left", dontSendNotification);

	addAndMakeVisible(protocolStepTimeLeft_text = new TextEditor("protocolStepTimeLeft_text", 0));
	protocolStepTimeLeft_text->setReadOnly(true);
	protocolStepTimeLeft_text->setText("-", dontSendNotification);

	addAndMakeVisible(protocolStepTimeLeft_label = new Label("protocolStepTimeLeft_label"));
	protocolStepTimeLeft_label->setText("Step time left", dontSendNotification);

	addAndMakeVisible(protocolStepComment_text = new TextEditor("protocolStepComment_text", 0));
	protocolStepComment_text->setReadOnly(true);
	protocolStepComment_text->setText("-", dontSendNotification);

	addAndMakeVisible(protocolStepComment_label = new Label("protocolStepComment_label"));
	protocolStepComment_label->setText("Comment", dontSendNotification);
}

ppController::~ppController()
{
	//Disconnect pulsePal
	pulsePal.abortPulseTrains(); //Make sure we stop stimulating!
	pulsePal.updateDisplay("Disconnected", "Click for menu");
	pulsePal.end();
}

void ppController::paint(Graphics &g)
{
	g.setColour(Colours::darkslategrey);
	g.drawRoundedRectangle(0, 0, 305, 130, 5, 1);
}

void ppController::paintOverChildren(Graphics &g)
{
	if (!pulsePalConnected)
	{
		g.setColour(Colours::red);

		juce::Line<float> line1(juce::Point<float>(5, 5),
								juce::Point<float>(300, 125));

		juce::Line<float> line2(juce::Point<float>(5, 125),
								juce::Point<float>(300, 5));

		g.drawLine(line1, 5.0f);
		g.drawLine(line2, 5.0f);
	}
}

void ppController::resized()
{

	getFileButton->setBounds(5, 5, 30, 20);
	fileName_text->setBounds(45, 5, 255, 20);

	protocolStepSummary_text->setBounds(5, 30, 150, 20);
	protocolStepSummary_label->setBounds(160, 30, 150, 20);

	protocolTimeLeft_text->setBounds(5, 55, 150, 20);
	protocolTimeLeft_label->setBounds(160, 55, 150, 20);

	protocolStepTimeLeft_text->setBounds(5, 80, 150, 20);
	protocolStepTimeLeft_label->setBounds(160, 80, 150, 20);

	protocolStepComment_text->setBounds(5, 105, 150, 20);
	protocolStepComment_label->setBounds(160, 105, 150, 20);
}

void ppController::setStimulusVoltage(float newVoltage)
{
	//TODO: Check bounds
	stimulusVoltage = newVoltage;

	//Update channel voltages
	std::cout << "New stimulus voltage " << stimulusVoltage << std::endl;

	pulsePal.currentOutputParams[1].phase1Voltage = stimulusVoltage;
	pulsePal.currentOutputParams[2].phase1Voltage = 5.0f;
	pulsePal.syncAllParams();
}

void ppController::StopCurrentProtocol()
{
	// clear old protocol data, stop old timers
	protocolData.clear();

	protocolDuration = 0.0f;
	protocolStepNumber = -1;

	stopTimer(0); // UI timer
	stopTimer(1); // protocol timer

	//abort pulse train
	pulsePal.abortPulseTrains();
}

void ppController::timerCallback(int timerID)
{
	if (timerID == 1) // Protocol step timer
	{
		//Stop old Timer
		stopTimer(1);

		//PulsePal Specific
		pulsePal.abortPulseTrains();

		// Increment counter if there are any remaining steps in protocol
		if (protocolStepNumber < (elementCount - 1))
		{
			protocolStepNumber++;

			// Start next timer
			startTimer(1, static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));

			// Update protocol step end time
			int64 millisecondsNow = Time::getMillisecondCounter();

			protocolStepEndingTime = millisecondsNow + static_cast<int>(protocolData[protocolStepNumber].duration * 1000.0f);

			//send to pulse pal
			sendProtocolStepToPulsePal(protocolData[protocolStepNumber]);
		}
		else
		{
			// If no more steps then stop timer
			stopTimer(0);

			// update label and prompt info window
			protocolStepSummary_text->setText("Protocol completed");
			protocolStepComment_text->setText("-");
			protocolStepTimeLeft_text->setText("-");

			AlertWindow::showMessageBoxAsync(juce::AlertWindow::AlertIconType::InfoIcon, "Protocol info", "Protocol complete");
		}

		//Repaint
		repaint();
	}
	else if (timerID == 0) //UI refresh timer
	{
		// Calculate time left
		int64 millisecondsNow = Time::getMillisecondCounter();

		RelativeTime protocolStepTimeLeft = RelativeTime::milliseconds(protocolStepEndingTime - millisecondsNow);
		RelativeTime protocolTimeLeft = RelativeTime::milliseconds(protocolEndingTime - millisecondsNow);

		// Update labels

		// time labels
		protocolStepTimeLeft_text->setText(formatTimeLeftToString(protocolStepTimeLeft, protocolData[protocolStepNumber].duration));
		protocolTimeLeft_text->setText(formatTimeLeftToString(protocolTimeLeft, protocolDuration));

		// Update step labels
		protocolStepSummary_text->setText(String(protocolStepNumber + 1) + "/" + String(elementCount) + " at " + String(protocolData[protocolStepNumber].rate) + " Hz");
		protocolStepComment_text->setText(protocolData[protocolStepNumber].comment);

		//Repaint
		repaint();
	}
}

void ppController::buttonClicked(Button *buttonThatWasClicked)
{
	if (buttonThatWasClicked == getFileButton)
	{
		//If loading new file

		//TODO: Think about how this button should behave when there is already a file loaded.
		// TODO: implement drag/drop?
		// TODO: check extensions eg StringArray extensions hardcode for now
		String supportedExtensions = "*.csv";

		FileChooser chooseProtocolFile("Please select protocol file that you want to load",
									   lastFilePath,
									   supportedExtensions);

		if (chooseProtocolFile.browseForFileToOpen())
		{
			loadFile(chooseProtocolFile.getResult().getFullPathName());

			std::cout << "Stim file path: " << chooseProtocolFile.getResult().getFullPathName() << std::endl;
		}
	}
}

void ppController::loadFile(String file) //, std::vector<protocolDataElement> csvData)
{
	File fileToRead(file);
	//lastFilePath = fileToRead.getParentDirectory();
	fileName_text->setText(fileToRead.getFileName(), dontSendNotification);

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

	// Start UI refresh timer
	startTimer(0, 500);

	// Start Protocol Timer
	startTimer(1, static_cast<int>(1000.0f * protocolData[protocolStepNumber].duration));

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

void ppController::sendProtocolStepToPulsePal(protocolDataElement protocolDataStep)
{
	if (protocolDataStep.rate == 0)
	{
		// if rate is 0 the treat as pause and abort pulse trains
		pulsePal.abortPulseTrains();
	}
	else
	{
		// if rate is nonzero then calculate pulse period and send to pulsepal
		float pulsePeriod = (1.0f / protocolDataStep.rate);

		pulsePal.abortPulseTrains();
		pulsePal.currentOutputParams[1].pulseTrainDuration = protocolDataStep.duration; // in sec
		pulsePal.currentOutputParams[1].interPulseInterval = pulsePeriod;				// in sec
		pulsePal.currentOutputParams[2].pulseTrainDuration = protocolDataStep.duration; // in sec
		pulsePal.currentOutputParams[2].interPulseInterval = pulsePeriod;				// in sec

		pulsePal.syncAllParams();

		pulsePal.triggerChannels(1, 1, 0, 0);
	}
}

String ppController::formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration)
{
	//Format mm:ss/mm:ss
	// TODO: cleanup?
	return (String(static_cast<int>(std::floor(step_secondsRemaining.inMinutes())) % 60).paddedLeft('0', 2) + ":" + String(static_cast<int>(std::floor(step_secondsRemaining.inSeconds())) % 60).paddedLeft('0', 2) + "/" + String(static_cast<int>(std::floor(RelativeTime::RelativeTime(step_duration).inMinutes())) % 60).paddedLeft('0', 2) + ":" + String(static_cast<int>(std::floor(RelativeTime::RelativeTime(step_duration).inSeconds())) % 60).paddedLeft('0', 2));
}