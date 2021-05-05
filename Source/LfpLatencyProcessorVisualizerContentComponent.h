#ifndef __JUCE_HEADER_D83F67960ECDED8C__
#define __JUCE_HEADER_D83F67960ECDED8C__

#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

#include "pulsePalController/ppController.h"

#include "LfpLatencySpectrogram.h"
#include "LfpLatencySpectrogramControlPanel.h"


class TableContent : public TableListBoxModel
                     
{
public:
   
    TableContent();
    ~TableContent();
    
    int getNumRows();
    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    //Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* exsistingComponetToUpdate);

private:    
    
    friend class LfpLatencyProcessorVisualizer;
    
    bool spikeFound = false;


};


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

    ScopedPointer<TableContent> spikeTrackerContent;
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

    bool spikeDetected = false;
    bool newSpikeDetected = false;
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
    bool testSpikePls = false;

    bool del_0 = false;
    bool del_1 = false;
    bool del_2 = false;
    bool del_3 = false;

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
    ScopedPointer<TextButton> optionsButton;

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

    //ScopedPointer<ComboBox> trackSpikeComboBox;
    ScopedPointer<TableListBox> spikeTracker;
    
    ScopedPointer<TextEditor> location0;
    ScopedPointer<TextEditor> location1;
    ScopedPointer<TextEditor> location2;
    ScopedPointer<TextEditor> location3;

    ScopedPointer<TextEditor> fp0;
    ScopedPointer<TextEditor> fp1;
    ScopedPointer<TextEditor> fp2;
    ScopedPointer<TextEditor> fp3;
    
    ScopedPointer<ToggleButton> follow0;
    ScopedPointer<ToggleButton> follow1;
    ScopedPointer<ToggleButton> follow2;
    ScopedPointer<ToggleButton> follow3;

    ScopedPointer<TextButton> del0;
    ScopedPointer<TextButton> del1;
    ScopedPointer<TextButton> del2;
    ScopedPointer<TextButton> del3;

    ScopedPointer<ToggleButton> trackThreshold_button;
    ScopedPointer<Label> trackThreshold_button_Label;

    ScopedPointer<TextEditor> ROISpikeLatency;
	ScopedPointer<Label> ROISpikeLatencyLabel;

    ScopedPointer<TextEditor> ROISpikeMagnitude;
	ScopedPointer<Label> ROISpikeMagnitudeLabel;

    ScopedPointer<Slider> trigger_threshold_Slider;
	ScopedPointer<Label> trigger_threshold_Slider_Label;


    //DEBUG

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessorVisualizerContentComponent)
};

#endif // __JUCE_HEADER_D83F67960ECDED8C__
