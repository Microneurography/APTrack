#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "LfpLatencyProcessor.h"


// So we can make the setup box less dark.
class CustomLookAndFeel : public juce::LookAndFeel_V3 {
public:
	void drawCallOutBoxBackground(CallOutBox& box, Graphics& g, const Path& path, Image& cachedImage) override {
		if (cachedImage.isNull())
		{
			cachedImage = Image(Image::ARGB, box.getWidth(), box.getHeight(), true);
			Graphics g2(cachedImage);

			DropShadow(Colours::darkgrey.withAlpha(1.0f), 8, juce::Point<int>(0, 2)).drawForPath(g2, path);
		}

		g.setColour(Colours::black);
		g.drawImageAt(cachedImage, 0, 0);

		g.setColour(Colour::greyLevel(0.23f).withAlpha(0.9f));
		g.fillPath(path);

		g.setColour(Colours::white.withAlpha(0.8f));
		g.strokePath(path, PathStrokeType(2.0f));
	}

	void drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height,
		float /*sliderPos*/,
		float /*minSliderPos*/,
		float /*maxSliderPos*/,
		const Slider::SliderStyle /*style*/, Slider& slider) override
	{
		const float sliderRadius = (float)(getSliderThumbRadius(slider) - 2);

		const Colour trackColour(slider.findColour(Slider::trackColourId));
		const Colour gradCol1(trackColour.overlaidWith(Colours::lightgrey.withAlpha(slider.isEnabled() ? 0.25f : 0.13f)));
		const Colour gradCol2(trackColour.overlaidWith(Colour(0x14000000)));
		Path indent;

		if (slider.isHorizontal())
		{
			const float iy = y + height * 0.5f - sliderRadius * 0.5f;
			const float ih = sliderRadius;

			g.setGradientFill(ColourGradient(gradCol1, 0.0f, iy,
				gradCol2, 0.0f, iy + ih, false));

			indent.addRoundedRectangle(x - sliderRadius * 0.5f, iy,
				width + sliderRadius, ih,
				5.0f);
		}
		else
		{
			const float ix = x + width * 0.5f - sliderRadius * 0.5f;
			const float iw = sliderRadius;

			g.setGradientFill(ColourGradient(gradCol1, ix, 0.0f,
				gradCol2, ix + iw, 0.0f, false));

			indent.addRoundedRectangle(ix, y - sliderRadius * 0.5f,
				iw, height + sliderRadius,
				5.0f);
		}

		g.fillPath(indent);

		g.setColour(Colour(0x4c000000));
		g.strokePath(indent, PathStrokeType(0.5f));
	}
};


