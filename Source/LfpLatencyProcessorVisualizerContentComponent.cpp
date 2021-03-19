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
#include "LfpLatencyProcessor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

// So we can make the setup box less dark.
class CustomLookAndFeel : public juce::LookAndFeel_V3 {
public:
	void CallOutBox::LookAndFeelMethods::drawCallOutBoxBackground(CallOutBox& box, Graphics& g, const Path& path, Image& cachedImage) {
		if (cachedImage.isNull())
		{
			cachedImage = Image(Image::ARGB, box.getWidth(), box.getHeight(), true);
			Graphics g2(cachedImage);

			DropShadow(Colours::darkgrey.withAlpha(1.0f), 8, Point<int>(0, 2)).drawForPath(g2, path);
		}

		g.setColour(Colours::black);
		g.drawImageAt(cachedImage, 0, 0);

		g.setColour(Colour::greyLevel(0.23f).withAlpha(0.9f));
		g.fillPath(path);

		g.setColour(Colours::white.withAlpha(0.8f));
		g.strokePath(path, PathStrokeType(2.0f));
	}

	void Slider::LookAndFeelMethods::drawLinearSliderBackground(Graphics& g, int x, int y, int width, int height,
		float /*sliderPos*/,
		float /*minSliderPos*/,
		float /*maxSliderPos*/,
		const Slider::SliderStyle /*style*/, Slider& slider)
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
//[/MiscUserDefs]

//==============================================================================
LfpLatencyProcessorVisualizerContentComponent::LfpLatencyProcessorVisualizerContentComponent ()
: spectrogramImage (Image::RGB, SPECTROGRAM_WIDTH, SPECTROGRAM_HEIGHT, true),searchBoxLocation(150),subsamplesPerWindow(60),startingSample(0),colorStyle(1)
{
    //[Constructor_pre] You can add your own custom stuff here..
    searchBoxLocation = 150;
    conductionDistance = 100;

	setWantsKeyboardFocus(true);

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


	addAndMakeVisible(setupButton = new TextButton("setupButton"));
	setupButton->setButtonText("Setup");
	setupButton->addListener(this);
	setupButton->setColour(TextButton::ColourIds::buttonColourId, Colours::lightgrey);


	// Stimulus control - setup components
	addAndMakeVisible(ppControllerComponent = new ppController());

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

    
    addAndMakeVisible (searchBoxSlider = new Slider ("searchBox"));
    searchBoxSlider->setRange(0, SPECTROGRAM_HEIGHT, 1);
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
    searchBoxWidthSlider->setRange (1, 63, 1);
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
    
    addAndMakeVisible(extendedColorScaleToggleButton = new ToggleButton(""));
    extendedColorScaleToggleButton->addListener(this);
	extendedColorScaleToggleButton->setColour(ToggleButton::ColourIds::tickDisabledColourId,Colours::lightgrey);
	addAndMakeVisible(extendedColorScaleToggleButtonLabel = new Label("Extended_Scale_Toggle_Button_Label"));
	extendedColorScaleToggleButtonLabel->setText("Extended Scale", sendNotification);

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

	addAndMakeVisible(trackSpike_button = new ToggleButton(""));
	trackSpike_button->addListener(this);
	trackSpike_button->setToggleState(false, sendNotification);
	trackSpike_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);
	addAndMakeVisible(trackSpike_button_Label = new Label("track_spike_button_label"));
	trackSpike_button_Label->setText("Track Spike", sendNotification);

