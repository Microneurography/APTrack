#ifndef __SPIKEGROUPTABLECOMPONENT_HEADER__
#define __SPIKEGROUPTABLECOMPONENT_HEADER__
#include <EditorHeaders.h>
#include "LfpLatencyProcessor.h"

class LfpLatencyProcessor;

class SpikeGroupTableContent : public TableListBoxModel//, public ButtonListener
                     
{
public:
    enum Columns{spike_id_info=1, location_info=2, firing_probability_info=3, threshold_info=4, track_spike_button=5, threshold_spike_button=6, delete_button=7};
    const juce::Colour colorWheel[4] = {Colours::lightsteelblue, Colours::lightskyblue,Colours::darkgreen,Colours::orange};
    SpikeGroupTableContent(LfpLatencyProcessor* processor);
    ~SpikeGroupTableContent();
    
    /* These are all implementations of the functions defined in TableListBoxModel */
    
    int getNumRows();

    void paintRowBackground(Graphics& g, int rowNumber, int width, int height, bool rowIsSelected);
    
    void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);
    
    Component* refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component* existingComponetToUpdate);

    
    /* This is a custom class used to add custom cells with toggle buttons inside them, the helper functions above help */
    class SelectableColumnComponent : public juce::ToggleButton
    {
    public:
        SelectableColumnComponent(SpikeGroupTableContent& tcon);
        ~SelectableColumnComponent();

        ScopedPointer<ToggleButton> toggleButton;
        //void ButtonListener::buttonClicked (Button* button) override;
    private:
        SpikeGroupTableContent& owner;

    };
    
    
    /* This a custom class used to add custom cells that display data on tracked spike, with the updateInfo() function handling most of the work*/
    class UpdatingTextColumnComponent : public juce::TextEditor
                                       
    {
    public:
        
        UpdatingTextColumnComponent(SpikeGroupTableContent& tcon, int rowNumber, int columnNumber);
        ~UpdatingTextColumnComponent();

        ScopedPointer<TextEditor> value;

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