#ifndef __JUCE_HEADER_D83F67960ECDED8C__
#define __JUCE_HEADER_D83F67960ECDED8C__

#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "pulsePalController/ppController.h"

#include "LfpLatencySpectrogram.h"
#include "LfpLatencySpectrogramControlPanel.h"

class LfpLatencyProcessorVisualizerContentComponent : public Component,
                                                      public SliderListener,
                                                      public ButtonListener
{
public:
    LfpLatencyProcessorVisualizerContentComponent();
    ~LfpLatencyProcessorVisualizerContentComponent();

    void paint(Graphics &g) override;
    void resized() override;
    void sliderValueChanged(Slider *sliderThatWasMoved) override;
    void buttonClicked(Button *buttonThatWasClicked) override;
	bool keyPressed(const KeyPress& k) override;
    //void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;

    int getStartingSample() const;
    int getSubsamplesPerWindow() const;
    float getLowImageThreshold() const;
    float getHighImageThreshold() const;
    float getDetectionThreshold() const;
    int getColorStyleComboBoxSelectedId() const;

private:
    LfpLatencySpectrogram spectrogram; // Will contain the spectrogram image.
    // Make an editor to be friendly class of this content component,
    // so the editor will have access to all methods and variables of this component.
    friend class LfpLatencyProcessorVisualizer;

    ScopedPointer<LfpLatencySpectrogramControlPanel> spectrogramControlPanel;

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

    float stimulusVoltage;

    float stimulusVoltageMax;

    float stimulusVoltageMin;

	bool voltageTooHighOkay;
	bool alreadyAlerted = false;

    float trackSpike_DecreaseRate;
    float trackSpike_IncreaseRate;

	// setup

	ScopedPointer<Label> trackSpike_IncreaseRate_Slider_Label;
	ScopedPointer<Slider> trackSpike_IncreaseRate_Slider;
	ScopedPointer<TextEditor> trackSpike_IncreaseRate_Text;

	ScopedPointer<Label> trackSpike_DecreaseRate_Slider_Label;
	ScopedPointer<Slider> trackSpike_DecreaseRate_Slider;
	ScopedPointer<TextEditor> trackSpike_DecreaseRate_Text;

	ScopedPointer<Slider> stimulusVoltageSlider;
	ScopedPointer<Label> stimulusVoltageSliderLabel;

	ScopedPointer<TextEditor> stimulusVoltageMax_text;
	ScopedPointer<Label> stimulusVoltageMax_textLabel;

	ScopedPointer<TextEditor> stimulusVoltage_text;
	ScopedPointer<Label> stimulusVoltage_textLabel;

	ScopedPointer<TextEditor> stimulusVoltageMin_text;
	ScopedPointer<Label> stimulusVoltageMin_textLabel;
	
	// main GUI
    ScopedPointer<TextButton> setupButton;

    ScopedPointer<Slider> searchBoxSlider;
	ScopedPointer<Label> searchBoxSliderLabel;

	ScopedPointer<Label> ROIspikeLocationLabel;
    ScopedPointer<TextEditor> ROIspikeLocation;
	ScopedPointer<Label> msLabel;

	ScopedPointer<Label> ROIspikeValueLabel;
    ScopedPointer<TextEditor> ROIspikeValue;
	ScopedPointer<Label> mpersLabel;
    
    ScopedPointer<Slider> searchBoxWidthSlider;
	ScopedPointer<Label> searchBoxWidthSliderLabel;
    
    ScopedPointer<ComboBox> colorStyleComboBox;
	ScopedPointer<Label> colorStyleComboBoxLabel;
    
    ScopedPointer<ToggleButton> extendedColorScaleToggleButton;
    ScopedPointer<Label> extendedColorScaleToggleButtonLabel;
	
    ScopedPointer<Label> cmLabel;
 
    //ScopedPointer<GroupComponent> detectionControlGroup;

	// Stimulus control
	ScopedPointer<ppController> ppControllerComponent;

    ScopedPointer<TextEditor> textBox1;
    ScopedPointer<TextEditor> textBox2;

    ScopedPointer<ComboBox> triggerChannelComboBox;
	ScopedPointer<Label> triggerChannelComboBoxLabel;

    ScopedPointer<ComboBox> dataChannelComboBox;
	ScopedPointer<Label> dataChannelComboBoxLabel;

    ScopedPointer<Slider> Trigger_threshold; //TODO

    ScopedPointer<ToggleButton> trackSpike_button;
    ScopedPointer<Label> trackSpike_button_Label;

    ScopedPointer<ToggleButton> trackThreshold_button;
    ScopedPointer<Label> trackThreshold_button_Label;

    ScopedPointer<TextEditor> ROISpikeLatency;
	ScopedPointer<Label> ROISpikeLatencyLabel;

    ScopedPointer<TextEditor> ROISpikeMagnitude;
	ScopedPointer<Label> ROISpikeMagnitudeLabel;

    ScopedPointer<Slider> trigger_threshold_Slider;
	ScopedPointer<Label> trigger_threshold_Slider_Label;

	// In order to save values
	// This can't be a scoped pointer because scoped pointer actually deals with it badly because we redeclare it every time
	XmlElement *XmlValue;
    //DEBUG

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessorVisualizerContentComponent)
};

#endif // __JUCE_HEADER_D83F67960ECDED8C__
