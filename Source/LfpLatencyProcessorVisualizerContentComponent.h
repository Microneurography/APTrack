#ifndef __JUCE_HEADER_D83F67960ECDED8C__
#define __JUCE_HEADER_D83F67960ECDED8C__

#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "pulsePalController/ppController.h"

#include "LfpLatencySpectrogram.h"
#include "LfpLatencySpectrogramPanel.h"
#include "LfpLatencySpectrogramControlPanel.h"
#include "LfpLatencyOtherControlPanel.h"
#include "LfpLatencyRightMiddlePanel.h"

class TableContent : public TableListBoxModel
                     
{
public:
   
    TableContent();
    ~TableContent();
    
    /* These are all implementations of the functions defined in TableListBoxModel */
    
    int getNumRows();

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    
    Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* exsistingComponetToUpdate);
    
    /* Update the table with new spike data */
    friend void updateInfo(TableContent &tc, int location, float fp, float threshold, int i);
    
    /* Returns whether the spike track button in the table has been toggled */
    friend bool getSpikeSelect(TableContent& tc, int row);
    
    /* Returns whether the threshold track button in the table has been toggled*/
    friend bool getThresholdSelect(TableContent& tc, int row);
    
    /* Returns an array of all the states of the buttons in a table*/
    friend Array <bool> getRow(TableContent& tc, bool spike, bool threshold);
    
    /* Toggles the tracking of a threshold */
    friend void selectThreshold(TableContent& tc, int row);
    
    /* Toggles the tracking of a spike */
    friend void selectSpike(TableContent& tc, int row);
    
    /* Determines which row has been selected to delete*/
    friend bool getRowToDelete(TableContent& tc, int row);
    
    /* Delete all data in a specific row */
    friend void deleteSpikeAndThreshold(TableContent& tc, int row);

    /* This structure is used to keep track of data to be displayed in the table*/
    struct tableData {
        int location;
        float firingProb;
        float threshold;
    };

    tableData info[4];

    /* This is a custom class used to add custom cells with toggle buttons inside them, the helper functions above help */
    class SelectableColumnComponent : public juce::ToggleButton
    {
    public:
        SelectableColumnComponent(TableContent& tcon);
        ~SelectableColumnComponent();

        ScopedPointer<ToggleButton> toggleButton;

    private:
        TableContent& owner;

    };
    
    /* This a custom class used to add custom cells that display data on tracked spike, with the updateInfo() function handling most of the work*/
    class UpdatingTextColumnComponent : public juce::TextEditor
                                       
    {
    public:
        
        UpdatingTextColumnComponent(TableContent& tcon, int rowNumber, int columnNumber);
        ~UpdatingTextColumnComponent();

        ScopedPointer<TextEditor> value;

    private:
        TableContent& owner;
        juce::Colour textColour;

    };
    class DeleteComponent : public juce::TextButton

    {
    public:

        DeleteComponent(TableContent& tcon);
        ~DeleteComponent();

        ScopedPointer<TextButton> del;

    private:
        TableContent& owner;

    };


private:    
    
    friend class LfpLatencyProcessorVisualizer;
    friend class LfpLatencyProcessorVisulizerContentComponent;

    bool trackSpikes[4];
    bool newSpikeFound[4];
    bool trackThresholds[4];
    bool newThresholdFound[4];
    bool keybind[4];

    bool thresholdAlreadyTracked;
    bool spikeAlreadyTracked;

    bool deleteSpike[4];
};


class LfpLatencyProcessorVisualizerContentComponent : public Component,
                                                      public SliderListener,
                                                      public ButtonListener
                                                 
{
public:
    LfpLatencyProcessorVisualizerContentComponent(LfpLatencyProcessor* processor);
    ~LfpLatencyProcessorVisualizerContentComponent();

    void paint(Graphics &g) override;
    void resized() override;
    void sliderValueChanged(Slider *sliderThatWasMoved) override;
    void buttonClicked(Button *buttonThatWasClicked) override;
	bool keyPressed(const KeyPress& k) override;
    //std::function<void()> onTextChange override;
    //void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;


    int getStartingSample() const;
    bool getExtendedColorScale() const;
    int getSubsamplesPerWindow() const;
    float getLowImageThreshold() const;
    float getHighImageThreshold() const;
    float getDetectionThreshold() const;
    int getColorStyleComboBoxSelectedId() const;
    void tryToSave();

    std::tuple<float, float, float, float, Colour> getSearchBoxInfo() const;

private:
    // Make an editor to be friendly class of this content component,
    // so the editor will have access to all methods and variables of this component.
    friend class LfpLatencyProcessorVisualizer;
    friend class TableContent;

    ScopedPointer<TableContent> spikeTrackerContent;
    ScopedPointer<LfpLatencySpectrogramControlPanel> spectrogramControlPanel;
    ScopedPointer<LfpLatencyOtherControlPanel> otherControlPanel;
    ScopedPointer<LfpLatencySpectrogramPanel> spectrogramPanel;
    ScopedPointer<LfpLatencyRightMiddlePanel> rightMiddlePanel;

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
    bool extendedColorScale;
    float detectionThreshold;
    int subsamplesPerWindow;
    int startingSample;

    float bitBolts;

    float conductionDistance;

    int absPos;

    int stimuli = 4;
    
    float stimulusVoltage;

    float stimulusVoltageMax;

    float stimulusVoltageMin;

	bool voltageTooHighOkay;
	bool alreadyAlerted = false;
    bool testSpikePls = false;

    bool deletes[4] = { false, false, false, false };

    bool t_deletes[4] = { false, false, false, false };

    float trackSpike_DecreaseRate;
    float trackSpike_IncreaseRate;

    bool isSaving;
    unordered_map<string, juce::String> *valuesMap;

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

    ScopedPointer<TableListBox> spikeTracker;

    ScopedPointer<Slider> stimuliNumberSlider;
    ScopedPointer<TextEditor> stimuliNumber;
    ScopedPointer<Label> stimuliNumberLabel;

    TableContent tcon;

    //DEBUG

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessorVisualizerContentComponent)
};

#endif // __JUCE_HEADER_D83F67960ECDED8C__
