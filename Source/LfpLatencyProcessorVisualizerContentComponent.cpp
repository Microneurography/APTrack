#include "LfpLatencyProcessorVisualizerContentComponent.h"


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
LfpLatencyProcessorVisualizerContentComponent::LfpLatencyProcessorVisualizerContentComponent ()
: spectrogram(SPECTROGRAM_WIDTH, SPECTROGRAM_HEIGHT),searchBoxLocation(150),subsamplesPerWindow(60),startingSample(0),colorStyle(1)
{
    searchBoxLocation = 150;
    conductionDistance = 100;

	setWantsKeyboardFocus(true);

	// HACK
	stimulusVoltage = 0.0f;
	stimulusVoltageMax = 10.0f; // you also have to change the range of the slider on line 87
	stimulusVoltageMin = 0.5f;

	trackSpike_IncreaseRate = 0.01;
	trackSpike_DecreaseRate = 0.01;

	// The code for the descriptions is below
	// I think that the labels can have the argument dontSendNotification. Not sure what sending does

	addAndMakeVisible(setupButton = new TextButton("setupButton"));
	setupButton->setButtonText("Setup");
	setupButton->addListener(this);
	setupButton->setColour(TextButton::ColourIds::buttonColourId, Colours::lightgrey);

	addAndMakeVisible(optionsButton = new TextButton("optionsButton"));
	optionsButton->setButtonText("Options");
	optionsButton->addListener(this);
	optionsButton->setColour(TextButton::ColourIds::buttonColourId, Colours::lightgrey);

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

    //buffer/window = ssp
    
    addAndMakeVisible (searchBoxWidthSlider = new Slider ("searchBoxWidthSlider"));
    searchBoxWidthSlider->setRange (1, 63, 1);
    searchBoxWidthSlider->setSliderStyle (Slider::Rotary);
    searchBoxWidthSlider->setTextBoxStyle (Slider::NoTextBox, false, 80, 20);
    searchBoxWidthSlider->addListener (this);
	addAndMakeVisible(searchBoxWidthSliderLabel = new Label("search_Box_Width_Slider_Label"));
	searchBoxWidthSliderLabel->setText("Search Box Width", sendNotification);
    
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
	trackSpike_button->setToggleState(false, sendNotification);
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
	
	//MULTI SPIKE TRACKING------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	spikeTrackerContent = new TableContent();
	addAndMakeVisible(spikeTracker = new TableListBox("Tracked Spikes", spikeTrackerContent));
	spikeTracker->setColour(ListBox::backgroundColourId, Colours::lightgrey);
	spikeTracker->getHeader().addColumn("Spike", 1, 50);
	spikeTracker->getHeader().addColumn("Location", 2, 100);
	spikeTracker->getHeader().addColumn("Firing Proabability", 3, 120);
	spikeTracker->getHeader().addColumn("Select Spike", 4, 100);
	spikeTracker->getHeader().addColumn("Delete Track", 5, 100);
	spikeTracker->autoSizeAllColumns();
	spikeTracker->updateContent();

	addAndMakeVisible(location0 = new TextEditor("Location 0"));
	location0->setText("0");
	addAndMakeVisible(location1 = new TextEditor("Location 1"));
	location1->setText("0");
	addAndMakeVisible(location2 = new TextEditor("Location 2"));
	location2->setText("0");
	addAndMakeVisible(location3 = new TextEditor("Location 3"));
	location3->setText("0");

	addAndMakeVisible(fp0 = new TextEditor("Firing Proabability 0"));
	fp0->setText("0");
	addAndMakeVisible(fp1 = new TextEditor("Firing Proabability 1"));
	fp1->setText("0");
	addAndMakeVisible(fp2 = new TextEditor("Firing Proabability 2"));
	fp2->setText("0");
	addAndMakeVisible(fp3 = new TextEditor("Firing Proabability 3"));
	fp3->setText("0");

	addAndMakeVisible(follow0 = new ToggleButton(""));
	follow0->addListener(this);
	follow0->setToggleState(false, sendNotification);
	follow0->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);
	addAndMakeVisible(follow1 = new ToggleButton(""));
	follow1->addListener(this);
	follow1->setToggleState(false, sendNotification);
	follow1->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);
	addAndMakeVisible(follow2 = new ToggleButton(""));
	follow2->addListener(this);
	follow2->setToggleState(false, sendNotification);
	follow2->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);
	addAndMakeVisible(follow3 = new ToggleButton(""));
	follow3->addListener(this);
	follow3->setToggleState(false, sendNotification);
	follow3->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::white);

	addAndMakeVisible(del0 = new TextButton(""));
	del0->addListener(this);
	del0->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
	del0->setToggleState(false, sendNotification);
	addAndMakeVisible(del1 = new TextButton(""));
	del1->addListener(this);
	del1->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
	del1->setToggleState(false, sendNotification);
	addAndMakeVisible(del2 = new TextButton(""));
	del2->addListener(this);
	del2->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
	del2->setToggleState(false, sendNotification);
	addAndMakeVisible(del3 = new TextButton(""));
	del3->addListener(this);
	del3->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
	del3->setToggleState(false, sendNotification);

	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
   
	addAndMakeVisible(textBox2 = new TextEditor("selectedDataChanText"));
	textBox2->setText("Data");

	addAndMakeVisible(textBox1 = new TextEditor("selectedTriggerChanText"));
	textBox1->setText("Trigger");

	stimulusVoltageSlider->setMinValue(stimulusVoltageMin);
	stimulusVoltageSlider->setMaxValue(stimulusVoltageMax);
	stimulusVoltageSlider->setValue(stimulusVoltage);

	searchBoxSlider->setValue(10.0f);

    colorStyleComboBox->setSelectedId(1);
    searchBoxWidthSlider->setValue(3);
    
    extendedColorScaleToggleButton->setToggleState(false,sendNotification);

    spectrogramControlPanel = new LfpLatencySpectrogramControlPanel(this);
    addAndMakeVisible(spectrogramControlPanel);

    setSize (700, 900);
    
    spikeDetected = false;

}