	addAndMakeVisible(trackThreshold_button = new ToggleButton(""));
	trackThreshold_button->addListener(this);
	trackThreshold_button->setToggleState(false, sendNotification);
	trackThreshold_button->setEnabled(trackSpike_button->getToggleState());
	trackThreshold_button->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);
	addAndMakeVisible(trackThreshold_button_Label = new Label("track_threshold_button_label"));
	trackThreshold_button_Label->setText("Track Threshold", sendNotification);
	trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::darkgrey);


	// Increase/Decrease rate of spike tracking
	// Not added here as they are in the setup box.
	trackSpike_IncreaseRate_Slider = new Slider("searchBoxWidthSlider");
	trackSpike_IncreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_IncreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_IncreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_IncreaseRate_Slider->addListener(this);
	trackSpike_IncreaseRate_Slider->setValue(0.01f);
	trackSpike_IncreaseRate_Slider_Label = new Label("Track_Spike_Increase_Rate_Slider_Label");
	trackSpike_IncreaseRate_Slider_Label->setText("Increase Rate of Spike Tracking", sendNotification);
	trackSpike_IncreaseRate_Slider_Label->setColour(Label::ColourIds::textColourId, Colours::white);

	trackSpike_DecreaseRate_Slider = new Slider("searchBoxWidthSlider");
	trackSpike_DecreaseRate_Slider->setRange(0.0f, 0.05f, 0.0001f);
	trackSpike_DecreaseRate_Slider->setSliderStyle(Slider::Rotary);
	trackSpike_DecreaseRate_Slider->setTextBoxStyle(Slider::NoTextBox, false, 80, 20);
	trackSpike_DecreaseRate_Slider->addListener(this);
	trackSpike_DecreaseRate_Slider->setValue(0.01f);
	trackSpike_DecreaseRate_Slider_Label = new Label("Track_Spike_Decrease_Rate_Slider_Label");
	trackSpike_DecreaseRate_Slider_Label->setText("Decrease Rate of Spike Tracking", sendNotification);
	trackSpike_DecreaseRate_Slider_Label->setColour(Label::ColourIds::textColourId, Colours::white);

	trackSpike_IncreaseRate_Text = new TextEditor("trackSpike_IncreaseRate_Text");
	trackSpike_IncreaseRate_Text->setText("+"+String(trackSpike_IncreaseRate_Slider->getValue(),0) + " V");

	trackSpike_DecreaseRate_Text = new TextEditor("trackSpike_DecreaseRate_Text");
	trackSpike_DecreaseRate_Text->setText("-"+String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	//


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
    
    g.drawRoundedRectangle(SPECTROGRAM_WIDTH-8, SPECTROGRAM_HEIGHT-(searchBoxLocation+searchBoxWidth),8, searchBoxWidth*2+1,1,2);

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

	// Diana's Group
    imageThresholdSlider->setBounds (685, 400, 55, 264);
	imageThresholdSliderLabel->setBounds(670, 664, 100, 24); // opposite to the instructions above - got moved in the rebase
   
    highImageThresholdText->setBounds (770, 410, 55, 24);
	highImageThresholdTextLabel->setBounds(830, 410, 160, 25); // opposite to the instructions above
    
    lowImageThresholdText->setBounds (770, 458, 55, 24);
	lowImageThresholdTextLabel->setBounds(830, 458, 160, 25); // opposite to the instructions above
    
    detectionThresholdText->setBounds(770, 434, 55, 24);
	detectionThresholdTextLabel->setBounds(830, 434, 160, 25); // opposite to the instructions above
    
    searchBoxSlider->setBounds (SPECTROGRAM_WIDTH-5, 0, 15, SPECTROGRAM_HEIGHT);
	searchBoxSliderLabel->setBounds(SPECTROGRAM_WIDTH-35, SPECTROGRAM_HEIGHT-17, 80, 50); // x value is inverted
    
    subsamplesPerWindowSlider->setBounds(850, 487, 159, 64);
	subsamplesPerWindowSliderLabel->setBounds(770, 494, 80, 50);
    
	// Grace's group
    startingSampleSlider->setBounds(850, 556, 159, 64);
	startingSampleSliderLabel->setBounds(770, 563, 80, 50); // x value is inverted
   
    colorStyleComboBox->setBounds(785, 10, 120, 24);
	colorStyleComboBoxLabel->setBounds(665, 10, 120, 24); 

	colorControlGroup->setBounds(665, 390, 398, 304); // the rectangle in the gui - doesn't need a label

    extendedColorScaleToggleButton->setBounds(780, 39, 24, 24); 
    extendedColorScaleToggleButtonLabel->setBounds(665, 39, 120, 24); 

	// x inversed on these two
    searchBoxWidthSlider->setBounds(478, 643, 50, 64);
	searchBoxWidthSliderLabel->setBounds(524, 664, 120, 24);

	ROISpikeLatencyLabel->setBounds(866, 126, 120, 24);  // 192 difference
	ROISpikeLatency->setBounds(980, 126, 72, 24);
	msLabel->setBounds(1052, 126, 72, 24);	// this is a label for the units used x inverted orignially (432, 336, 72, 24)
	// latency is 24 less on the y
	ROISpikeMagnitudeLabel->setBounds(866, 155, 120, 24); // not in line with the label above it and this angers me greatly, but 257 is too much, 256 is too little, there is no sweet spot 16 more than the other label
	ROISpikeMagnitude->setBounds(980, 155, 72, 24); // 72 difference
	mpersLabel->setBounds(1052, 155, 72, 24); // this is a label for the units used x inverted orignially (432, 336, 72, 24)

	// Lucy's Group
    conductionDistanceSlider->setBounds(850, 625, 159, 64);
	conductionDistanceSliderLabel->setBounds(770, 632, 79, 64); // x inverted

	setupButton->setBounds(955, 10, 120, 24);
	// Stimulus
	ppControllerComponent->setBounds(667, 260, 402, 350);

	// Threshold trigger control
	trigger_threshold_Slider->setBounds(780, 189, 159, 64);
	trigger_threshold_Slider_Label->setBounds(665, 209, 120, 24); // in a good place, the slider itself needs to move


	// channel control
	//textBox1->setBounds(10, 320, 72, 24);
	//textBox2->setBounds(10, 350, 72, 24);

	triggerChannelComboBox->setBounds(785, 68, 120, 24);
	triggerChannelComboBoxLabel->setBounds(665, 68, 120, 24);

	dataChannelComboBox->setBounds(785, 97, 120, 24);
	dataChannelComboBoxLabel->setBounds(665, 97, 120, 24); // fine

	trackSpike_button->setBounds(780, 126, 120, 24);
	trackSpike_button_Label->setBounds(665, 126, 120, 24);

	trackThreshold_button->setBounds(780, 155, 120, 24);
	trackThreshold_button_Label->setBounds(665, 155, 120, 24);

	
    //[UserResized]
    //[/UserResized]
}