//==============================================================================
LfpLatencyProcessorVisualizerContentComponent::LfpLatencyProcessorVisualizerContentComponent (LfpLatencyProcessor* processor)
: searchBoxLocation(150),subsamplesPerWindow(60),startingSample(0),colorStyle(1)
{
	isSaving = false;

	valuesMap = new unordered_map<string, juce::String>;

    searchBoxLocation = 150;
	searchBoxWidth = 25;
    conductionDistance = 100;
	subsamplesPerWindow = 60;

	setWantsKeyboardFocus(true);

	// HACK
	stimulusVoltage = 0.0f;
	stimulusVoltageMax = 10.0f; // you also have to change the range of the slider on line 87
	stimulusVoltageMin = 0.5f;

	lowImageThreshold = 5.0f;
	highImageThreshold = 80.0f;
	detectionThreshold = 20.0f;

	trackSpike_IncreaseRate = 0.01;
	trackSpike_DecreaseRate = 0.01;

	LfpLatencyProcessor::loadRecoveryData(valuesMap);

	if (valuesMap->find("trackSpike_IncreaseRate") != valuesMap->end())
	{
		trackSpike_IncreaseRate = stof((*valuesMap)["trackSpike_IncreaseRate"].toStdString());
	}

	if (valuesMap->find("trackSpike_DecreaseRate") != valuesMap->end())
	{
		trackSpike_DecreaseRate = stof((*valuesMap)["trackSpike_DecreaseRate"].toStdString());
	}

	if (valuesMap->find("stimulusVoltageMax") != valuesMap->end())
	{
		stimulusVoltageMax = stof((*valuesMap)["stimulusVoltageMax"].toStdString());
	}

	if (valuesMap->find("stimulusVoltageMin") != valuesMap->end())
	{
		stimulusVoltageMin = stof((*valuesMap)["stimulusVoltageMin"].toStdString());
	}

	if (valuesMap->find("stimulusVoltage") != valuesMap->end())
	{
		stimulusVoltage = stof((*valuesMap)["stimulusVoltage"].toStdString());
	}

	if (valuesMap->find("highImageThreshold") != valuesMap->end())
	{
		highImageThreshold = stof((*valuesMap)["highImageThreshold"].toStdString());
	}

	if (valuesMap->find("lowImageThreshold") != valuesMap->end())
	{
		lowImageThreshold = stof((*valuesMap)["lowImageThreshold"].toStdString());
	}

	if (valuesMap->find("detectionThreshold") != valuesMap->end())
	{
		detectionThreshold = stof((*valuesMap)["detectionThreshold"].toStdString());
	}

	if (valuesMap->find("searchBoxLocation") != valuesMap->end())
	{
		searchBoxLocation = stoi((*valuesMap)["searchBoxLocation"].toStdString());
	}

	if (valuesMap->find("searchBoxWidth") != valuesMap->end())
	{
		searchBoxWidth = stoi((*valuesMap)["searchBoxWidth"].toStdString());
	}

	if (valuesMap->find("subsamplesPerWindow") != valuesMap->end())
	{
		subsamplesPerWindow = stoi((*valuesMap)["subsamplesPerWindow"].toStdString());
	}

	if (valuesMap->find("startingSample") != valuesMap->end())
	{
		startingSample = stoi((*valuesMap)["startingSample"].toStdString());
	}

	if (valuesMap->find("extendedColorScale") != valuesMap->end())
	{
		if ((*valuesMap)["extendedColorScale"] == "1")
		{
			extendedColorScale = true;
		}
		else
		{
			extendedColorScale = false;
		}
	}
	else
	{
		extendedColorScale = false;
	}

	// The code for the descriptions is below
	// I think that the labels can have the argument dontSendNotification. Not sure what sending does

	// Stimulus control - setup components
	//
	auto evtChannel =  processor->getEventChannel(0);

	addAndMakeVisible(ppControllerComponent = new ppController());
	ppControllerComponent->setProcessor(processor);

	// Not added here because they appear in the setup box.
	stimulusVoltageSlider = new Slider("stimulusVoltage");
	stimulusVoltageSlider->setRange(0.0f, 10.0f, 0);
	stimulusVoltageSlider->setSliderStyle(Slider::ThreeValueVertical);
	stimulusVoltageSlider->setTextBoxStyle(Slider::NoTextBox, true, 80, 20);
	stimulusVoltageSlider->addListener(this);
	stimulusVoltageSlider->setLookAndFeel(new CustomLookAndFeel);
	stimulusVoltageSlider->setColour(Slider::ColourIds::thumbColourId, Colours::darkgrey);
	stimulusVoltageSliderLabel = new Label("Stimulus_Voltage_Slider_Label");
	stimulusVoltageSliderLabel->setText("Stimulus Voltage", sendNotification);
	stimulusVoltageSliderLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltageMin_text = new TextEditor("Stimulus Min");
	stimulusVoltageMin_text->setText(String(stimulusVoltageMin) + " V");
	stimulusVoltageMin_textLabel = new Label("Stimulus_Voltage_Min_Text_Label");
	stimulusVoltageMin_textLabel->setText("Stimulus Voltage Min", sendNotification);
	stimulusVoltageMin_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltageMax_text = new TextEditor("Stimulus Max");
	stimulusVoltageMax_text->setText(String(stimulusVoltageMax) + " V");
	stimulusVoltageMax_textLabel = new Label("Stimulus_Voltage_Max_Text_Label");
	stimulusVoltageMax_textLabel->setText("Stimulus Voltage Max", sendNotification);
	stimulusVoltageMax_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltage_text = new TextEditor("Stimulus now");
	stimulusVoltage_text->setText(String(stimulusVoltage) + " V");
	stimulusVoltage_textLabel = new Label("Stimulus_Voltage_Text_Label");
	stimulusVoltage_textLabel->setText("Current Stimulus Voltage", sendNotification);
	stimulusVoltage_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	//

    //buffer/window = ssp
    
    //OPTIONS BUTTON-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	colorStyleComboBox = new ComboBox("Color style selector");
    colorStyleComboBox->setEditableText(false);
    colorStyleComboBox->setJustificationType(Justification::centredLeft);
    colorStyleComboBox->setTextWhenNothingSelected(TRANS("WHOT"));
    colorStyleComboBox->addItem("WHOT", 1);
    colorStyleComboBox->addItem("BHOT", 2);
    colorStyleComboBox->addItem("WHOT,PLAIN", 3);
    colorStyleComboBox->addItem("BHOT,PLAIN", 4);
	colorStyleComboBoxLabel = new Label("Color_Style_Combo_Box_Label");
	colorStyleComboBoxLabel->setText("Color Style Combination", sendNotification);
    
    extendedColorScaleToggleButton = new ToggleButton("");
    extendedColorScaleToggleButton->addListener(this);
	extendedColorScaleToggleButton->setColour(ToggleButton::ColourIds::tickDisabledColourId,Colours::lightgrey);
	extendedColorScaleToggleButtonLabel = new Label("Extended_Scale_Toggle_Button_Label");
	extendedColorScaleToggleButtonLabel->setText("Extended Scale", sendNotification);

	triggerChannelComboBox = new ComboBox("Trigger Channel");
	triggerChannelComboBox->setEditableText(false);
	triggerChannelComboBox->setJustificationType(Justification::centredLeft);
	triggerChannelComboBox->setTextWhenNothingSelected(TRANS("None"));
	triggerChannelComboBox->addSectionHeading("Trigger");
	triggerChannelComboBoxLabel = new Label("Trigger_Channel_Combo_Box_Label");
	triggerChannelComboBoxLabel->setText("Trigger Channel", sendNotification);

	dataChannelComboBox = new ComboBox("Data Channel");
	dataChannelComboBox->setEditableText(false);
	dataChannelComboBox->setJustificationType(Justification::centredLeft);
	dataChannelComboBox->setTextWhenNothingSelected(TRANS("None"));
	dataChannelComboBox->addSectionHeading("Data");
	dataChannelComboBoxLabel = new Label("Data_Channel_Combo_Box_Label");
	dataChannelComboBoxLabel->setText("Data Channel", sendNotification);

	trackSpike_button = new ToggleButton("");
	trackSpike_button->addListener(this);
	trackSpike_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);
	trackSpike_button_Label = new Label("track_spike_button_label");
	trackSpike_button_Label->setText("Track Spike", sendNotification);

	trackThreshold_button = new ToggleButton("");
	trackThreshold_button->addListener(this);
	trackThreshold_button->setToggleState(false, sendNotification);
	trackThreshold_button->setEnabled(trackSpike_button->getToggleState());
	trackThreshold_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);
	trackThreshold_button_Label = new Label("track_threshold_button_label");
	trackThreshold_button_Label->setText("Track Threshold", sendNotification);
	trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::darkgrey);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//SETUP BUTTON -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Increase/Decrease rate of spike tracking
	// Not added here as they are in the setup box.
	trackSpike_IncreaseRate_Slider = new Slider("trackSpike_IncreaseRate_Slider");
	trackSpike_IncreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_IncreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_IncreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_IncreaseRate_Slider->addListener(this);
	trackSpike_IncreaseRate_Slider->setValue(trackSpike_IncreaseRate);
	trackSpike_IncreaseRate_Slider_Label = new Label("Track_Spike_Increase_Rate_Slider_Label");
	trackSpike_IncreaseRate_Slider_Label->setText("Increase Rate of Spike Tracking", sendNotification);
	trackSpike_IncreaseRate_Slider_Label->setColour(Label::ColourIds::textColourId, Colours::white);

	trackSpike_DecreaseRate_Slider = new Slider("trackSpike_DecreaseRate_Slider");
	trackSpike_DecreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_DecreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_DecreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_DecreaseRate_Slider->addListener(this);
	trackSpike_DecreaseRate_Slider->setValue(trackSpike_DecreaseRate);
	trackSpike_DecreaseRate_Slider_Label = new Label("Track_Spike_Decrease_Rate_Slider_Label");
	trackSpike_DecreaseRate_Slider_Label->setText("Decrease Rate of Spike Tracking", sendNotification);
	trackSpike_DecreaseRate_Slider_Label->setColour(Label::ColourIds::textColourId, Colours::white);

	trackSpike_IncreaseRate_Text = new TextEditor("trackSpike_IncreaseRate_Text");
	trackSpike_IncreaseRate_Text->setText("+"+String(trackSpike_IncreaseRate_Slider->getValue(),0) + " V");

	trackSpike_DecreaseRate_Text = new TextEditor("trackSpike_DecreaseRate_Text");
	trackSpike_DecreaseRate_Text->setText("-"+String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	//


	stimulusVoltageSlider = new Slider("stimulusVoltage");
	stimulusVoltageSlider->setRange(0.0f, 10.0f, 0);
	stimulusVoltageSlider->setSliderStyle(Slider::ThreeValueVertical);
	stimulusVoltageSlider->setTextBoxStyle(Slider::NoTextBox, true, 80, 20);
	stimulusVoltageSlider->addListener(this);
	stimulusVoltageSlider->setLookAndFeel(new CustomLookAndFeel);
	stimulusVoltageSlider->setColour(Slider::ColourIds::thumbColourId, Colours::darkgrey);
	stimulusVoltageSliderLabel = new Label("Stimulus_Voltage_Slider_Label");
	stimulusVoltageSliderLabel->setText("Stimulus Voltage", sendNotification);
	stimulusVoltageSliderLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltageMin_text = new TextEditor("Stimulus Min");
	stimulusVoltageMin_text->setText(String(stimulusVoltageMin) + " V");
	stimulusVoltageMin_textLabel = new Label("Stimulus_Voltage_Min_Text_Label");
	stimulusVoltageMin_textLabel->setText("Stimulus Voltage Min", sendNotification);
	stimulusVoltageMin_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltageMax_text = new TextEditor("Stimulus Max");
	stimulusVoltageMax_text->setText(String(stimulusVoltageMax) + " V");
	stimulusVoltageMax_textLabel = new Label("Stimulus_Voltage_Max_Text_Label");
	stimulusVoltageMax_textLabel->setText("Stimulus Voltage Max", sendNotification);
	stimulusVoltageMax_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	stimulusVoltage_text = new TextEditor("Stimulus now");
	stimulusVoltage_text->setText(String(stimulusVoltage) + " V");
	stimulusVoltage_textLabel = new Label("Stimulus_Voltage_Text_Label");
	stimulusVoltage_textLabel->setText("Current Stimulus Voltage", sendNotification);
	stimulusVoltage_textLabel->setColour(Label::ColourIds::textColourId, Colours::white);

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	//MULTI SPIKE AND THRESHOLD TRACKING------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	spikeTrackerContent = new TableContent();
	addAndMakeVisible(spikeTracker = new TableListBox("Tracked Spikes", spikeTrackerContent));
	spikeTracker->setColour(ListBox::backgroundColourId, Colours::lightgrey);
	spikeTracker->getHeader().addColumn("Spike", 1, 50);
	spikeTracker->getHeader().addColumn("Location", 2, 100);
	spikeTracker->getHeader().addColumn("Firing Proabability", 3, 120);
	spikeTracker->getHeader().addColumn("Select", 4, 50);
	spikeTracker->getHeader().addColumn("Delete", 5, 50);
	spikeTracker->autoSizeAllColumns();
	spikeTracker->updateContent();

	thresholdTrackerContent = new TableContent();
	addAndMakeVisible(thresholdTracker = new TableListBox("Tracked Thresholds", thresholdTrackerContent));
	thresholdTracker->setColour(ListBox::backgroundColourId, Colours::lightgrey);
	thresholdTracker->getHeader().addColumn("Threshold", 1, 100);
	thresholdTracker->getHeader().addColumn("Value", 2, 100);
	thresholdTracker->getHeader().addColumn("Select", 3, 50);
	thresholdTracker->getHeader().addColumn("Delete", 4, 50);
	thresholdTracker->autoSizeAllColumns();
	thresholdTracker->updateContent();

	for (int i = 0; i < 4; i++) {
		addAndMakeVisible(locations[i] = new TextEditor("Location"));
		locations[i]->setText("0");
		addAndMakeVisible(fps[i] = new TextEditor("Firing Probability"));
		fps[i]->setText("0");
		addAndMakeVisible(follows[i] = new ToggleButton(""));
		follows[i]->addListener(this);
		follows[i]->setToggleState(false, sendNotification);
		follows[i]->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);
		addAndMakeVisible(dels[i] = new TextButton(""));
		dels[i]->addListener(this);
		dels[i]->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
		dels[i]->setToggleState(false, sendNotification);
		addAndMakeVisible(ts[i] = new TextEditor("Threshold"));
		ts[i]->setText("0");
		addAndMakeVisible(thresholds[i] = new ToggleButton(""));
		thresholds[i]->addListener(this);
		thresholds[i]->setToggleState(false, sendNotification);
		thresholds[i]->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);
		addAndMakeVisible(tdels[i] = new TextButton(""));
		tdels[i]->addListener(this);
		tdels[i]->setToggleState(false, sendNotification);
		tdels[i]->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
	}
	

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//Debug
    
    addAndMakeVisible(cmLabel = new Label("cm_label"));
    cmLabel->setText ("cm", dontSendNotification);
    
	addAndMakeVisible(textBox2 = new TextEditor("selectedDataChanText"));
	textBox2->setText("Data");

	addAndMakeVisible(textBox1 = new TextEditor("selectedTriggerChanText"));
	textBox1->setText("Trigger");

	stimulusVoltageSlider->setMinValue(stimulusVoltageMin);
	stimulusVoltageSlider->setMaxValue(stimulusVoltageMax);
	stimulusVoltageSlider->setValue(stimulusVoltage);
	stimulusVoltageSlider->setMinValue(stimulusVoltageMin);

    colorStyleComboBox->setSelectedId(1);
    extendedColorScaleToggleButton->setToggleState(false,sendNotification);

    spectrogramControlPanel = new LfpLatencySpectrogramControlPanel(this);
    addAndMakeVisible(spectrogramControlPanel);

	if (valuesMap->find("extendedColorScale") != valuesMap->end())
	{
		if ((*valuesMap)["extendedColorScale"] == "1")
		{
			extendedColorScaleToggleButton->setToggleState(true, sendNotification);
		}
		else
		{
			extendedColorScaleToggleButton->setToggleState(false, sendNotification);
		}
	}
	else
	{
		extendedColorScaleToggleButton->setToggleState(false, sendNotification);
	}

	if (valuesMap->find("trackSpike") != valuesMap->end())
	{
		if ((*valuesMap)["trackSpike"] == "1")
		{
			trackSpike_button->setToggleState(true, sendNotification);
		}
		else
		{
			trackSpike_button->setToggleState(false, sendNotification);
		}
	}
	else
	{
		trackSpike_button->setToggleState(false, sendNotification);
	}
	otherControlPanel = new LfpLatencyOtherControlPanel(this);
	addAndMakeVisible(otherControlPanel);
	otherControlPanel->toBack();

	spectrogramPanel = new LfpLatencySpectrogramPanel(this);
	addAndMakeVisible(spectrogramPanel);
	spectrogramPanel->setSearchBoxValue(searchBoxLocation);
	spectrogramPanel->setSearchBoxWidthValue(searchBoxWidth);

	rightMiddlePanel = new LfpLatencyRightMiddlePanel(this);
	addAndMakeVisible(rightMiddlePanel);

	rightMiddlePanel->setROISpikeLatencyText(String(searchBoxLocation));
	rightMiddlePanel->setROISpikeValueText("NaN");

    setSize (700, 900);
    
    spikeDetected = false;
}