LfpLatencyProcessorVisualizerContentComponent::~LfpLatencyProcessorVisualizerContentComponent()
{
    searchBoxSlider = nullptr;
    searchBoxWidthSlider=nullptr;
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
	location0 = nullptr; location1 = nullptr; location2 = nullptr; location3 = nullptr;
	follow0 = nullptr; follow1 = nullptr; follow2 = nullptr; follow3 = nullptr;
	del0 = nullptr; del1 = nullptr; del2 = nullptr; del3 = nullptr;

	trackSpike_IncreaseRate_Slider = nullptr;
	trackSpike_DecreaseRate_Slider = nullptr;

	trackSpike_IncreaseRate_Text = nullptr;
	trackSpike_DecreaseRate_Text = nullptr;
}

//==============================================================================
void LfpLatencyProcessorVisualizerContentComponent::paint (Graphics& g)
{
    g.fillAll (Colours::grey);
    g.setOpacity (1.0f);
    g.drawImage(spectrogram.getImage(),
		0,0,spectrogram.getImageWidth(),spectrogram.getImageHeight(),
		0,0,spectrogram.getImageWidth(),spectrogram.getImageHeight());
    //Note, drawImage handles rescaling!
    
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
	//Paint is called constatnly, so the cells should be paiting the new number in them
	//spikeTrackerContent->paintCell(g, 1, 1, 10, 10, true);
	//spikeTrackerContent->paintCell(g, 2, 1, 10, 10, true);
	//spikeTrackerContent->paintCell(g, 1, 2, 10, 10, true);
	//spikeTrackerContent->paintCell(g, 2, 2, 10, 10, true);

	
	//spikeTracker->autoSizeAllColumns();
	//spikeTracker->updateContent();
	g.drawRoundedRectangle(SPECTROGRAM_WIDTH-8, SPECTROGRAM_HEIGHT-(searchBoxLocation+searchBoxWidth),8, searchBoxWidth*2+1,1,2);
	spikeTracker->updateContent();
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
	// Diana's Group
    searchBoxSlider->setBounds (SPECTROGRAM_WIDTH-5, 0, 15, SPECTROGRAM_HEIGHT);
	searchBoxSliderLabel->setBounds(SPECTROGRAM_WIDTH-35, SPECTROGRAM_HEIGHT-17, 80, 50); // x value is inverted
    
	// Grace's group
    colorStyleComboBox->setBounds(785, 10, 120, 24);
	colorStyleComboBoxLabel->setBounds(665, 10, 120, 24); 

    extendedColorScaleToggleButton->setBounds(780, 39, 24, 24); 
    extendedColorScaleToggleButtonLabel->setBounds(665, 39, 120, 24); 

	// x inversed on these two
    searchBoxWidthSlider->setBounds(478, 643, 50, 64);
	searchBoxWidthSliderLabel->setBounds(524, 664, 120, 24);

	ROISpikeLatencyLabel->setBounds(980, 260, 120, 24);  // 192 difference
	ROISpikeLatency->setBounds(1110, 260, 72, 24);
	msLabel->setBounds(1190, 260, 72, 24);	// this is a label for the units used x inverted orignially (432, 336, 72, 24)
	// latency is 24 less on the y
	ROISpikeMagnitudeLabel->setBounds(980, 290, 120, 24); // not in line with the label above it and this angers me greatly, but 257 is too much, 256 is too little, there is no sweet spot 16 more than the other label
	ROISpikeMagnitude->setBounds(1110, 290, 72, 24); // 72 difference
	mpersLabel->setBounds(1190, 290, 72, 24); // this is a label for the units used x inverted orignially (432, 336, 72, 24)


	// Lucy's Group
	setupButton->setBounds(955, 10, 120, 24);
	optionsButton->setBounds(665, 10, 120, 24);
	// Stimulus
	ppControllerComponent->setBounds(667, 260, 402, 350);

	// Threshold trigger control
	trigger_threshold_Slider->setBounds(1110, 320, 159, 64);
	trigger_threshold_Slider_Label->setBounds(980, 340, 120, 24); // in a good place, the slider itself needs to move


	// channel control
	//textBox1->setBounds(10, 320, 72, 24);
	//textBox2->setBounds(10, 350, 72, 24);

	triggerChannelComboBox->setBounds(785, 68, 120, 24);
	triggerChannelComboBoxLabel->setBounds(665, 68, 120, 24);

	dataChannelComboBox->setBounds(785, 97, 120, 24);
	dataChannelComboBoxLabel->setBounds(665, 97, 120, 24); // fine

	//trackSpikeComboBox->setBounds(950, 97, 120, 24);
	spikeTracker->setBounds(665, 40, 470, 200);

	location0->setBounds(715, 69, 100, 20);
	location1->setBounds(715, 90, 100, 20);
	location2->setBounds(715, 111, 100, 20);
	location3->setBounds(715, 132, 100, 20);

	fp0->setBounds(815, 69, 120, 20);
	fp1->setBounds(815, 90, 120, 20);
	fp2->setBounds(815, 111, 120, 20);
	fp3->setBounds(815, 132, 120, 20);

	follow0->setBounds(935, 69, 120, 20);
	follow1->setBounds(935, 91, 120, 20);
	follow2->setBounds(935, 112, 120, 20);
	follow3->setBounds(935, 134, 120, 20);

	del0->setBounds(1050, 68, 60, 18);
	del1->setBounds(1050, 89, 60, 18);
	del2->setBounds(1050, 110, 60, 18);
	del3->setBounds(1050, 131, 60, 18);

	trackSpike_button->setBounds(780, 126, 120, 24);
	trackSpike_button_Label->setBounds(665, 126, 120, 24);

	trackThreshold_button->setBounds(780, 155, 120, 24);
	trackThreshold_button_Label->setBounds(665, 155, 120, 24);

    auto area = getLocalBounds().withTrimmedLeft(getWidth() * 0.60).withTrimmedTop(getHeight() * 0.60);
    spectrogramControlPanel->setBounds(area);
}

