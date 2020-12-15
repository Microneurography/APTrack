/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "LfpLatencyProcessorVisualizerContentComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LfpLatencyProcessorVisualizerContentComponent::LfpLatencyProcessorVisualizerContentComponent ()
: spectrogramImage (Image::RGB, SPECTROGRAM_WIDTH, SPECTROGRAM_HEIGHT, true),searchBoxLocation(150),subsamplesPerWindow(60),startingSample(0),colorStyle(1)
{
    //[Constructor_pre] You can add your own custom stuff here..
    searchBoxLocation = 150;
    conductionDistance = 100;



	// HACK
	stimulusVoltage = 0.0f;
	stimulusVoltageMax = 10.0f; // you also have to change the range of the slider on line 87
	stimulusVoltageMin = 0.5f;

	trackSpike_IncreaseRate = 0.01;
	trackSpike_DecreaseRate = 0.01;


    //[/Constructor_pre]
    
    std::cout << "Pre" << std::endl;
    draw_imageHeight = spectrogramImage.getHeight();
    draw_rightHandEdge = spectrogramImage.getWidth();
    std::cout << "Med" << std::endl;
    
    //Paint image
    for (auto ii = 0; ii< SPECTROGRAM_HEIGHT; ii++)
    {
        for (auto jj = 0; jj<SPECTROGRAM_WIDTH ; jj++)
        {
            spectrogramImage.setPixelAt (draw_rightHandEdge-jj, draw_imageHeight-ii, Colours::yellowgreen);
        }
    }
    
    std::cout << "Post" << std::endl;
    
	// The code for the descriptions is below
	// I think that the labels can have the argument dontSendNotification. Not sure what sending does

    addAndMakeVisible(colorControlGroup = new GroupComponent);
    colorControlGroup->setName(("Color control"));
    
    addAndMakeVisible (imageThresholdSlider = new Slider ("imageThreshold"));
    imageThresholdSlider->setRange (0, 100, 0);
    imageThresholdSlider->setSliderStyle (Slider::ThreeValueVertical);
    imageThresholdSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    imageThresholdSlider->addListener (this);

	addAndMakeVisible(imageThresholdSliderLabel = new Label("Image_Threshold_Slider_Label"));
	imageThresholdSliderLabel->setText("Image Threshold", sendNotification);


	// Stimulus control

	addAndMakeVisible(ppControllerComponent = new ppController());

	addAndMakeVisible(stimulusVoltageSlider = new Slider("stimulusVoltage"));
	stimulusVoltageSlider->setRange(0.0f, 10.0f, 0);
	stimulusVoltageSlider->setSliderStyle(Slider::ThreeValueVertical);
	stimulusVoltageSlider->setTextBoxStyle(Slider::NoTextBox, true, 80, 20);
	stimulusVoltageSlider->addListener(this);
	addAndMakeVisible(stimulusVoltageSliderLabel = new Label("Stimulus_Voltage_Slider_Label"));
	stimulusVoltageSliderLabel->setText("Stimulus Voltage", sendNotification);

	addAndMakeVisible(stimulusVoltageMin_text = new TextEditor("Stimulus Min"));
	stimulusVoltageMin_text->setText(String(stimulusVoltageMin) + " V");
	addAndMakeVisible(stimulusVoltageMin_textLabel = new Label("Stimulus_Voltage_Min_Text_Label"));
	stimulusVoltageMin_textLabel->setText("Stimulus Voltage Min", sendNotification);

	addAndMakeVisible(stimulusVoltageMax_text = new TextEditor("Stimulus Max"));
	stimulusVoltageMax_text->setText(String(stimulusVoltageMax) + " V");
	addAndMakeVisible(stimulusVoltageMax_textLabel = new Label("Stimulus_Voltage_Max_Text_Label"));
	stimulusVoltageMax_textLabel->setText("Stimulus Voltage Max", sendNotification);

	addAndMakeVisible(stimulusVoltage_text = new TextEditor("Stimulus now"));
	stimulusVoltage_text->setText(String(stimulusVoltage) + " V");
	addAndMakeVisible(stimulusVoltage_textLabel = new Label("Stimulus_Voltage_Text_Label"));
	stimulusVoltage_textLabel->setText("Current Stimulus Voltage", sendNotification);
	//

    
    addAndMakeVisible (searchBoxSlider = new Slider ("searchBox"));
    searchBoxSlider->setRange(0, 300, 1);
    searchBoxSlider->setSliderStyle (Slider::LinearVertical);
    searchBoxSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    searchBoxSlider->addListener (this);
	addAndMakeVisible(searchBoxSliderLabel = new Label("Search_Box_Slider_Label"));
	searchBoxSliderLabel->setText("Search Box", sendNotification);
    
    addAndMakeVisible(ROISpikeLatency = new TextEditor("SearchBoxLocationLatency"));
    ROISpikeLatency->setText(String(searchBoxLocation));
	addAndMakeVisible(ROISpikeLatencyLabel = new Label("ROI_Spike_Location_Label"));
	ROISpikeLatencyLabel->setText("ROI Spike Location", sendNotification);
    
    addAndMakeVisible(ROISpikeMagnitude = new TextEditor("SearchBoxLocationSpeed"));
    ROISpikeMagnitude->setText("NaN");
	addAndMakeVisible(ROISpikeMagnitudeLabel = new Label("ROI_Spike_Value_Label"));
	ROISpikeMagnitudeLabel->setText("ROI Spike Value", sendNotification);

    addAndMakeVisible(lowImageThresholdText = new TextEditor("lowImageThreshold"));
	lowImageThresholdText->setText(String(lowImageThreshold) + " uV");
	addAndMakeVisible(lowImageThresholdTextLabel = new Label("Low_Image_Threshold_Text_Label"));
	lowImageThresholdTextLabel->setText("Low Image Threshold", sendNotification);
    
    addAndMakeVisible(highImageThresholdText = new TextEditor("highImageThreshold"));
	highImageThresholdText->setText(String(highImageThreshold) + " uV");
	addAndMakeVisible(highImageThresholdTextLabel = new Label("High_Image_Threshold_Text_Label"));
	highImageThresholdTextLabel->setText("High Image Threshold", sendNotification);
    
    addAndMakeVisible(detectionThresholdText = new TextEditor("spikeDetectionThresholdText"));
    detectionThresholdText->setText(String(detectionThreshold) +" uV");
	addAndMakeVisible(detectionThresholdTextLabel = new Label("Detection_Threshold_Text_Label"));
	detectionThresholdTextLabel->setText("Detection Threshold", sendNotification);
    
    //buffer/window = ssp
    
    addAndMakeVisible (subsamplesPerWindowSlider = new Slider ("subsampleSlider"));
    
    int maxSubsample = std::round(DATA_CACHE_SIZE_SAMPLES/SPECTROGRAM_HEIGHT);
    
    subsamplesPerWindowSlider->setRange (1, maxSubsample, 1); // 100/30000 = 10ms
    subsamplesPerWindowSlider->setSliderStyle (Slider::Rotary);
    subsamplesPerWindowSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    subsamplesPerWindowSlider->addListener (this);
    
	// This makes a label, x and y coordinates are described in function starting online 252
	addAndMakeVisible(subsamplesPerWindowSliderLabel = new Label("Subsamples_Per_Window_Slider_Label"));
	subsamplesPerWindowSliderLabel->setText("Subsamples Per Window", sendNotification);
	

    addAndMakeVisible (startingSampleSlider = new Slider ("startingSampleSlider"));
    startingSampleSlider->setRange (0, 30000, 1);
    startingSampleSlider->setSliderStyle (Slider::Rotary);
    startingSampleSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    startingSampleSlider->addListener (this);
	addAndMakeVisible(startingSampleSliderLabel = new Label("Starting_Sample_Slider_Label"));
	startingSampleSliderLabel->setText("Starting Sample", sendNotification);
    
    addAndMakeVisible (conductionDistanceSlider = new Slider ("conductionDistanceSlider"));
    conductionDistanceSlider->setRange (0, 2000, 1);
    conductionDistanceSlider->setSliderStyle (Slider::Rotary);
    conductionDistanceSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    conductionDistanceSlider->addListener (this);
	addAndMakeVisible(conductionDistanceSliderLabel = new Label("Conduction_Distance_Slider_Label"));
	conductionDistanceSliderLabel->setText("Conduction Distance", sendNotification);
    
    addAndMakeVisible (searchBoxWidthSlider = new Slider ("searchBoxWidthSlider"));
    searchBoxWidthSlider->setRange (1, 30, 1);
    searchBoxWidthSlider->setSliderStyle (Slider::Rotary);
    searchBoxWidthSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    searchBoxWidthSlider->addListener (this);
	addAndMakeVisible(searchBoxWidthSliderLabel = new Label("search_Box_Width_Slider_Label"));
	searchBoxWidthSliderLabel->setText("Search Box Width", sendNotification);
    
    addAndMakeVisible(colorStyleComboBox = new ComboBox("Color style selector"));
    colorStyleComboBox->setEditableText(false);
    colorStyleComboBox->setJustificationType(Justification::centredLeft);
    colorStyleComboBox->setTextWhenNothingSelected(TRANS("WHOT"));
    colorStyleComboBox->addItem("WHOT", 1);
    colorStyleComboBox->addItem("BHOT", 2);
    colorStyleComboBox->addItem("WHOT,PLAIN", 3);
    colorStyleComboBox->addItem("BHOT,PLAIN", 4);
	addAndMakeVisible(colorStyleComboBoxLabel = new Label("Color_Style_Combo_Box_Label"));
	colorStyleComboBoxLabel->setText("Color Style Combination", sendNotification);
    
    addAndMakeVisible(extendedColorScaleToggleButton = new ToggleButton("Extended scale?"));
    extendedColorScaleToggleButton->addListener(this);
	extendedColorScaleToggleButton->setColour(ToggleButton::ColourIds::tickDisabledColourId,Colours::lightgrey);

	addAndMakeVisible(textBox2 = new TextEditor("selectedDataChanText"));
	textBox2->setText("Data");

	addAndMakeVisible(textBox1 = new TextEditor("selectedTriggerChanText"));
	textBox1->setText("Trigger");


	addAndMakeVisible(triggerChannelComboBox = new ComboBox("Trigger Channel"));
	triggerChannelComboBox->setEditableText(false);
	triggerChannelComboBox->setJustificationType(Justification::centredLeft);
	triggerChannelComboBox->setTextWhenNothingSelected(TRANS("None"));
	triggerChannelComboBox->addSectionHeading("Trigger");
	addAndMakeVisible(triggerChannelComboBoxLabel = new Label("Trigger_Channel_Combo_Box_Label"));
	triggerChannelComboBoxLabel->setText("Trigger Channel", sendNotification);

	addAndMakeVisible(dataChannelComboBox = new ComboBox("Data Channel"));
	dataChannelComboBox->setEditableText(false);
	dataChannelComboBox->setJustificationType(Justification::centredLeft);
	dataChannelComboBox->setTextWhenNothingSelected(TRANS("None"));
	dataChannelComboBox->addSectionHeading("Data");
	addAndMakeVisible(dataChannelComboBoxLabel = new Label("Data_Channel_Combo_Box_Label"));
	dataChannelComboBoxLabel->setText("Data Channel", sendNotification);

	addAndMakeVisible(trackSpike_button = new ToggleButton("Track spike"));
	trackSpike_button->addListener(this);
	trackSpike_button->setToggleState(false, sendNotification);
	trackSpike_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);

	addAndMakeVisible(trackThreshold_button = new ToggleButton("Track threshold"));
	trackThreshold_button->addListener(this);
	trackThreshold_button->setToggleState(false, sendNotification);
	trackThreshold_button->setEnabled(trackSpike_button->getToggleState());
	trackThreshold_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);

	addAndMakeVisible(trackSpike_IncreaseRate_Slider = new Slider("searchBoxWidthSlider"));
	trackSpike_IncreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_IncreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_IncreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_IncreaseRate_Slider->addListener(this);
	trackSpike_IncreaseRate_Slider->setValue(0.01f);
	addAndMakeVisible(trackSpike_IncreaseRate_Slider_Label = new Label("Track_Spike_Increase_Rate_Slider_Label"));
	trackSpike_IncreaseRate_Slider_Label->setText("Increase Rate of Spike Tracking", sendNotification);


	addAndMakeVisible(trackSpike_DecreaseRate_Slider = new Slider("searchBoxWidthSlider"));
	trackSpike_DecreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_DecreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_DecreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_DecreaseRate_Slider->addListener(this);
	trackSpike_DecreaseRate_Slider->setValue(0.01f);
	addAndMakeVisible(trackSpike_DecreaseRate_Slider_Label = new Label("Track_Spike_Decrease_Rate_Slider_Label"));
	trackSpike_DecreaseRate_Slider_Label->setText("Decrease Rate of Spike Tracking", sendNotification);

	addAndMakeVisible(trackSpike_IncreaseRate_Text = new TextEditor("trackSpike_IncreaseRate_Text"));
	trackSpike_IncreaseRate_Text->setText("+"+String(trackSpike_IncreaseRate_Slider->getValue(),0) + " V");

	addAndMakeVisible(trackSpike_DecreaseRate_Text = new TextEditor("trackSpike_DecreaseRate_Text"));
	trackSpike_DecreaseRate_Text->setText("-"+String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");

	//Debug
	addAndMakeVisible(trigger_threshold_Slider = new Slider("trigger_threshold_Slider"));
	trigger_threshold_Slider->setRange(0.1f, 150.0f, 0);
	trigger_threshold_Slider->setSliderStyle(Slider::Rotary);
	trigger_threshold_Slider->setTextBoxStyle(Slider::TextBoxRight, false, 80, 20);
	trigger_threshold_Slider->addListener(this);
	trigger_threshold_Slider->setValue(2.5f);
	addAndMakeVisible(trigger_threshold_Slider_Label = new Label("Trigger_Threshold_Slider_Label"));
	trigger_threshold_Slider_Label->setText("Trigger Threshold", sendNotification);


    
    addAndMakeVisible(msLabel = new Label("ms_label"));
    msLabel->setText ("ms", dontSendNotification);
    
    addAndMakeVisible(msLabel = new Label("ms_label"));
    msLabel->setText ("ms", dontSendNotification);
    
    addAndMakeVisible(cmLabel = new Label("cm_label"));
    cmLabel->setText ("cm", dontSendNotification);
    
    addAndMakeVisible(mpersLabel = new Label("mpers_label"));
    mpersLabel->setText ("m/s", dontSendNotification);
   
    
    
    
    
    
    //[UserPreSize]

    imageThresholdSlider->setMinValue(0.0f);
    imageThresholdSlider->setMaxValue(90.0f);
    imageThresholdSlider->setValue(50.0f);

	stimulusVoltageSlider->setMinValue(stimulusVoltageMin);
	stimulusVoltageSlider->setMaxValue(stimulusVoltageMax);
	stimulusVoltageSlider->setValue(stimulusVoltage);

	searchBoxSlider->setValue(10.0f);

    subsamplesPerWindowSlider->setValue(1);
    startingSampleSlider->setValue(0);
    
    colorStyleComboBox->setSelectedId(1);
    searchBoxWidthSlider->setValue(3);
    
    extendedColorScaleToggleButton->setToggleState(false,sendNotification);
    //[/UserPreSize]

    setSize (700, 900);
    
    spikeDetected = false;


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

LfpLatencyProcessorVisualizerContentComponent::~LfpLatencyProcessorVisualizerContentComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    imageThresholdSlider = nullptr;
    searchBoxSlider = nullptr;
    subsamplesPerWindowSlider = nullptr;
    startingSampleSlider = nullptr;
    searchBoxWidthSlider=nullptr;
    colorStyleComboBox = nullptr;
    colorControlGroup = nullptr;
	stimulusVoltageSlider = nullptr;

	textBox1 = nullptr;
	textBox2 = nullptr;

	triggerChannelComboBox = nullptr;
	dataChannelComboBox = nullptr;

	trackThreshold_button = nullptr;
	trackSpike_button = nullptr;

	trackSpike_IncreaseRate_Slider = nullptr;
	trackSpike_DecreaseRate_Slider = nullptr;

	trackSpike_IncreaseRate_Text = nullptr;
	trackSpike_DecreaseRate_Text = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LfpLatencyProcessorVisualizerContentComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    g.fillAll (Colours::grey);
    g.setOpacity (1.0f);
    g.drawImage(spectrogramImage, 0,0,SPECTROGRAM_WIDTH,SPECTROGRAM_HEIGHT,0,0,SPECTROGRAM_WIDTH,SPECTROGRAM_HEIGHT);
    //Note, drawImage handles rescaling!
    //[/UserPrePaint]
    
	if (trackSpike_button->getToggleState() == true)
	{
		if (spikeDetected) {
			g.setColour(Colours::green);
		}
		else {
			g.setColour(Colours::red);
		}
	}
	else
	{
		g.setColour(Colours::lightyellow);
	}
    
    g.drawRoundedRectangle(300-8, 300-(searchBoxLocation+searchBoxWidth),8, searchBoxWidth*2+1,1,2);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
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
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    imageThresholdSlider->setBounds (360, 24, 55, 264);
	imageThresholdSliderLabel->setBounds(348, 270, 80, 50); // opposite to the instructions above - got moved in the rebase
   
    highImageThresholdText->setBounds (424, 24, 55, 24);
	highImageThresholdTextLabel->setBounds(477, 24, 160, 25); // opposite to the instructions above
    
    lowImageThresholdText->setBounds (424, 72, 55, 24);
	lowImageThresholdTextLabel->setBounds(477, 72, 160, 25); // opposite to the instructions above
    
    detectionThresholdText->setBounds(424, 48, 55, 24);
	detectionThresholdTextLabel->setBounds(477, 48, 160, 25); // opposite to the instructions above
    
    searchBoxSlider->setBounds (295, 0, 15, 300);
	searchBoxSliderLabel->setBounds(269, 281, 80, 50); // x value is inverted
    
    subsamplesPerWindowSlider->setBounds(424, 152, 159, 64);
	subsamplesPerWindowSliderLabel->setBounds(347, 159, 80, 50);
    
    startingSampleSlider->setBounds(424, 224, 159, 64);
	startingSampleSliderLabel->setBounds(347, 230, 80, 50); // x value is inverted
    
    colorStyleComboBox->setBounds(424, 112, 120, 24);
	colorStyleComboBoxLabel->setBounds(424, 92, 120, 24); 

	colorControlGroup->setBounds(344, 0, 248, 304); // the rectangle in the gui - doesn't need a label

    extendedColorScaleToggleButton->setBounds(500, 24, 80, 24); // has label
    
    searchBoxWidthSlider->setBounds(500, 56, 50, 50);
	searchBoxWidthSliderLabel->setBounds(550, 57, 60, 45); // might need moving down a little, hard to tell when it's a mess
    
	ROISpikeLatencyLabel->setBounds(240, 336, 120, 24); 
    ROISpikeLatency->setBounds(360, 336, 72, 24);
    msLabel->setBounds(432, 336, 72, 24);	// this is a label for the units used 
    
	ROISpikeMagnitudeLabel->setBounds(256.75, 360, 120, 24); // not in line with the label above it and this angers me greatly, but 257 is too much, 256 is too little, there is no sweet spot
    ROISpikeMagnitude->setBounds(360, 360, 72, 24);
    mpersLabel->setBounds(432, 360, 72, 24); // this is a label for the units used
    
    conductionDistanceSlider->setBounds(360, 456, 159, 64);
	conductionDistanceSliderLabel->setBounds(296, 456, 79, 64); // x inverted

	// Stimulus
	ppControllerComponent->setBounds(520, 400, 402, 350); // Don't think this needs a label

	stimulusVoltageSlider->setBounds(600, 0, 55, 264);
	stimulusVoltageSliderLabel->setBounds(590, 250, 80, 50);

	stimulusVoltageMin_text->setBounds(664, 72, 55, 24);
	stimulusVoltageMin_textLabel->setBounds(719, 72, 105, 24); // x inverted

	stimulusVoltage_text->setBounds(664, 48, 55, 24);
	stimulusVoltage_textLabel->setBounds(719, 48, 115, 24);  // x inverted

	stimulusVoltageMax_text->setBounds(664, 24, 55, 24);
	stimulusVoltageMax_textLabel->setBounds(719, 24, 105, 24);  // x inverted 

	// Threshold trigger control
	trigger_threshold_Slider->setBounds(30, 400, 159, 64);
	trigger_threshold_Slider_Label->setBounds(15, 400, 79, 64); // in a good place, the slider itself needs to move


	// channel control
	//textBox1->setBounds(10, 320, 72, 24);
	//textBox2->setBounds(10, 350, 72, 24);

	triggerChannelComboBox->setBounds(120, 320, 72, 24);
	triggerChannelComboBoxLabel->setBounds(28, 320, 92, 24);

	dataChannelComboBox->setBounds(120, 350, 72, 24);
	dataChannelComboBoxLabel->setBounds(48, 350, 72, 24); // fine

	trackSpike_button->setBounds(360, 394, 120, 24); // has a label

	trackThreshold_button->setBounds(360, 428, 120, 24); // has a label

	trackSpike_IncreaseRate_Slider_Label->setBounds(800, 100, 60, 70); // x inverted
	trackSpike_IncreaseRate_Slider->setBounds(690, 106, 159, 64);
	trackSpike_IncreaseRate_Text->setBounds(660, 106, 72, 24);

	trackSpike_DecreaseRate_Slider_Label->setBounds(800, 178, 60, 70); // x inverted
	trackSpike_DecreaseRate_Slider->setBounds(690, 180, 159, 64);
	trackSpike_DecreaseRate_Text->setBounds(660, 180, 72, 24);

	
    //[UserResized]
    //[/UserResized]
}

void LfpLatencyProcessorVisualizerContentComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		//Lower value
		stimulusVoltageMin = sliderThatWasMoved->getMinValue();
		stimulusVoltageMin_text->setText(String(stimulusVoltageMin,2));

		//Upper value
		stimulusVoltageMax = sliderThatWasMoved->getMaxValue();
		stimulusVoltageMax_text->setText(String(stimulusVoltageMax,2));

		//mid value
		stimulusVoltage = sliderThatWasMoved->getValue();
		stimulusVoltage_text->setText(String(stimulusVoltage,2));

		// very slow for some reason
		if (stimulusVoltage > 4 && alreadyAlerted == false) {
			// makes an alert window, returns true if user is okay with it being high
			voltageTooHighOkay = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::WarningIcon, "Voltage Could Be Too High", "Are you sure you want to set the voltage this high?", "Yes", "No");
			alreadyAlerted = true;
		}
		if (voltageTooHighOkay) {
			ppControllerComponent->setStimulusVoltage(stimulusVoltage);
		}
	}
    if (sliderThatWasMoved == imageThresholdSlider)
    {
        //[UserSliderCode_imageThresholdSlider] -- add your slider handling code here..

        //Lower value
        lowImageThreshold = sliderThatWasMoved->getMinValue();
        std::cout << "Slider lower: " << lowImageThreshold << std::endl;
        lowImageThresholdText->setText(String(lowImageThreshold,1)+" uV");

        //Upper value
        highImageThreshold = sliderThatWasMoved->getMaxValue();
        std::cout << "Slider upper: " << highImageThreshold << std::endl;
        highImageThresholdText->setText(String(highImageThreshold,1) + " uV");
        
        //mid value
        detectionThreshold = sliderThatWasMoved->getValue();
        std::cout << "DetectionThehold" << detectionThreshold << std::endl;
        detectionThresholdText->setText(String(detectionThreshold,1) + " uV");

        //sliderThatWasMoved.getMinValue (1.0 / sliderThatWasMoved.getValue(), dontSendNotification);
        //[/UserSliderCode_imageThresholdSlider]
    }
    if (sliderThatWasMoved == searchBoxSlider)
    {

        searchBoxLocation = sliderThatWasMoved->getValue();
        std::cout << "searchBoxLocation" << searchBoxLocation << std::endl;
        
    }
    if (sliderThatWasMoved == subsamplesPerWindowSlider)
    {
        
        //auto subsamplesPerWindowOld = subsamplesPerWindow;
        subsamplesPerWindow = sliderThatWasMoved->getValue();
        std::cout << "subsamplesPerWindow" << searchBoxLocation << std::endl;
      
    }
    if (sliderThatWasMoved == startingSampleSlider)
    {
        
        startingSample = sliderThatWasMoved->getValue();
        std::cout << "startingSample" << startingSample << std::endl;
        
    }
    if (sliderThatWasMoved == searchBoxWidthSlider)
    {
        
        searchBoxWidth = sliderThatWasMoved->getValue();
        std::cout << "searchBoxWidth" << searchBoxWidth << std::endl;
        
    }
	if (sliderThatWasMoved == trackSpike_IncreaseRate_Slider)
	{
		trackSpike_IncreaseRate = sliderThatWasMoved->getValue();
		trackSpike_IncreaseRate_Text->setText("+" + String(trackSpike_IncreaseRate_Slider->getValue(), 0)+ " V");
	}
	if (sliderThatWasMoved == trackSpike_DecreaseRate_Slider)
	{
		trackSpike_DecreaseRate = sliderThatWasMoved->getValue();
		trackSpike_DecreaseRate_Text->setText("-" + String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	}

    
    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
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
            imageThresholdSlider->setRange (0, 1000, 0);
        }
        else {
            // If using regular scale (eg when using FPGA real time data)
            imageThresholdSlider->setRange (0, 100, 0);
        }
    }
	if (buttonThatWasClicked == trackSpike_button)
	{
		if (buttonThatWasClicked->getToggleState() == true) {
			trackThreshold_button->setEnabled(true);
		}
		else if (buttonThatWasClicked->getToggleState() == false)
		{
			trackThreshold_button->setEnabled(false);
			trackThreshold_button->setToggleState(false, sendNotification);
		}
	}
}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LfpLatencyProcessorVisualizerContentComponent"
                 componentName="" parentClasses="public Component" constructorParams=""
                 variableInitialisers="spectrogramImage (Image::RGB, SPECTROGRAM_WIDTH, FIFO_BUFFER_SIZE, true)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="700" initialHeight="900">
  <BACKGROUND backgroundColour="ffffff">
    <ROUNDRECT pos="544 8 104 296" cornerSize="10" fill="solid: fff0f8ff" hasStroke="0"/>
  </BACKGROUND>
  <SLIDER name="imageThreshold" id="170175c6caff4f98" memberName="imageThresholdSlider"
          virtualName="" explicitFocusOrder="0" pos="568 16 55 272" min="0"
          max="1000" int="0" style="TwoValueVertical" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