LfpLatencyProcessorVisualizerContentComponent::~LfpLatencyProcessorVisualizerContentComponent()
{
    colorStyleComboBox = nullptr;
	stimulusVoltageSlider = nullptr;

	textBox1 = nullptr;
	textBox2 = nullptr;

	triggerChannelComboBox = nullptr;
	dataChannelComboBox = nullptr;

	trackThreshold_button = nullptr;
	trackSpike_button = nullptr;
	
	spikeTracker = nullptr;
	spikeTrackerContent = nullptr;
	thresholdTracker = nullptr;
	thresholdTrackerContent = nullptr;
	for (int i = 0; i < 4; i++) {
		locations[i] = nullptr;
		fps[i] = nullptr;
		follows[i] = nullptr;
		dels[i] = nullptr;
		ts[i] = nullptr;
		thresholds[i] = nullptr;
		tdels[i] = nullptr;
	}
	

	trackSpike_IncreaseRate_Slider = nullptr;
	trackSpike_DecreaseRate_Slider = nullptr;

	trackSpike_IncreaseRate_Text = nullptr;
	trackSpike_DecreaseRate_Text = nullptr;

	delete valuesMap;
}

//==============================================================================
void LfpLatencyProcessorVisualizerContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
    g.setOpacity (1.0f);

	//Paint is called constatnly, so the cells should be paiting the new number in them


	
	//spikeTracker->autoSizeAllColumns();
	//spikeTracker->updateContent();
	spikeTracker->updateContent();
	thresholdTracker->updateContent();
}

