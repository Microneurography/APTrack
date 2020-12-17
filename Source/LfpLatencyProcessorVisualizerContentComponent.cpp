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

	setWantsKeyboardFocus(true);
	
	// HACK
	stimulusVoltage = 3.0f;

	stimulusVoltageMax = 5.0f;

	stimulusVoltageMin = 0.5f;


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
    
    addAndMakeVisible(colorControlGroup = new GroupComponent);
    colorControlGroup->setName(("Color control"));
    

    addAndMakeVisible (imageThresholdSlider = new Slider ("imageThreshold"));
    imageThresholdSlider->setRange (0, 90, 0);
    imageThresholdSlider->setSliderStyle (Slider::ThreeValueVertical);
    imageThresholdSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    imageThresholdSlider->addListener (this);


	// Stimulus control

	addAndMakeVisible(ppControllerComponent = new ppController());

	addAndMakeVisible(stimulusVoltageSlider = new Slider("stimulusVoltage"));
	stimulusVoltageSlider->setRange(0.0f, 4.0f, 0);
	stimulusVoltageSlider->setSliderStyle(Slider::ThreeValueVertical);
	stimulusVoltageSlider->setTextBoxStyle(Slider::NoTextBox, true, 80, 20);
	stimulusVoltageSlider->addListener(this);

	addAndMakeVisible(stimulusVoltageMin_text = new TextEditor("Stimulus Min"));
	stimulusVoltageMin_text->setText(String(stimulusVoltageMin));

	addAndMakeVisible(stimulusVoltageMax_text = new TextEditor("Stimulus Max"));
	stimulusVoltageMax_text->setText(String(stimulusVoltageMax));

	addAndMakeVisible(stimulusVoltage_text = new TextEditor("Stimulus now"));
	stimulusVoltage_text->setText(String(stimulusVoltage));
	//

    
    addAndMakeVisible (searchBoxSlider = new Slider ("searchBox"));
    searchBoxSlider->setRange(0, 300, 1);
    searchBoxSlider->setSliderStyle (Slider::LinearVertical);
    searchBoxSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);
    searchBoxSlider->addListener (this);
    
    addAndMakeVisible(ROIspikeLocation = new TextEditor("SearchBoxLocationLatency"));
    ROIspikeLocation->setText(String(searchBoxLocation));
    
    
    addAndMakeVisible(ROIspikeValue = new TextEditor("SearchBoxLocationSpeed"));
    ROIspikeValue->setText("NaN");
    
    
    addAndMakeVisible(lowImageThresholdText = new TextEditor("lowImageThreshold"));
	lowImageThresholdText->setText(String(lowImageThreshold));
    
    addAndMakeVisible(highImageThresholdText = new TextEditor("highImageThreshold"));
	highImageThresholdText->setText(String(highImageThreshold));
    
    addAndMakeVisible(detectionThresholdText = new TextEditor("spikeDetectionThresholdText"));
    detectionThresholdText->setText(String(detectionThreshold));
    
    //buffer/window = ssp
    
    addAndMakeVisible (subsamplesPerWindowSlider = new Slider ("subsampleSlider"));
    
    int maxSubsample = std::round(DATA_CACHE_SIZE_SAMPLES/SPECTROGRAM_HEIGHT);
    
    subsamplesPerWindowSlider->setRange (1, maxSubsample, 1); // 100/30000 = 10ms
    subsamplesPerWindowSlider->setSliderStyle (Slider::Rotary);
    subsamplesPerWindowSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    subsamplesPerWindowSlider->addListener (this);
    
    addAndMakeVisible (startingSampleSlider = new Slider ("startingSampleSlider"));
    startingSampleSlider->setRange (0, 30000, 1);
    startingSampleSlider->setSliderStyle (Slider::Rotary);
    startingSampleSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    startingSampleSlider->addListener (this);
    
    addAndMakeVisible (conductionDistanceSlider = new Slider ("conductionDistanceSlider"));
    conductionDistanceSlider->setRange (0, 2000, 1);
    conductionDistanceSlider->setSliderStyle (Slider::Rotary);
    conductionDistanceSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    conductionDistanceSlider->addListener (this);
    
    
    addAndMakeVisible (searchBoxWidthSlider = new Slider ("searchBoxWidthSlider"));
    searchBoxWidthSlider->setRange (1, 30, 1);
    searchBoxWidthSlider->setSliderStyle (Slider::Rotary);
    searchBoxWidthSlider->setTextBoxStyle (Slider::TextBoxRight, false, 80, 20);
    searchBoxWidthSlider->addListener (this);
    
    addAndMakeVisible(colorStyleComboBox = new ComboBox("Color style selector"));
    colorStyleComboBox->setEditableText(false);
    colorStyleComboBox->setJustificationType(Justification::centredLeft);
    colorStyleComboBox->setTextWhenNothingSelected(TRANS("WHOT"));
    colorStyleComboBox->addItem("WHOTTT", 1);
    colorStyleComboBox->addItem("BHOT", 2);
    colorStyleComboBox->addItem("WHOT,PLAIN", 3);
    colorStyleComboBox->addItem("BHOT,PLAIN", 4);
    
    addAndMakeVisible(extendedColorScaleToggleButton = new ToggleButton("Extended scale?"));
    extendedColorScaleToggleButton->addListener(this);
    
    addAndMakeVisible(msLabel = new Label("ms_label"));
    msLabel->setText ("ms", dontSendNotification);
    
    addAndMakeVisible(msLabel = new Label("ms_label"));
    msLabel->setText ("ms", dontSendNotification);
    
    addAndMakeVisible(cmLabel = new Label("cm_label"));
    cmLabel->setText ("cm", dontSendNotification);
    
    addAndMakeVisible(mpersLabel = new Label("mpers_label"));
    mpersLabel->setText ("m/s", dontSendNotification);
   
    
    
    
    
    
    //[UserPreSize]

    //set default minVal
    imageThresholdSlider->setMinValue(1.0f);
    //set default maxVal
    imageThresholdSlider->setMaxValue(10.0f);
    searchBoxSlider->setValue(10.0f);
    imageThresholdSlider->setValue(5.0f);
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
    

    if (spikeDetected) {
        g.setColour(Colours::green);
    } else {
        g.setColour(Colours::red);
    }
    
    g.drawRoundedRectangle(300-8, 300-(searchBoxLocation+searchBoxWidth),8, searchBoxWidth*2+1,1,2);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LfpLatencyProcessorVisualizerContentComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    imageThresholdSlider->setBounds (360, 24, 55, 264);
    
    highImageThresholdText->setBounds (424, 24, 55, 24);
    
    lowImageThresholdText->setBounds (424, 72, 55, 24);
    
    detectionThresholdText->setBounds(424, 48, 55, 24);
    
    searchBoxSlider->setBounds (295, 0, 15, 300);
    
    
    
    
    
    subsamplesPerWindowSlider->setBounds(424, 152, 159, 64);
    
    startingSampleSlider->setBounds(424, 224, 159, 64);
    
    colorStyleComboBox->setBounds(424, 112, 120, 24);
    
    colorControlGroup->setBounds(344, 0, 248, 304);
    
    extendedColorScaleToggleButton->setBounds(500, 24, 80, 24);
    
    searchBoxWidthSlider->setBounds(500, 54, 50, 50);
    
    ROIspikeLocation->setBounds(360, 336, 72, 24);
    msLabel->setBounds(432, 336, 72, 24);
    
    ROIspikeValue->setBounds(360, 360, 72, 24);
    mpersLabel->setBounds(432, 360, 72, 24);
    
    conductionDistanceSlider->setBounds(360, 456, 159, 64);

	// Stimulus
	ppControllerComponent->setBounds(600, 400, 402, 350);

	stimulusVoltageSlider->setBounds(600, 0, 55, 264);

	stimulusVoltageMin_text->setBounds(664, 72, 55, 24);
	stimulusVoltage_text->setBounds(664, 48, 55, 24);
	stimulusVoltageMax_text->setBounds(664, 24, 55, 24);
	
    //[UserResized]
    //[/UserResized]
}

