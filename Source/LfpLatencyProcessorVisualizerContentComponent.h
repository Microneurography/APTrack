/*
    ------------------------------------------------------------------

    This file is part of APTrack, a plugin for the Open-Ephys Gui
    
    Copyright (C) 2019-2023 Eli Lilly and Company, University of Bristol, Open Ephys
    Authors: Aidan Nickerson, Grace Stangroome, Merle Zhang, James O'Sullivan, Manuel Martinez

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef __JUCE_HEADER_D83F67960ECDED8C__
#define __JUCE_HEADER_D83F67960ECDED8C__

#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"
#include "pulsePalController/ppControllerVisualizer.h"

#include "LfpLatencySpectrogram.h"
#include "LfpLatencySpectrogramPanel.h"
#include "LfpLatencySpectrogramControlPanel.h"
#include "LfpLatencyOtherControlPanel.h"
#include "LfpLatencyRightMiddlePanel.h"
#include "SpikeGroupTableContent.h"

class LfpLatencySpectrogramControlPanel;
class LfpLatencyOtherControlPanel;
class LfpLatencyRightMiddlePanel;
class SpikeGroupTableContent;
class LfpLatencySpectrogramPanel;
class LfpLatencyProcessorVisualizerContentComponent : public Component,
                                                      public Slider::Listener,
                                                      public Button::Listener

{
public:
    LfpLatencyProcessorVisualizerContentComponent(LfpLatencyProcessor *processor);
    ~LfpLatencyProcessorVisualizerContentComponent();

    void paint(Graphics &g) override;
    void resized() override;
    void sliderValueChanged(Slider *sliderThatWasMoved) override;
    void buttonClicked(Button *buttonThatWasClicked) override;
    bool keyPressed(const KeyPress &k) override;
    //std::function<void()> onTextChange override;
    void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;

    int getSearchBoxSampleLocation();
    void setSearchBoxSampleLocation(int x);

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
    friend class LfpLatencySpectrogramControlPanel;

    ScopedPointer<LfpLatencyProcessor> processor;
    ScopedPointer<SpikeGroupTableContent> spikeTrackerContent;
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

    bool deletes[4] = {false, false, false, false};

    bool t_deletes[4] = {false, false, false, false};

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
    ScopedPointer<ppControllerVisualizer> ppControllerComponent;

    ScopedPointer<TextEditor> textBox1;
    ScopedPointer<TextEditor> textBox2;

    ScopedPointer<ComboBox> triggerChannelComboBox;
    ScopedPointer<Label> triggerChannelComboBoxLabel;

    ScopedPointer<ComboBox> dataChannelComboBox;
    ScopedPointer<Label> dataChannelComboBoxLabel;

    ScopedPointer<Slider> Trigger_threshold; //TODO

    ScopedPointer<TableListBox> spikeTracker;
    ScopedPointer<TextButton> addNewSpikeButton;

    ScopedPointer<Slider> stimuliNumberSlider;
    ScopedPointer<TextEditor> stimuliNumber;
    ScopedPointer<Label> stimuliNumberLabel;

    SpikeGroupTableContent tcon;

    std::unique_ptr<Component>  createSetupView();
    std::unique_ptr<Component>  stimulusSettingsView;

    //DEBUG

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LfpLatencyProcessorVisualizerContentComponent)
};

#endif // __JUCE_HEADER_D83F67960ECDED8C__
