// LfpLatencyElements.h
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

#ifndef LFPLATENCYELEMENTS_H
#define LFPLATENCYELEMENTS_H

#include <EditorHeaders.h>

class LfpLatencyLabelTextEditor : public Component
{
public:
    LfpLatencyLabelTextEditor(const String &labelText);
    void resized() override;

    void setTextEditorText(const String &newText);

private:
    ScopedPointer<Label> label;
    ScopedPointer<TextEditor> textEditor;
};

class LfpLatencyLabelSlider : public Component
{
public:
    LfpLatencyLabelSlider(const String &labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener *listener);

    double getSliderMaximum() const;
    double getSliderMinimum() const;
    void setSliderValue(double newValue);
    double getSliderValue() const;

private:
    ScopedPointer<Label> label;
    ScopedPointer<Slider> slider;
};

class LfpLatencyLabelHorizontalSlider : public Component
{
public:
    LfpLatencyLabelHorizontalSlider(const String &labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener *listener);

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

class LfpLatencyLabelComboBox : public Component
{
public:
    LfpLatencyLabelComboBox(const String &labelText);
    void resized() override;

    int getComboBoxSelectedId() const;
    void setComboBoxSelectedId(int newItemId);
    void setComboBoxTextWhenNothingSelected(const String &newMessage);
    int getComboBoxNumItems() const;
    void addComboBoxItem(const String &newItemText, int newItemId);
    void addComboBoxSectionHeading(const String &headingName);
    void clearComboBox();

private:
    ScopedPointer<Label> label;
    ScopedPointer<ComboBox> comboBox;
};

class LfpLatencyLabelToggleButton : public Component
{
public:
    LfpLatencyLabelToggleButton(const String &labelText);
    void resized() override;

    void addToggleButtonListener(Button::Listener *listener);
    bool getToggleButtonState() const;
    void setToggleButtonState(bool shouldBeOn, NotificationType notification);

private:
    ScopedPointer<Label> label;
    ScopedPointer<ToggleButton> toggleButton;
};

class LfpLatencyLabelSliderNoTextBox : public Component
{
public:
    LfpLatencyLabelSliderNoTextBox(const String &labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener *listener);
    void setSliderValue(double newValue);
    double getSliderValue() const;

private:
    ScopedPointer<Label> label;
    ScopedPointer<Slider> slider;
};

class LfpLatencyLabelLinearVerticalSliderNoTextBox : public Component
{
public:
    LfpLatencyLabelLinearVerticalSliderNoTextBox(const String &labelText);
    void resized() override;

    void setSliderRange(double newMinimum, double newMaximum, double newInterval = 0);
    void addSliderListener(Slider::Listener *listener);
    void setSliderValue(double newValue);
    double getSliderValue() const;

private:
    ScopedPointer<Label> label;
    ScopedPointer<Slider> slider;
};

class LfpLatencyProcessorVisualizerContentComponent;
class LfpLatencySpectrogram;
class LfpLatencySearchBox : public Component
{
public:
    /* LfpLatencySearchBox must have the same bounds as the spectrogram image for correct positioning*/
    LfpLatencySearchBox(const LfpLatencyProcessorVisualizerContentComponent &content, const LfpLatencySpectrogram &spectrogram);
    void paint(Graphics &g) override;

private:
    const LfpLatencyProcessorVisualizerContentComponent &content;
    const LfpLatencySpectrogram &spectrogram;
};

#endif