void LfpLatencyProcessorVisualizerContentComponent::sliderValueChanged(Slider* sliderThatWasMoved)
{
	//[UsersliderValueChanged_Pre]
	//[/UsersliderValueChanged_Pre]
	if (sliderThatWasMoved == stimulusVoltageSlider)
	{
		//Lower value
		stimulusVoltageMin = sliderThatWasMoved->getMinValue();
		stimulusVoltageMin_text->setText(String(stimulusVoltageMin, 2));

		//Upper value
		stimulusVoltageMax = sliderThatWasMoved->getMaxValue();
		stimulusVoltageMax_text->setText(String(stimulusVoltageMax, 2));

		//mid value
		stimulusVoltage = sliderThatWasMoved->getValue();
		stimulusVoltage_text->setText(String(stimulusVoltage, 2));

		ppControllerComponent->setStimulusVoltage(stimulusVoltage);

		//
	}
	if (sliderThatWasMoved == imageThresholdSlider)
	{
		//[UserSliderCode_imageThresholdSlider] -- add your slider handling code here..

		//Lower value
		lowImageThreshold = sliderThatWasMoved->getMinValue();
		std::cout << "Slider lower: " << lowImageThreshold << std::endl;
		lowImageThresholdText->setText(String(lowImageThreshold));

		//Upper value
		highImageThreshold = sliderThatWasMoved->getMaxValue();
		std::cout << "Slider upper: " << highImageThreshold << std::endl;
		highImageThresholdText->setText(String(highImageThreshold));

		//mid value
		detectionThreshold = sliderThatWasMoved->getValue();
		std::cout << "DetectionThehold" << detectionThreshold << std::endl;
		detectionThresholdText->setText(String(detectionThreshold));

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
		std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;

		/*
		   searchBoxWidthSlider->setValue(std::round(searchBoxWidth*subsamplesPerWindowOld/subsamplesPerWindow));

		   std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
		   std::cout << "absPos" << absPos << std::endl;
		   int startingSample2 = (absPos-searchBoxLocation*subsamplesPerWindow);

		   std::cout << "sugestedStartSample " << startingSample2 << std::endl;

		   if (startingSample2 > 0)
		   {
		   startingSample = startingSample2;

		   // searchBoxLocation = startingSample;
		   startingSampleSlider->setValue(startingSample);

		   std::cout << "using startSample " << startingSample << std::endl;

		   }
		   else
		   {
		   startingSample = 0;
		   }
		   */
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
	if (sliderThatWasMoved == conductionDistanceSlider)
	{

		conductionDistance = conductionDistanceSlider->getValue();
		std::cout << "conductionSpeed" << conductionDistance << std::endl;
	}
	//[UsersliderValueChanged_Post]
	//[/UsersliderValueChanged_Post]
}

bool LfpLatencyProcessorVisualizerContentComponent::keyPressed(const KeyPress &key) {
	if ((key == KeyPress::upKey) && (startingSample < 30000)) {
		startingSample = startingSample + 10;
		startingSampleSlider->setValue(startingSample);
		std::cout << "startingSample" << startingSample << std::endl;
		return true;
	}
	else if ((key == KeyPress::downKey) && (startingSample > 0)) {
		startingSample = startingSample--;
		startingSampleSlider->setValue(startingSample);
		std::cout << "startingSample" << startingSample << std::endl;
		return true;
	}
	else if ((key.getTextCharacter() == '=' || key.getTextCharacter() == '+') && (subsamplesPerWindow < 100)){
		subsamplesPerWindow = subsamplesPerWindow++;
		subsamplesPerWindowSlider->setValue(subsamplesPerWindow);
		std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
		return true;
	}
	else if ((key.getTextCharacter() == '-') && (subsamplesPerWindow > 0)){
		subsamplesPerWindow = subsamplesPerWindow--;
		subsamplesPerWindowSlider->setValue(subsamplesPerWindow);
		std::cout << "subsamplesPerWindow" << subsamplesPerWindow << std::endl;
		return true;
	}
	else {
		return false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------
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
            imageThresholdSlider->setRange (0, 90, 0);
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