bool LfpLatencyProcessorVisualizerContentComponent::keyPressed(const KeyPress& k) {
	//Lucy's style of keybind was much better than mine as it allowed to adjust value and slider position and send a notification in one single line, so thank you <3, from James
	//Increase search box location
	if ((k == KeyPress::rightKey || k == KeyPress::numberPad6) && (searchBoxLocation < 600)) {
		searchBoxSlider->setValue(searchBoxSlider->getValue() + 5, sendNotificationAsync);
		return true;
	}
	//Decrease search box location
	else if ((k == KeyPress::leftKey || k == KeyPress::numberPad4) && (searchBoxLocation > 0)) {
		searchBoxSlider->setValue(searchBoxSlider->getValue() - 5, sendNotificationAsync);
		return true;
	}
	else {
		return false;
	}
}

void LfpLatencyProcessorVisualizerContentComponent::sliderValueChanged (Slider* sliderThatWasMoved)
{
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		cout << "Stuck here 1\n";
		//Lower value
		stimulusVoltageMin = sliderThatWasMoved->getMinValue();
		stimulusVoltageMin_text->setText(String(stimulusVoltageMin,2));
		cout << "Stuck here 2\n";
		//Upper value
		stimulusVoltageMax = sliderThatWasMoved->getMaxValue();
		stimulusVoltageMax_text->setText(String(stimulusVoltageMax,2));
		cout << "Stuck here 3\n";
		//mid value
		stimulusVoltage = sliderThatWasMoved->getValue();
		stimulusVoltage_text->setText(String(stimulusVoltage,2));

		// very slow for some reason
		cout << "Got here\n";
		if (stimulusVoltage > 4 && alreadyAlerted == false) {
			cout << "Made it past the if\n";
			// makes an alert window, returns true if user is okay with it being high
			voltageTooHighOkay = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::WarningIcon, "Voltage Could Be Too High", "Are you sure you want to set the voltage this high?", "Yes", "No");
			cout << "Created Window\n";
			alreadyAlerted = true;
			cout << "already alterted is now true\n";
		}
		if (voltageTooHighOkay) {
			cout << "Made it past the 2nd if\n";
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
        spectrogramControlPanel->setLowImageThresholdText(String(lowImageThreshold, 1) + " uV");
		cout << "Stuck here 6\n";
        //Upper value
        highImageThreshold = sliderThatWasMoved->getMaxValue();
        std::cout << "Slider upper: " << highImageThreshold << std::endl;
        spectrogramControlPanel->setHighImageThresholdText(String(highImageThreshold, 1) + " uV");
		cout << "Stuck here 7\n";
        //mid value
        detectionThreshold = sliderThatWasMoved->getValue();
        std::cout << "DetectionThehold" << detectionThreshold << std::endl;
        spectrogramControlPanel->setDetectionThresholdText(String(detectionThreshold, 1) + " uV");

        //sliderThatWasMoved.getMinValue (1.0 / sliderThatWasMoved.getValue(), dontSendNotification);
    }
    if (sliderThatWasMoved == searchBoxSlider)
    {
		cout << "Stuck here 8\n";
        searchBoxLocation = sliderThatWasMoved->getValue();
        std::cout << "searchBoxLocation" << searchBoxLocation << std::endl;
        
    }
    if (sliderThatWasMoved->getName() == "Subsamples Per Window")
    {
		cout << "Stuck here 9\n";
        //auto subsamplesPerWindowOld = subsamplesPerWindow;
        subsamplesPerWindow = sliderThatWasMoved->getValue();
        std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
      
    }
    if (sliderThatWasMoved->getName() == "Starting Sample")
    {
		cout << "Stuck here 10\n";
        startingSample = sliderThatWasMoved->getValue();
        std::cout << "startingSample" << startingSample << std::endl;
        
    }
    if (sliderThatWasMoved == searchBoxWidthSlider)
    {
		cout << "Stuck here 11\n";
        searchBoxWidth = sliderThatWasMoved->getValue();
        std::cout << "searchBoxWidth" << searchBoxWidth << std::endl;
        
    }
	if (sliderThatWasMoved == trackSpike_IncreaseRate_Slider)
	{
		cout << "Stuck here 12\n";
		trackSpike_IncreaseRate = sliderThatWasMoved->getValue();
		trackSpike_IncreaseRate_Text->setText("+" + String(trackSpike_IncreaseRate_Slider->getValue(), 0)+ " V");
	}
	if (sliderThatWasMoved == trackSpike_DecreaseRate_Slider)
	{
		cout << "Stuck here 13\n";
		trackSpike_DecreaseRate = sliderThatWasMoved->getValue();
		trackSpike_DecreaseRate_Text->setText("-" + String(trackSpike_DecreaseRate_Slider->getValue(), 0) + " V");
	}
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
        }
        else {
            // If using regular scale (eg when using FPGA real time data)
            spectrogramControlPanel->setImageThresholdRange(0, 100, 0);
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
	if (buttonThatWasClicked == del0)
		del_0 = true;
	if (buttonThatWasClicked == del1)
		del_1 = true;
	if (buttonThatWasClicked == del2)
		del_2 = true;
	if (buttonThatWasClicked == del3)
		del_3 = true;
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
	if (buttonThatWasClicked == optionsButton) {

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

		auto& setupBox = juce::CallOutBox::launchAsynchronously(view, optionsButton->getBounds(), this);
		setupBox.setLookAndFeel(new CustomLookAndFeel());
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
		g.fillAll(Colours::darkgrey);
	}
	else {
		g.fillAll(Colours::lightgrey);
	}

}

Component* TableContent::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* exsistingComponetToUpdate) {
	jassert(existingComponentToUpdate == nullptr);
	return nullptr;

}

int LfpLatencyProcessorVisualizerContentComponent::getStartingSample() const
{
	return startingSample;
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