bool LfpLatencyProcessorVisualizerContentComponent::keyPressed(const KeyPress& k) {
	
	int maxSubsample = std::round(DATA_CACHE_SIZE_SAMPLES / SPECTROGRAM_HEIGHT);
	
	//Lucy's style of keybind was much better than mine as it allowed to adjust value and slider position and send a notification in one single line, so thank you <3, from James
	
	//Increase subsamplesperwindow
	if ((k.getTextCharacter() == '=' || k.getTextCharacter() == '+' || k == KeyPress::numberPadAdd) && (subsamplesPerWindow < maxSubsample)) {
		subsamplesPerWindowSlider->setValue(subsamplesPerWindowSlider->getValue() + 5, sendNotificationAsync);
		return true;
	}
	//Decrease subsamplesperwindow
	else if ((k.getTextCharacter() == '-' || k == KeyPress::numberPadSubtract) && (subsamplesPerWindow > 0)) {
		subsamplesPerWindowSlider->setValue(subsamplesPerWindowSlider->getValue() - 5, sendNotificationAsync);
		return true;
	}
	//Increase starting sample
	else if ((k == KeyPress::upKey || k == KeyPress::numberPad8) && (startingSample < 30000)) {
		startingSampleSlider->setValue(startingSampleSlider->getValue() + 100, sendNotificationAsync);
		return true;
	}
	//Decrease starting sample
	else if ((k == KeyPress::downKey || k == KeyPress::numberPad2) && (startingSample > 0)) {
		startingSampleSlider->setValue(startingSampleSlider->getValue() - 100, sendNotificationAsync);
		return true;
	}
	//Increase search box location
	else if ((k == KeyPress::rightKey || k == KeyPress::numberPad6) && (searchBoxLocation < 300)) {
		searchBoxSlider->setValue(searchBoxSlider->getValue() + 5, sendNotificationAsync);
		return true;
	}
	//Decrease search box location
	else if ((k == KeyPress::leftKey || k == KeyPress::numberPad4) && (searchBoxLocation > 0)) {
		searchBoxSlider->setValue(searchBoxSlider->getValue() - 5, sendNotificationAsync);
		return true;
	}
	//Increase highImageThreshold
	else if ((k == KeyPress::pageUpKey || k == KeyPress::numberPad9) && (highImageThreshold < 100)) {
		imageThresholdSlider->setMaxValue(imageThresholdSlider->getMaxValue() + 2, sendNotificationAsync);
		return true;
	}
	//Decrease highImageThreshold
	else if ((k == KeyPress::pageDownKey || k == KeyPress::numberPad3) && (highImageThreshold > 0)) {
		imageThresholdSlider->setMaxValue(imageThresholdSlider->getMaxValue() - 2, sendNotificationAsync);
		return true;
	}
	//Increase lowImageThreshold
	else if ((k == KeyPress::homeKey || k == KeyPress::numberPad7) && (lowImageThreshold < 100)) {
		imageThresholdSlider->setMinValue(imageThresholdSlider->getMinValue() + 2, sendNotificationAsync);
		return true;
	}
	//Decrease lowImageThreshold
	else if ((k == KeyPress::endKey || k == KeyPress::numberPad1) && (lowImageThreshold > 0)) {
		imageThresholdSlider->setMinValue(imageThresholdSlider->getMinValue() - 2, sendNotificationAsync);
		return true;
	}
	else {
		return false;
	}
}


// could pass a bool value to this function for loading in values
// if the bool is false, we are not loading up, run as currently written
// if the bool is true, we are loading up, so do not update the xml
// also need a way of passing the value and connecting to a slider, maybe as just a string and int

void LfpLatencyProcessorVisualizerContentComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
	LfpLatencyProcessor* process = new LfpLatencyProcessor(); // to save the new value to xml
	juce::XmlElement *XmlValue = new XmlElement("Components");
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		cout << "Stuck here 1\n";
		//Lower value
		stimulusVoltageMin = sliderThatWasMoved->getMinValue();
		XmlValue->setAttribute("stimulusVoltageMin", stimulusVoltageMin);
		stimulusVoltageMin_text->setText(String(stimulusVoltageMin,2));
		cout << "Stuck here 2\n";
		//Upper value
		stimulusVoltageMax = sliderThatWasMoved->getMaxValue();
		XmlValue->setAttribute("stimulusVoltageMax", stimulusVoltageMax);
		stimulusVoltageMax_text->setText(String(stimulusVoltageMax,2));
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
			XmlValue->setAttribute("stimulusVoltage", stimulusVoltage);
			stimulusVoltage_text->setText(String(stimulusVoltage, 2));
			ppControllerComponent->setStimulusVoltage(stimulusVoltage);
			cout << "Updated stimulus voltage\n";
		}
		cout << "Done\n";
	}
    if (sliderThatWasMoved == imageThresholdSlider)
    {
		cout << "Stuck here 4\n";
        //[UserSliderCode_imageThresholdSlider] -- add your slider handling code here..

        //Lower value
		cout << "Stuck here 5\n";
        lowImageThreshold = sliderThatWasMoved->getMinValue();
		XmlValue->setAttribute("lowImageThreshold", lowImageThreshold);
        std::cout << "Slider lower: " << lowImageThreshold << std::endl;
        lowImageThresholdText->setText(String(lowImageThreshold,1)+" uV");
		cout << "Stuck here 6\n";
        //Upper value
        highImageThreshold = sliderThatWasMoved->getMaxValue();
		XmlValue->setAttribute("highImageThreshold", highImageThreshold);
        std::cout << "Slider upper: " << highImageThreshold << std::endl;
        highImageThresholdText->setText(String(highImageThreshold,1) + " uV");
		cout << "Stuck here 7\n";
        //mid value
        detectionThreshold = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("detectionThreshold", detectionThreshold);
        std::cout << "DetectionThehold" << detectionThreshold << std::endl;
        detectionThresholdText->setText(String(detectionThreshold,1) + " uV");

        //sliderThatWasMoved.getMinValue (1.0 / sliderThatWasMoved.getValue(), dontSendNotification);
        //[/UserSliderCode_imageThresholdSlider]
    }
    if (sliderThatWasMoved == searchBoxSlider)
    {
		cout << "Stuck here 8\n";
        searchBoxLocation = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("searchBoxLocation", searchBoxLocation);
        std::cout << "searchBoxLocation" << searchBoxLocation << std::endl;
        
    }
    if (sliderThatWasMoved == subsamplesPerWindowSlider)
    {
		cout << "Stuck here 9\n";
        //auto subsamplesPerWindowOld = subsamplesPerWindow;
        subsamplesPerWindow = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("subsamplesPerWindow", subsamplesPerWindow);
        std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
      
    }
    if (sliderThatWasMoved == startingSampleSlider)
    {
		cout << "Stuck here 10\n";
        startingSample = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("startingSample", startingSample);
        std::cout << "startingSample" << startingSample << std::endl;
        
    }
    if (sliderThatWasMoved == searchBoxWidthSlider)
    {
		cout << "Stuck here 11\n";
        searchBoxWidth = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("searchBoxWidth", searchBoxWidth);
        std::cout << "searchBoxWidth" << searchBoxWidth << std::endl;
        
    }
	if (sliderThatWasMoved == trackSpike_IncreaseRate_Slider)
	{
		cout << "Stuck here 12\n";
		trackSpike_IncreaseRate = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("trackSpike_IncreaseRate", trackSpike_IncreaseRate);
		trackSpike_IncreaseRate_Text->setText("+" + String(trackSpike_IncreaseRate_Slider->getValue(), 0)+ " V");
	}
	if (sliderThatWasMoved == trackSpike_DecreaseRate_Slider)
	{
		cout << "Stuck here 13\n";
		trackSpike_DecreaseRate = sliderThatWasMoved->getValue();
		XmlValue->setAttribute("trackSpike_DecreaseRate", trackSpike_DecreaseRate);
		trackSpike_DecreaseRate_Text->setText("-" + String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	}
	printf("running save custom params to XML\n");
	process->saveCustomParametersToXml(XmlValue);
	printf("tidying up\n");
	delete process;
	printf("all done\n");
    
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
			trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::black);
		}
		else if (buttonThatWasClicked->getToggleState() == false)
		{
			trackThreshold_button->setEnabled(false);
			trackThreshold_button->setToggleState(false, sendNotification);
			trackThreshold_button_Label->setColour(juce::Label::ColourIds::textColourId, Colours::darkgrey);
		}
	}
	if (buttonThatWasClicked == setupButton) {

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

		auto& setupBox = juce::CallOutBox::launchAsynchronously(view, setupButton->getBounds(), this);
		setupBox.setLookAndFeel(new CustomLookAndFeel());
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
