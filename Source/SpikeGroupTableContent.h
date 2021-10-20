#ifndef __SPIKEGROUPTABLECOMPONENT_HEADER__
#define __SPIKEGROUPTABLECOMPONENT_HEADER__
#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

class LfpLatencyProcessor;

class SpikeGroupTableContent : public TableListBoxModel, public ButtonListener
                     
{
public:
    enum Columns{spike_id_info=1, location_info=2, firing_probability_info=3, threshold_info=4, track_spike_button=5, threshold_spike_button=6, delete_button=7};
    const juce::Colour colorWheel[4] = {Colours::lightsteelblue, Colours::lightskyblue,Colours::darkgreen,Colours::orange};
    SpikeGroupTableContent(LfpLatencyProcessor* processor);
    ~SpikeGroupTableContent();
    
    /* These are all implementations of the functions defined in TableListBoxModel */
    
    int getNumRows() override;

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    
    Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* existingComponetToUpdate) override;
    void buttonClicked (Button* button) override;
    
    /* This is a custom class used to add custom cells with toggle buttons inside them, the helper functions above help */
    class SelectableColumnComponent : public juce::ToggleButton, public juce::ToggleButton::Listener
    {
    public:
        enum Action{TRACK_SPIKE,ACTIVATE_SPIKE};
        SelectableColumnComponent(SpikeGroupTableContent& tcon, int spikeID, Action action,LfpLatencyProcessor* processor);
        ~SelectableColumnComponent();
        void buttonClicked (juce::Button* b);
        void setSpikeID(int spikeID);

        ScopedPointer<ToggleButton> toggleButton;
        //void ButtonListener::buttonClicked (Button* button) override;
    private:
        LfpLatencyProcessor* processor;
        SpikeGroupTableContent& owner;
        int spikeID;
        Action action;

    };
    
    
    /* This a custom class used to add custom cells that display data on tracked spike, with the updateInfo() function handling most of the work*/
    class UpdatingTextColumnComponent : public juce::Label
                                       
    {
    public:
        
        UpdatingTextColumnComponent(SpikeGroupTableContent& tcon, int rowNumber, int columnNumber);
        ~UpdatingTextColumnComponent();

        ScopedPointer<Label> value;

    private:
        SpikeGroupTableContent& owner;
        juce::Colour textColour;

    };
    class DeleteComponent : public juce::TextButton

    {
    public:

        DeleteComponent(SpikeGroupTableContent& tcon);
        ~DeleteComponent();

        ScopedPointer<TextButton> del;

    private:
        SpikeGroupTableContent& owner;

    };


private:    
    LfpLatencyProcessor* processor;
};

#endif