// If you want to move something down, you have to increase the y value
// If you want something to move left, increase the x value
// Sometimes this isn't true, as the coordinates are relative to the top-left of the component's parent
// But I don't know how to find the parent, so this section of code was pain and suffering to make.
// This also means you can't relate the location of one thing to another.
// Please leave all arguments as you found them. Thank you <3
// set bounds argument order is x y width height
void LfpLatencyProcessorVisualizerContentComponent::resized()
{
	auto area = getLocalBounds();

	auto spectrogramPanelWidth = getWidth() * 0.5;
	spectrogramPanel->setBounds(area.removeFromLeft(spectrogramPanelWidth));

	// TODO: these numbers were found in ppController.cpp. Need to change to dynamic;
	auto ppControllerWidth = 305;
	auto ppControllerHeight = 130;

	auto panelHeight = (getHeight() - ppControllerHeight) * 0.5;

	otherControlPanel->setBounds(area.removeFromTop(panelHeight));

	auto middleArea = area.removeFromTop(ppControllerHeight);
	ppControllerComponent->setBounds(middleArea.removeFromLeft(ppControllerWidth));
	rightMiddlePanel->setBounds(middleArea);

	spectrogramControlPanel->setBounds(area);

	// Grace's group
    colorStyleComboBox->setBounds(785, 10, 120, 24);
	colorStyleComboBoxLabel->setBounds(665, 10, 120, 24); 

    extendedColorScaleToggleButton->setBounds(780, 39, 24, 24); 
    extendedColorScaleToggleButtonLabel->setBounds(665, 39, 120, 24); 

	// Lucy's Group

	// channel control
	//textBox1->setBounds(10, 320, 72, 24);
	//textBox2->setBounds(10, 350, 72, 24);

	triggerChannelComboBox->setBounds(785, 68, 120, 24);
	triggerChannelComboBoxLabel->setBounds(665, 68, 120, 24);

	dataChannelComboBox->setBounds(785, 97, 120, 24);
	dataChannelComboBoxLabel->setBounds(665, 97, 120, 24); // fine

	auto boundsMap = otherControlPanel->getTableBounds();
	//trackSpikeComboBox->setBounds(950, 97, 120, 24);
	spikeTracker->setBounds(boundsMap["spikeTracker"]);
	thresholdTracker->setBounds(boundsMap["thresholdTracker"]);
	//spikeTracker->setBounds(665, 40, 470, 200);

	auto STtableX = boundsMap["spikeTracker"].getX();
	auto STtableY = boundsMap["spikeTracker"].getY();

	auto TTtableX = boundsMap["thresholdTracker"].getX();
	auto TTtableY = boundsMap["thresholdTracker"].getY();

	vector<vector<Component*>> tableCells{ 
		{locations[0], locations[1], locations[2], locations[3]},
		{fps[0], fps[1], fps[2], fps[3]},
		{follows[0], follows[1], follows[2], follows[3]},
		{dels[0], dels[1], dels[2], dels[3]},
		{ts[0], ts[1], ts[2], ts[3]},
		{thresholds[0], thresholds[1], thresholds[2], thresholds[3]},
		{tdels[0], tdels[1], tdels[2], tdels[3]}
	};

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tableCells[i][j]->setBounds(spikeTracker->getCellPosition(i + 2, j, false).translated(STtableX, STtableY));
		}
	}

	for (int i = 0; i < 4; i++)
	{
		auto cellArea = spikeTracker->getCellPosition(5, i, false).translated(STtableX, STtableY);
		cellArea = cellArea.withSizeKeepingCentre(40, 18);
		tableCells[3][i]->setBounds(cellArea);
	}

	for (int i = 4; i < 6; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tableCells[i][j]->setBounds(thresholdTracker->getCellPosition(i - 2, j, false).translated(TTtableX, TTtableY));
		}
	}
	for (int i = 0; i < 4; i++)
	{
		auto TcellArea = thresholdTracker->getCellPosition(4, i, false).translated(TTtableX, TTtableY);
		TcellArea = TcellArea.withSizeKeepingCentre(40, 18);
		tableCells[6][i]->setBounds(TcellArea);
	}

	trackSpike_button->setBounds(780, 126, 120, 24);
	trackSpike_button_Label->setBounds(665, 126, 120, 24);

	trackThreshold_button->setBounds(780, 155, 120, 24);
	trackThreshold_button_Label->setBounds(665, 155, 120, 24);
}

