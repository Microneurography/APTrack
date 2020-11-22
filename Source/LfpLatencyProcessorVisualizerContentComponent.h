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

#ifndef __JUCE_HEADER_D83F67960ECDED8C__
#define __JUCE_HEADER_D83F67960ECDED8C__

//[Headers]     -- You can add your own extra header files here --
#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

#include "pulsePalController/ppController.h"

//[/Headers]

//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LfpLatencyProcessorVisualizerContentComponent : public Component,
                                                      public SliderListener,
                                                      public ButtonListener
{
public:
    //==============================================================================
    LfpLatencyProcessorVisualizerContentComponent();
    ~LfpLatencyProcessorVisualizerContentComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint(Graphics &g) override;
    void resized() override;
    void sliderValueChanged(Slider *sliderThatWasMoved) override;
    void buttonClicked(Button *buttonThatWasClicked) override;

    //void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    Image spectrogramImage; //Will contain the spectrogram image.
    // Make an editor to be friendly class of this content component,
    // so the editor will have access to all methods and variables of this component.
    friend class LfpLatencyProcessorVisualizer;

    //Image thresholds
    float lowImageThreshold;
    float highImageThreshold;
    int colorStyle;

    int draw_imageHeight;
    int draw_rightHandEdge;

    //LfpLatencyProcessorVisualizer content;

    //SearchBoxParams
    int searchBoxLocation;

    int searchBoxWidth;

    bool spikeDetected;
    float detectionThreshold;
    int subsamplesPerWindow;
    int startingSample;

    float bitBolts;

    float conductionDistance;

    int absPos;
    //[/UserVariables]

    float stimulusVoltage;

    float stimulusVoltageMax;

    float stimulusVoltageMin;

    float trackSpike_DecreaseRate;
    float trackSpike_IncreaseRate;

    //==============================================================================
    ScopedPointer<Slider> imageThresholdSlider;
	ScopedPointer<Label> imageThresholdSliderLabel;

    ScopedPointer<Slider> searchBoxSlider;
	ScopedPointer<Label> searchBoxSliderLabel;

    ScopedPointer<TextEditor> ROIspikeLocation;
	ScopedPointer<Label> msLabel;

    ScopedPointer<TextEditor> ROIspikeValue;
	ScopedPointer<Label> mpersLabel;
    
    ScopedPointer<Slider> conductionDistanceSlider;
    
    ScopedPointer<TextEditor> detectionThresholdText;
	ScopedPointer<Label> detectionThresholdTextLabel;
    
    ScopedPointer<TextEditor> lowImageThresholdText;
	ScopedPointer<Label> lowImageThresholdTextLabel;

    ScopedPointer<TextEditor> highImageThresholdText;
	ScopedPointer<Label> highImageThresholdTextLabel;
    
    ScopedPointer<Slider> subsamplesPerWindowSlider;
	ScopedPointer<Label> subsamplesPerWindowSliderLabel;
    
    ScopedPointer<Slider> startingSampleSlider;
	ScopedPointer<Label> startingSampleSliderLabel;
    
    ScopedPointer<Slider> searchBoxWidthSlider;

    ScopedPointer<ComboBox> colorStyleComboBox;
	ScopedPointer<Label> colorStyleComboBoxLabel;
    
    ScopedPointer<GroupComponent> colorControlGroup;
	ScopedPointer<Label> colorControlGroupLabel;
    
    ScopedPointer<ToggleButton> extendedColorScaleToggleButton;
    
	
    ScopedPointer<Label> cmLabel;
 
    //ScopedPointer<GroupComponent> detectionControlGroup;

	// Stimulus control
	ScopedPointer<Slider> stimulusVoltageSlider;
	ScopedPointer<Label> stimulusVoltageSliderLabel;

    ScopedPointer<TextEditor> stimulusVoltageMax_text;
    ScopedPointer<TextEditor> stimulusVoltage_text;
    ScopedPointer<TextEditor> stimulusVoltageMin_text;

    ScopedPointer<ppController> ppControllerComponent;

    ScopedPointer<TextEditor> textBox1;
    ScopedPointer<TextEditor> textBox2;

    ScopedPointer<ComboBox> triggerChannelComboBox;
    ScopedPointer<ComboBox> dataChannelComboBox;

    ScopedPointer<Slider> Trigger_threshold; //TODO

    ScopedPointer<ToggleButton> trackSpike_button;
    ScopedPointer<ToggleButton> trackThreshold_button;

    ScopedPointer<TextEditor> ROISpikeLatency;
    ScopedPointer<TextEditor> ROISpikeMagnitude;

    ScopedPointer<Slider> trackSpike_IncreaseRate_Slider;
    ScopedPointer<Slider> trackSpike_DecreaseRate_Slider;

    ScopedPointer<TextEditor> trackSpike_IncreaseRate_Text;
    ScopedPointer<TextEditor> trackSpike_DecreaseRate_Text;

    ScopedPointer<Slider> trigger_threshold_Slider;

    //DEBUG

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessorVisualizerContentComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif // __JUCE_HEADER_D83F67960ECDED8C__
