// LfpLatencyElements.cpp

#include "LfpLatencyElements.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

// -------------------------------------------------------------
LfpLatencyLabelTextEditor::LfpLatencyLabelTextEditor(const String& labelText)
{  
    label = new Label("Label", labelText);
    textEditor = new TextEditor("TextEditor");

    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(textEditor, true);

    addAndMakeVisible(label);
    addAndMakeVisible(textEditor);
}

void LfpLatencyLabelTextEditor::resized()
{
    auto textEditorLeft = getWidth() / 3;
    textEditor->setBounds(textEditorLeft, 0, getWidth() - textEditorLeft, getHeight());
}

void LfpLatencyLabelTextEditor::setTextEditorText(const String& newText) {
    textEditor->setText(newText);
}

// -------------------------------------------------------------
LfpLatencyLabelSlider::LfpLatencyLabelSlider(const String& labelText)
{
    label = new Label("Label", labelText);
    slider = new Slider(Slider::Rotary, Slider::TextBoxRight);
    slider->setName(labelText);

    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(slider, true);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void LfpLatencyLabelSlider::resized()
{
    auto sliderLeft = getWidth() / 3;
    slider->setBounds(sliderLeft, 0, getWidth() - sliderLeft, getHeight());
}

void LfpLatencyLabelSlider::setSliderRange(double newMinimum, double newMaximum, double newInterval)
{
    slider->setRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencyLabelSlider::addSliderListener(Slider::Listener* listener)
{
    slider->addListener(listener);
}

double LfpLatencyLabelSlider::getSliderMaximum() const {
    return slider->getMaximum();
}

double LfpLatencyLabelSlider::getSliderMinimum() const {
    return slider->getMinimum();
}

void LfpLatencyLabelSlider::setSliderValue(double newValue)
{
    slider->setValue(newValue, juce::NotificationType::dontSendNotification);
}

double LfpLatencyLabelSlider::getSliderValue() const
{
    return slider->getValue();
}

// -------------------------------------------------------------
LfpLatencyLabelVerticalSlider::LfpLatencyLabelVerticalSlider(const String& labelText)
{
    label = new Label("Label", labelText);
    slider = new Slider(Slider::ThreeValueVertical, Slider::NoTextBox);
    slider->setName(labelText);

    label->setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void LfpLatencyLabelVerticalSlider::resized()
{
    auto labelHeight = 24;
    slider->setBounds(0, 0, getWidth(), getHeight() - labelHeight);

    // Label::attachToComponent() only attach to left or top; have to config it manually
    label->setBounds(0, slider->getY() + slider->getHeight() , getWidth(), labelHeight);
}

void LfpLatencyLabelVerticalSlider::setSliderRange(double newMinimum, double newMaximum, double newInterval)
{
    slider->setRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencyLabelVerticalSlider::addSliderListener(Slider::Listener* listener)
{
    slider->addListener(listener);
}

double LfpLatencyLabelVerticalSlider::getSliderMaximum() const {
    return slider->getMaximum();
}

double LfpLatencyLabelVerticalSlider::getSliderMinimum() const {
    return slider->getMinimum();
}

void LfpLatencyLabelVerticalSlider::setSliderValue(double newValue)
{
    slider->setValue(newValue);
}

double LfpLatencyLabelVerticalSlider::getSliderValue() const
{
    return slider->getValue();
}

void LfpLatencyLabelVerticalSlider::setSliderMaxValue(double newValue)
{
    slider->setMaxValue(newValue);
}

double LfpLatencyLabelVerticalSlider::getSliderMaxValue() const
{
    return slider->getMaxValue();
}

void LfpLatencyLabelVerticalSlider::setSliderMinValue(double newValue)
{
    slider->setMinValue(newValue);
}

double LfpLatencyLabelVerticalSlider::getSliderMinValue() const
{
    return slider->getMinValue();
}

// -------------------------------------------------------------
LfpLatencyLabelComboBox::LfpLatencyLabelComboBox(const String& labelText)
{
    label = new Label("Label", labelText);
    comboBox = new ComboBox("ComboBox");

    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(comboBox, true);

    comboBox->setEditableText(false);
    comboBox->setJustificationType(Justification::centredLeft);

    comboBox->setSelectedId(1);

    addAndMakeVisible(label);
    addAndMakeVisible(comboBox);
}

void LfpLatencyLabelComboBox::resized()
{
    auto comboBoxLeft = getWidth() * 2 / 3;
    comboBox->setBounds(getLocalBounds().withTrimmedLeft(comboBoxLeft));
}

int LfpLatencyLabelComboBox::getComboBoxSelectedId() const
{
    return comboBox->getSelectedId();
}

void LfpLatencyLabelComboBox::setComboBoxSelectedId(int newItemId)
{
    comboBox->setSelectedId(newItemId);
}

void LfpLatencyLabelComboBox::setComboBoxTextWhenNothingSelected(const String& newMessage)
{
    comboBox->setTextWhenNothingSelected(newMessage);
}

int LfpLatencyLabelComboBox::getComboBoxNumItems() const
{
    return comboBox->getNumItems();
}

void LfpLatencyLabelComboBox::addComboBoxItem(const String& newItemText, int newItemId)
{
    comboBox->addItem(newItemText, newItemId);
}

void LfpLatencyLabelComboBox::addComboBoxSectionHeading(const String& headingName)
{
    comboBox->addSectionHeading(headingName);
}

void LfpLatencyLabelComboBox::clearComboBox()
{
    comboBox->clear();
}

// -------------------------------------------------------------
LfpLatencyLabelToggleButton::LfpLatencyLabelToggleButton(const String& labelText)
{
    label = new Label("Label", labelText);
    toggleButton = new ToggleButton("");

    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(toggleButton, true);

    toggleButton->setName(labelText);
    toggleButton->setColour(ToggleButton::ColourIds::tickDisabledColourId, Colours::lightgrey);

    addAndMakeVisible(label);
    addAndMakeVisible(toggleButton);
}

void LfpLatencyLabelToggleButton::resized()
{
    auto toggleButtonLeft = getWidth() * 2 / 3;
    toggleButton->setBounds(getLocalBounds().withTrimmedLeft(toggleButtonLeft));
}

void LfpLatencyLabelToggleButton::addToggleButtonListener(Button::Listener* listener)
{
    toggleButton->addListener(listener);
}

bool LfpLatencyLabelToggleButton::getToggleButtonState() const
{
    return toggleButton->getToggleState();
}

void LfpLatencyLabelToggleButton::setToggleButtonState(bool shouldBeOn, NotificationType notification)
{
    toggleButton->setToggleState(shouldBeOn, notification);
}

// -------------------------------------------------------------
LfpLatencyLabelSliderNoTextBox::LfpLatencyLabelSliderNoTextBox(const String& labelText)
{
    label = new Label("Label", labelText);
    slider = new Slider(Slider::Rotary, Slider::NoTextBox);
    slider->setName(labelText);

    label->setJustificationType(juce::Justification::centred);
    label->attachToComponent(slider, true);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void LfpLatencyLabelSliderNoTextBox::resized()
{
    auto sliderWidth = 50;
    slider->setBounds(getLocalBounds().removeFromRight(sliderWidth));
}

void LfpLatencyLabelSliderNoTextBox::setSliderRange(double newMinimum, double newMaximum, double newInterval)
{
    slider->setRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencyLabelSliderNoTextBox::addSliderListener(Slider::Listener* listener)
{
    slider->addListener(listener);
}

void LfpLatencyLabelSliderNoTextBox::setSliderValue(double newValue)
{
    slider->setValue(newValue);
}

double LfpLatencyLabelSliderNoTextBox::getSliderValue() const
{
    return slider->getValue();
}

// -------------------------------------------------------------
LfpLatencyLabelLinearVerticalSliderNoTextBox::LfpLatencyLabelLinearVerticalSliderNoTextBox(const String& labelText)
{
    label = new Label("Label", labelText);
    slider = new Slider(Slider::LinearVertical, Slider::NoTextBox);
    slider->setName(labelText);

    label->setJustificationType(juce::Justification::centred);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

void LfpLatencyLabelLinearVerticalSliderNoTextBox::resized()
{
    auto area = getLocalBounds();
    auto sliderWidth = 15;
    slider->setBounds(area.removeFromLeft(sliderWidth));
    // auto labelHeight = 24;
    // label->setBounds(area.removeFromBottom(labelHeight));
}

void LfpLatencyLabelLinearVerticalSliderNoTextBox::setSliderRange(double newMinimum, double newMaximum, double newInterval)
{
    slider->setRange(newMinimum, newMaximum, newInterval);
}

void LfpLatencyLabelLinearVerticalSliderNoTextBox::addSliderListener(Slider::Listener* listener)
{
    slider->addListener(listener);
}

void LfpLatencyLabelLinearVerticalSliderNoTextBox::setSliderValue(double newValue)
{
    slider->setValue(newValue, juce::NotificationType::dontSendNotification);
}

double LfpLatencyLabelLinearVerticalSliderNoTextBox::getSliderValue() const
{
    return slider->getValue();
}

// -------------------------------------------------------------
LfpLatencySearchBox::LfpLatencySearchBox(const LfpLatencyProcessorVisualizerContentComponent& content, const LfpLatencySpectrogram& spectrogram)
:content(content), spectrogram(spectrogram) {}

void LfpLatencySearchBox::paint(Graphics& g)
{
    // Ideally this would map directly to the same scaling as the visualizer window. move back into original render?

    Colour colour;
    float x, y, width, height;
    std::tie(x, y, width, height, colour) = content.getSearchBoxInfo();
    g.setColour(colour);

    auto area = getLocalBounds().toFloat();
    x = jmap(x, 0.0f, (float)spectrogram.getImageWidth(), area.getX(), area.getRight());
    width = jmap(width, 0.0f, (float)spectrogram.getImageWidth(), area.getX(), area.getRight());
    // this is very confusing. it also doesn't work as the range of the slider isn't from 0->top of image. this leads to a small (and annoying) offset.
    y = jmap(y, 0.0f, (float)spectrogram.getImageHeight(), area.getY(), area.getBottom());
    height = jmap(height, 0.0f, (float)spectrogram.getImageHeight() * content.getSubsamplesPerWindow(), area.getY(), area.getBottom());

    auto cornerSize = 1;
    auto lineThickness = 2;
    g.drawRoundedRectangle(x, y-(height/2), width, height, cornerSize, lineThickness);
}