bool LfpLatencyProcessorVisualizerContentComponent::keyPressed(const KeyPress& k) {
	//Lucy's style of keybind was much better than mine as it allowed to adjust value and slider position and send a notification in one single line, so thank you <3, from James
	//Increase search box location
	if ((k == KeyPress::rightKey || k == KeyPress::numberPad6) && (searchBoxLocation < SPECTROGRAM_HEIGHT)) {
		spectrogramPanel->changeSearchBoxValue(5);
		return true;
	}
	//Decrease search box location
	else if ((k == KeyPress::leftKey || k == KeyPress::numberPad4) && (searchBoxLocation > 0)) {
		spectrogramPanel->changeSearchBoxValue(-5);
		return true;
	}
	else if (k == KeyPress::F1Key) {
		if (follows[0]->getToggleState() == true) {
			follows[0]->setToggleState(false, sendNotification);
			spikeTracker->selectedRowsChanged(0);
			return true;
		}
		else if (follows[0]->getToggleState() == false) {
			follows[0]->setToggleState(true, sendNotification);
			return true;
		}
	}
	else if (k == KeyPress::F2Key) {
		if (follows[1]->getToggleState() == true) {
			follows[1]->setToggleState(false, sendNotification);
			spikeTracker->selectedRowsChanged(1);
			return true;
		}
		else if (follows[1]->getToggleState() == false) {
			follows[1]->setToggleState(true, sendNotification);
			return true;
		}
	}
	else if (k == KeyPress::F3Key) {
		if (follows[2]->getToggleState() == true) {
			follows[2]->setToggleState(false, sendNotification);
			spikeTracker->selectedRowsChanged(2);
			return true;
		}
		else if (follows[2]->getToggleState() == false) {
			follows[2]->setToggleState(true, sendNotification);
			return true;
		}
	}
	else if (k == KeyPress::F4Key) {
		if (follows[3]->getToggleState() == true) {
			follows[3]->setToggleState(false, sendNotification);
			spikeTracker->selectedRowsChanged(3);
			return true;
		}
		else if (follows[3]->getToggleState() == false) {
			follows[3]->setToggleState(true, sendNotification);
			return true;
		}
	}

	auto subsamplesPerWindowValue = spectrogramControlPanel->getSubsamplesPerWindowValue();
	//Increase subsamplesperwindow
	if ((k.getTextCharacter() == '=' || k.getTextCharacter() == '+' || k == KeyPress::numberPadAdd) && (subsamplesPerWindowValue < spectrogramControlPanel->getSubsamplesPerWindowMaximum())) {
		spectrogramControlPanel->changeSubsamplesPerWindowValue(5);
		return true;
	}
	//Decrease subsamplesperwindow
	else if ((k.getTextCharacter() == '-' || k == KeyPress::numberPadSubtract) && (subsamplesPerWindowValue > spectrogramControlPanel->getSubsamplesPerWindowMinimum())) {
		spectrogramControlPanel->changeSubsamplesPerWindowValue(-5);
		return true;
	}

	auto startingSampleValue = spectrogramControlPanel->getStartingSampleValue();
	//Increase starting sample
	if ((k == KeyPress::upKey || k == KeyPress::numberPad8) && (startingSampleValue < spectrogramControlPanel->getStartingSampleMaximum())) {
		spectrogramControlPanel->changeStartingSampleValue(100);
		return true;
	}
	//Decrease starting sample
	else if ((k == KeyPress::downKey || k == KeyPress::numberPad2) && (startingSampleValue > spectrogramControlPanel->getStartingSampleMinimum())) {
		spectrogramControlPanel->changeStartingSampleValue(-100);
		return true;
	}

	auto highImageThreshold = spectrogramControlPanel->getImageThresholdMaxValue();
	auto lowImageThreshold = spectrogramControlPanel->getImageThresholdMinValue();
	//Increase highImageThreshold
	if ((k == KeyPress::pageUpKey || k == KeyPress::numberPad9) && (highImageThreshold < spectrogramControlPanel->getImageThresholdMaximum())) {
		spectrogramControlPanel->changeImageThresholdMaxValue(2);
		return true;
	}
	//Decrease highImageThreshold
	else if ((k == KeyPress::pageDownKey || k == KeyPress::numberPad3) && (highImageThreshold > spectrogramControlPanel->getImageThresholdMinimum())) {
		spectrogramControlPanel->changeImageThresholdMaxValue(-2);
		return true;
	}
	//Increase lowImageThreshold
	else if ((k == KeyPress::homeKey || k == KeyPress::numberPad7) && (lowImageThreshold < spectrogramControlPanel->getImageThresholdMaximum())) {
		spectrogramControlPanel->changeImageThresholdMinValue(2);
		return true;
	}
	//Decrease lowImageThreshold
	else if ((k == KeyPress::endKey || k == KeyPress::numberPad1) && (lowImageThreshold > spectrogramControlPanel->getImageThresholdMinimum())) {
		spectrogramControlPanel->changeImageThresholdMinValue(-2);
		return true;
	}

	return false;
}


