// LfpLatencyElements.h

#ifndef LFPLATENCYELEMENTS_H
#define LFPLATENCYELEMENTS_H

#include <EditorHeaders.h>

class LfpLatencyLabelTextEditor : public Component
{
public:
    LfpLatencyLabelTextEditor(const String& labelText);
    void resized() override;

    void setTextEditorText(const String& newText);
private:
    ScopedPointer<Label> label;
    ScopedPointer<TextEditor> textEditor;
};

class LfpLatencyLabelSlider : public Component
{
public:
    LfpLatencyLabelSlider(const String& labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener* listener);

    double getSliderMaximum() const;
    double getSliderMinimum() const;
    void setSliderValue(double newValue);
    double getSliderValue() const; 
private:
    ScopedPointer<Label> label;
    ScopedPointer<Slider> slider;
};

class LfpLatencyLabelVerticalSlider : public Component
{
public:
    LfpLatencyLabelVerticalSlider(const String& labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener* listener);

    double getSliderMaximum() const;
    double getSliderMinimum() const;
    void setSliderValue(double newValue);
    double getSliderValue() const;
    void setSliderMaxValue(double newValue);
    double getSliderMaxValue() const;
    void setSliderMinValue(double newValue);
    double getSliderMinValue() const;
private:
    ScopedPointer<Label> label;
    ScopedPointer<Slider> slider;
};

#endif
