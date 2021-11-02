#include "ppControllerVisualizer.h"

/*
	General TODO s:
	- Explore the use of iterators to access dataVector
	TODO get duration as relative time to avoid casting back and forth

*/

/*
	Pulse Pal controller
*/
ppControllerVisualizer::ppControllerVisualizer(LfpLatencyProcessor *processor)
{
	this->processor = processor;
	this->controller = processor->pulsePalController;
	pulsePalConnected = true;
	while (!(controller->initializeConnection()))
	{
		if (!AlertWindow::showOkCancelBox(juce::AlertWindow::AlertIconType::WarningIcon, "PulsePal not connected", "A PulsePal could not be found", "Search again", "Continue without PulsePal"))
		{
			//pulsePalConnected = false;
			break;
		}
	}

	//Get last opened file path
	lastFilePath = CoreServices::getDefaultUserSaveDirectory();

	//Populate GUI
	addAndMakeVisible(getFileButton = new UtilityButton("F", Font("Small Text", 13, Font::plain)));
	getFileButton->setButtonText("F:");
	getFileButton->addListener(this);

	addAndMakeVisible(startStopButton = new UtilityButton(">", Font("Small Text", 13, Font::plain)));
	//startStopButton->setButtonText(">");
	startStopButton->addListener(this);
	startStopButton->setEnabled(false);

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
	startTimer(TIMER_UI, 30); // 30fps
}

ppControllerVisualizer::~ppControllerVisualizer()
{
}

void ppControllerVisualizer::paint(Graphics &g)
{
	g.setColour(Colours::darkslategrey);
	g.drawRoundedRectangle(0, 0, 305, 130, 5, 1);
}

void ppControllerVisualizer::paintOverChildren(Graphics &g)
{
	if (!pulsePalConnected)
	{
		// g.setColour(Colours::red);

		// juce::Line<float> line1(juce::Point<float>(5, 5),
		// 						juce::Point<float>(300, 125));

		// juce::Line<float> line2(juce::Point<float>(5, 125),
		// 						juce::Point<float>(300, 5));

		// g.drawLine(line1, 5.0f);
		// g.drawLine(line2, 5.0f);
	}
}

void ppControllerVisualizer::resized()
{

	getFileButton->setBounds(5, 5, 30, 20);
	fileName_text->setBounds(45, 5, 200, 20);
	startStopButton->setBounds(255, 5, 30, 20);

	protocolStepSummary_text->setBounds(5, 30, 150, 20);
	protocolStepSummary_label->setBounds(160, 30, 150, 20);

	protocolTimeLeft_text->setBounds(5, 55, 150, 20);
	protocolTimeLeft_label->setBounds(160, 55, 150, 20);

	protocolStepTimeLeft_text->setBounds(5, 80, 150, 20);
	protocolStepTimeLeft_label->setBounds(160, 80, 150, 20);

	protocolStepComment_text->setBounds(5, 105, 150, 20);
	protocolStepComment_label->setBounds(160, 105, 150, 20);
}

void ppControllerVisualizer::timerCallback(int timerID)
{

	if (timerID == TIMER_UI) //UI refresh timer
	{
		// Calculate time left
		int64 millisecondsNow = Time::getMillisecondCounter();

		RelativeTime protocolStepTimeLeft = RelativeTime::milliseconds(controller->protocolStepEndingTime - millisecondsNow);
		RelativeTime protocolTimeLeft = RelativeTime::milliseconds(controller->protocolEndingTime - millisecondsNow);

		// // Update labels
		if (controller->isProtocolRunning())
		{
			startStopButton->setLabel("[ ]");
		}
		else
		{
			startStopButton->setLabel(">");
		}

		// // time labels
		if ((controller->protocolData.size() > 0) & (controller->protocolStepNumber >= 0))
		{
			protocolStepTimeLeft_text->setText(formatTimeLeftToString(protocolStepTimeLeft, controller->protocolData[controller->protocolStepNumber].duration));
			protocolTimeLeft_text->setText(formatTimeLeftToString(protocolTimeLeft, controller->protocolDuration));

			// // Update step labels
			protocolStepSummary_text->setText(String(controller->protocolStepNumber + 1) + "/" + String(controller->elementCount) + " at " + String(controller->protocolData[controller->protocolStepNumber].rate) + " Hz");
			protocolStepComment_text->setText(controller->protocolData[controller->protocolStepNumber].comment);
		}
		//Repaint
		repaint();
	}
}

void ppControllerVisualizer::buttonClicked(Button *buttonThatWasClicked)
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
			auto res = chooseProtocolFile.getResult();
			controller->loadFile(res.getFullPathName());
			fileName_text->setText(res.getFileName());
			std::cout << "Stim file path: " << res.getFullPathName() << std::endl;
			startStopButton->setEnabled(true);
		}
		else
		{
			startStopButton->setEnabled(false);
		}
	}
	if (buttonThatWasClicked == startStopButton)
	{
		// start or stop the playlist
		if (controller->isProtocolRunning()) // currently running
		{
			controller->StopCurrentProtocol();
		}
		else
		{ // currently stopped
			controller->StartCurrentProtocol();
		}
	}
}

String ppControllerVisualizer::formatTimeLeftToString(RelativeTime step_secondsRemaining, float step_duration)
{
	//Format mm:ss/mm:ss
	// TODO: cleanup?
	return (String(static_cast<int>(std::floor(step_secondsRemaining.inMinutes())) % 60).paddedLeft('0', 2) +
			":" + String(static_cast<int>(std::floor(step_secondsRemaining.inSeconds())) % 60).paddedLeft('0', 2) +
			"/" + String(static_cast<int>(std::floor(RelativeTime(step_duration).inMinutes())) % 60).paddedLeft('0', 2) + ":" +
			String(static_cast<int>(std::floor(RelativeTime(step_duration).inSeconds())) % 60).paddedLeft('0', 2));
}