// could pass a bool value to this function for loading in values
// if the bool is false, we are not loading up, run as currently written
// if the bool is true, we are loading up, so do not update the xml
// also need a way of passing the value and connecting to a slider, maybe as just a string and int

void LfpLatencyProcessorVisualizerContentComponent::sliderValueChanged(Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		cout << "Stuck here 1\n";
		//Lower value
		stimulusVoltageMin = sliderThatWasMoved->getMinValue();
		(*valuesMap)["stimulusVoltageMin"] = String(stimulusVoltageMin, 2);
		stimulusVoltageMin_text->setText(String(stimulusVoltageMin, 2));
		cout << "Stuck here 2\n";
		//Upper value
		stimulusVoltageMax = sliderThatWasMoved->getMaxValue();
		(*valuesMap)["stimulusVoltageMax"] = String(stimulusVoltageMax, 2);
		stimulusVoltageMax_text->setText(String(stimulusVoltageMax, 2));
		cout << "Stuck here 3\n";
		//mid value
		stimulusVoltage = sliderThatWasMoved->getValue();

		cout << "Got here\n";
		if (stimulusVoltage > 4 && alreadyAlerted == false) {
			cout << "Made it past the if\n";
			// makes an alert window, returns true if user is okay with it being high
			voltageTooHighOkay = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::WarningIcon, "Voltage Could Be Too High", "Are you sure you want to set the voltage this high?", "Yes", "No");
			cout << "Created Window\n";
			alreadyAlerted = true;
			cout << "already alterted is now true\n";
		}
		if (voltageTooHighOkay || stimulusVoltage < 4) {
			cout << "Made it past the 2nd if\n";
			(*valuesMap)["stimulusVoltage"] = String(stimulusVoltage, 2);
			stimulusVoltage_text->setText(String(stimulusVoltage, 2));
			ppControllerComponent->setStimulusVoltage(stimulusVoltage);
			cout << "Updated stimulus voltage\n";
		}
		cout << "Done\n";
	}
    if (sliderThatWasMoved->getName() == "Image Threshold")
    {
		cout << "Stuck here 4\n";

		//Lower value
		cout << "Stuck here 5\n";
        lowImageThreshold = sliderThatWasMoved->getMinValue();
        std::cout << "Slider lower: " << lowImageThreshold << std::endl;
		(*valuesMap)["lowImageThreshold"] = String(lowImageThreshold, 1);
        spectrogramControlPanel->setLowImageThresholdText(String(lowImageThreshold, 1) + " uV");
		cout << "Stuck here 6\n";
        //Upper value
        highImageThreshold = sliderThatWasMoved->getMaxValue();
        std::cout << "Slider upper: " << highImageThreshold << std::endl;
		(*valuesMap)["highImageThreshold"] = String(highImageThreshold, 1);
        spectrogramControlPanel->setHighImageThresholdText(String(highImageThreshold, 1) + " uV");
		cout << "Stuck here 7\n";
        //mid value
        detectionThreshold = sliderThatWasMoved->getValue();
        std::cout << "DetectionThehold" << detectionThreshold << std::endl;
		(*valuesMap)["detectionThreshold"] = String(detectionThreshold, 1);
        spectrogramControlPanel->setDetectionThresholdText(String(detectionThreshold, 1) + " uV");

        //sliderThatWasMoved.getMinValue (1.0 / sliderThatWasMoved.getValue(), dontSendNotification);
    }
    if (sliderThatWasMoved->getName() == "Search Box")
    {
		cout << "Stuck here 8\n";
        searchBoxLocation = sliderThatWasMoved->getValue();
		(*valuesMap)["searchBoxLocation"] = String(searchBoxLocation);
        std::cout << "searchBoxLocation" << searchBoxLocation << std::endl;
        
    }
    if (sliderThatWasMoved->getName() == "Subsamples Per Window")
    {
		cout << "Stuck here 9\n";
        //auto subsamplesPerWindowOld = subsamplesPerWindow;
        subsamplesPerWindow = sliderThatWasMoved->getValue();
		(*valuesMap)["subsamplesPerWindow"] = String(subsamplesPerWindow);
        std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
      
    }
    if (sliderThatWasMoved->getName() == "Starting Sample")
    {
		cout << "Stuck here 10\n";
        startingSample = sliderThatWasMoved->getValue();
        (*valuesMap)["startingSample"] = String(startingSample);
        std::cout << "startingSample" << startingSample << std::endl;

    }
    if (sliderThatWasMoved->getName() == "Search Box Width")
    {
		cout << "Stuck here 11\n";
		searchBoxWidth = sliderThatWasMoved->getValue();
		(*valuesMap)["searchBoxWidth"] = String(searchBoxWidth);
		std::cout << "searchBoxWidth" << searchBoxWidth << std::endl;

	}
	if (sliderThatWasMoved == trackSpike_IncreaseRate_Slider)
	{
		cout << "Stuck here 12\n";
		trackSpike_IncreaseRate = sliderThatWasMoved->getValue();
		(*valuesMap)["trackSpike_IncreaseRate"] = String(trackSpike_IncreaseRate, 0);
		trackSpike_IncreaseRate_Text->setText("+" + String(trackSpike_IncreaseRate_Slider->getValue(), 0) + " V");
	}
	if (sliderThatWasMoved == trackSpike_DecreaseRate_Slider)
	{
		cout << "Stuck here 13\n";
		trackSpike_DecreaseRate = sliderThatWasMoved->getValue();
		(*valuesMap)["trackSpike_DecreaseRate"] = String(trackSpike_DecreaseRate, 0);
		trackSpike_DecreaseRate_Text->setText("-" + String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	}

	printf("running save custom params\n");
	tryToSave();
}

