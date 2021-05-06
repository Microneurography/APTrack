// LfpLatencySpectrogramPanel.cpp

#include "LfpLatencySpectrogramPanel.h"
#include "LfpLatencyElements.h"
#include "LfpLatencySpectrogram.h"
#include "LfpLatencyProcessorVisualizerContentComponent.h"

LfpLatencySpectrogramPanel::LfpLatencySpectrogramPanel(LfpLatencyProcessorVisualizerContentComponent* content)
    : content(*content)
{
    outline = new GroupComponent("Spectrogram");
    spectrogram = new LfpLatencySpectrogram();
    searchBox = new LfpLatencyLabelLinearVerticalSliderNoTextBox("Search Box");
    searchBoxWidth = new LfpLatencyLabelSliderNoTextBox("Search Box Width");
    searchBoxRectangle = new LfpLatencySearchBox(*content, *spectrogram);

    searchBox->setSliderRange(0, spectrogram->getImageHeight(), 1);
    searchBox->addSliderListener(content);
    searchBox->setSliderValue(10);

    searchBoxWidth->setSliderRange(1, 63, 1);
    searchBoxWidth->addSliderListener(content);
    searchBoxWidth->setSliderValue(3);

    addAndMakeVisible(outline);
    addAndMakeVisible(spectrogram);
    addAndMakeVisible(searchBox);
    addAndMakeVisible(searchBoxWidth);
    addAndMakeVisible(searchBoxRectangle);
}

void LfpLatencySpectrogramPanel::resized()
{
    auto area = getLocalBounds();
    outline->setBounds(area);

    auto borderWidth = 20;
    area = area.withSizeKeepingCentre(getWidth() - 2 * borderWidth, getHeight() - 2 * borderWidth);

    auto sliderHeight = 64;
    auto searchBoxWidthArea = area.removeFromBottom(sliderHeight);

    auto searchBoxWidthMaxWidth = 175;
    searchBoxWidth->setBounds(searchBoxWidthArea.removeFromRight(searchBoxWidthMaxWidth));

    auto widthOfSearchBox = 85;
    auto searchBoxArea = area.removeFromRight(widthOfSearchBox);
    auto offset = 5;
    searchBoxArea.setX(searchBoxArea.getX() - offset);
    searchBoxArea.setWidth(searchBoxArea.getWidth() + offset);
    searchBox->setBounds(searchBoxArea);

    spectrogram->setBounds(area);

    searchBoxRectangle->setBounds(area);
}

void LfpLatencySpectrogramPanel::updateSpectrogram(LfpLatencyProcessor& processor, const LfpLatencyProcessorVisualizerContentComponent& content)
{
    spectrogram->update(processor, content);
}

void LfpLatencySpectrogramPanel::setSearchBoxValue(double newValue)
{
    searchBox->setSliderValue(newValue);
}

double LfpLatencySpectrogramPanel::getSearchBoxValue() const
{
    return searchBox->getSliderValue();
}

void LfpLatencySpectrogramPanel::changeSearchBoxValue(double deltaValue)
{
    setSearchBoxValue(getSearchBoxValue() + deltaValue);
}

void LfpLatencySpectrogramPanel::setSearchBoxWidthValue(double newValue)
{
    searchBoxWidth->setSliderValue(newValue);
}

int LfpLatencySpectrogramPanel::getImageHeight() const
{
    return spectrogram->getImageHeight();
}

int LfpLatencySpectrogramPanel::getImageWidth() const
{
    return spectrogram->getImageWidth();
}