/*
void LfpLatencyProcessorVisualizerContentComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == nullptr)
    {
        switch (comboBoxThatHasChanged->getSelectedId()) {
            case 1: colorStyle = 1; break;
            case 2: colorStyle = 2; break;
            default: break;
        }
    }
}
 */

/*
void LfpLatencyProcessorVisualizerContentComponent::mouseWheelMove(const MouseEvent &e, const MouseWheelDetails &wheel)
{
    //e.getEventRelativeTo();
    int Ypos = e.getScreenY();
    int deltax = wheel.deltaX;
    int deltay = wheel.deltaY;
    
    subsamplesPerWindow = round(subsamplesPerWindow + 20*((std::abs (wheel.deltaX) > std::abs (wheel.deltaY)? -wheel.deltaX : wheel.deltaY)* (wheel.isReversed ? -1.0f : 1.0f)));
    
    std::cout << "ypos" << Ypos << std::endl;
    
    std::cout << "deltax " << deltax << " deltaY "<< deltay << " totpos "<<subsamplesPerWindow <<std::endl;

    
}
 */


void LfpLatencyProcessorVisualizerContentComponent::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked==extendedColorScaleToggleButton)
    {
        if (buttonThatWasClicked->getToggleState() == true) {
            // If using extended scale (eg when using file reader)
            spectrogramControlPanel->setImageThresholdRange(0, 1000, 0);
			(*valuesMap)["extendedColorScale"] = "1";
        }
        else {
            // If using regular scale (eg when using FPGA real time data)
            spectrogramControlPanel->setImageThresholdRange(0, 100, 0);
			(*valuesMap)["extendedColorScale"] = "0";
        }
    }
	if (buttonThatWasClicked == trackSpike_button)
	{
		if (buttonThatWasClicked->getToggleState() == true) {
			trackThreshold_button->setEnabled(true);
			trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::black);
			(*valuesMap)["trackSpike"] = "1";
		}
		else if (buttonThatWasClicked->getToggleState() == false)
		{
			trackThreshold_button->setEnabled(false);
			trackThreshold_button->setToggleState(false, sendNotification);
			trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::darkgrey);
			(*valuesMap)["trackSpike"] = "0";
		}
	}
	for (int i = 0; i < 4; i++) {
		if (buttonThatWasClicked == dels[i]) {
			deletes[i] = true;
		}
	}
	if (buttonThatWasClicked->getName() == "Setup") {

		Viewport* view = new Viewport("viewTest");
		view->setLookAndFeel(&this->getLookAndFeel());
		view->addAndMakeVisible(stimulusVoltageSlider);
		view->addAndMakeVisible(stimulusVoltageSliderLabel);

		view->addAndMakeVisible(stimulusVoltageMin_text);
		view->addAndMakeVisible(stimulusVoltageMin_textLabel);

		view->addAndMakeVisible(stimulusVoltageMax_text);
		view->addAndMakeVisible(stimulusVoltageMax_textLabel);

		view->addAndMakeVisible(stimulusVoltage_text);
		view->addAndMakeVisible(stimulusVoltage_textLabel);

		view->addAndMakeVisible(trackSpike_IncreaseRate_Slider);
		view->addAndMakeVisible(trackSpike_IncreaseRate_Text);
		view->addAndMakeVisible(trackSpike_IncreaseRate_Slider_Label);

		view->addAndMakeVisible(trackSpike_DecreaseRate_Slider);
		view->addAndMakeVisible(trackSpike_DecreaseRate_Text);
		view->addAndMakeVisible(trackSpike_DecreaseRate_Slider_Label);

		trackSpike_IncreaseRate_Text->setBounds(84, 101, 72, 24);
		trackSpike_IncreaseRate_Slider->setBounds(120, 130, 72, 72);
		trackSpike_IncreaseRate_Slider_Label->setBounds(156, 96, 105, 32);

		trackSpike_DecreaseRate_Text->setBounds(84, 221, 72, 24);
		trackSpike_DecreaseRate_Slider->setBounds(120, 250, 72, 72);
		trackSpike_DecreaseRate_Slider_Label->setBounds(156, 216, 105, 32);

		stimulusVoltageSlider->setBounds(12, 5, 55, 304);
		stimulusVoltageSliderLabel->setBounds(5, 290, 80, 50);

		stimulusVoltageMin_text->setBounds(84, 53, 72, 24);
		stimulusVoltageMin_textLabel->setBounds(156, 53, 105, 24); // x inverted

		stimulusVoltage_text->setBounds(84, 29, 72, 24);
		stimulusVoltage_textLabel->setBounds(156, 29, 115, 24);  // x inverted

		stimulusVoltageMax_text->setBounds(84, 5, 72, 24);
		stimulusVoltageMax_textLabel->setBounds(156, 5, 105, 24);  // x inverted 

		view->setSize(270, 325);

		auto& setupBox = juce::CallOutBox::launchAsynchronously(view, otherControlPanel->getSetupBoundsInPanelParent(), this);
		setupBox.setLookAndFeel(new CustomLookAndFeel());
	}
	if (buttonThatWasClicked->getName() == "Options") {

		Viewport* view = new Viewport("viewTest");
		view->setLookAndFeel(&this->getLookAndFeel());

		view->addAndMakeVisible(colorStyleComboBox);
		view->addAndMakeVisible(colorStyleComboBoxLabel);

		view->addAndMakeVisible(extendedColorScaleToggleButton);
		view->addAndMakeVisible(extendedColorScaleToggleButtonLabel);

		view->addAndMakeVisible(trackSpike_button);
		view->addAndMakeVisible(trackSpike_button_Label);

		view->addAndMakeVisible(trackThreshold_button);
		view->addAndMakeVisible(trackThreshold_button_Label);

		view->addAndMakeVisible(triggerChannelComboBox);
		view->addAndMakeVisible(triggerChannelComboBoxLabel);

		view->addAndMakeVisible(dataChannelComboBox);
		view->addAndMakeVisible(dataChannelComboBoxLabel);

		colorStyleComboBox->setBounds(135, 10, 120, 24);
		colorStyleComboBoxLabel->setBounds(10, 10, 120, 24);

		extendedColorScaleToggleButton->setBounds(135, 40, 24, 24);
		extendedColorScaleToggleButtonLabel->setBounds(10, 40, 120, 24);

		trackSpike_button->setBounds(135, 70, 120, 24);
		trackSpike_button_Label->setBounds(10, 70, 120, 24);

		trackThreshold_button->setBounds(135, 100, 120, 24);
		trackThreshold_button_Label->setBounds(10, 100, 120, 24);

		triggerChannelComboBox->setBounds(135, 130, 120, 24);
		triggerChannelComboBoxLabel->setBounds(10, 130, 120, 24);

		dataChannelComboBox->setBounds(135, 160, 120, 24);
		dataChannelComboBoxLabel->setBounds(10, 160, 120, 24); // fine

		view->setSize(300, 200);

		auto& setupBox = juce::CallOutBox::launchAsynchronously(view, otherControlPanel->getOptionsBoundsInPanelParent(), this);
		setupBox.setLookAndFeel(new CustomLookAndFeel());
	}

	printf("running save custom params\n");
	tryToSave();
}

void LfpLatencyProcessorVisualizerContentComponent::tryToSave()
{
	if (!isSaving)
	{
		isSaving = true;
		LfpLatencyProcessor::saveRecoveryData(valuesMap);
		isSaving = false;
	}
	
}

int TableContent::getNumRows() {
	return 4;
}

TableContent::TableContent() {
		
}

TableContent::~TableContent() {

}

void TableContent::paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {

	g.setColour(Colours::black);  // [5]
	Font font = 12.0f;
	g.setFont(font);

	if (columnId == 1)
	{
		auto text = to_string(rowNumber + 1);

		g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);                             // [6]
	}

	g.setColour(Colours::transparentWhite);
	g.fillRect(width - 1, 0, 1, height);

}

void TableContent::paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {

	if (rowIsSelected) {
		if (rowNumber == 0)
			g.fillAll(Colours::lightsteelblue);
		if (rowNumber == 1)
			g.fillAll(Colours::lightskyblue);
		if (rowNumber == 2)
			g.fillAll(Colours::darkgreen);
		if (rowNumber == 3)
			g.fillAll(Colours::orange);
	}
	else {
		g.fillAll(Colours::lightgrey);
	}
}

int LfpLatencyProcessorVisualizerContentComponent::getStartingSample() const
{
	return startingSample;
}

bool LfpLatencyProcessorVisualizerContentComponent::getExtendedColorScale() const
{
	return extendedColorScale;
}

int LfpLatencyProcessorVisualizerContentComponent::getSubsamplesPerWindow() const
{
	return subsamplesPerWindow;
}

float LfpLatencyProcessorVisualizerContentComponent::getLowImageThreshold() const
{
	return lowImageThreshold;
}

float LfpLatencyProcessorVisualizerContentComponent::getHighImageThreshold() const
{
	return highImageThreshold;
}

float LfpLatencyProcessorVisualizerContentComponent::getDetectionThreshold() const
{
	return detectionThreshold;
}

int LfpLatencyProcessorVisualizerContentComponent::getColorStyleComboBoxSelectedId() const
{
	return colorStyleComboBox->getSelectedId();
}

std::tuple<float, float, float, float, Colour> LfpLatencyProcessorVisualizerContentComponent::getSearchBoxInfo() const
{
	Colour colour;
	if (trackSpike_button->getToggleState() == true)
	{
		if (spikeDetected) {
			colour = Colours::green;
		}
		else {
			colour = Colours::red;
		}
	}
	else if (follows[0]->getToggleState() == true) 
	{
		colour = Colours::lightsteelblue;
	}
	else if (follows[1]->getToggleState() == true) 
	{
		colour = Colours::lightskyblue;
	}
	else if (follows[2]->getToggleState() == true)
	{
		colour = Colours::darkgreen;
	}
	else if (follows[3]->getToggleState() == true)
	{
		colour = Colours::orange;
	}
	else
	{
		colour = Colours::lightyellow;
	}
	
	auto width = 8;
	auto x = spectrogramPanel->getImageWidth() - width;
	auto y = spectrogramPanel->getImageHeight() - (searchBoxLocation + searchBoxWidth);
	auto height = searchBoxWidth * 2 + 1;

	return {x, y, width, height, colour};
}
