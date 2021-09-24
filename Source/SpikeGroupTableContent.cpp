#include "SpikeGroupTableContent.h"

int SpikeGroupTableContent::getNumRows()
{
	return this->processor->getSpikeGroupCount();
}

SpikeGroupTableContent::SpikeGroupTableContent(LfpLatencyProcessor *processor)
{
	this->processor = processor;
	// for (int j = 0; j < 4; j++)
	// {
	// 	info[j].location = 0;
	// 	info[j].firingProb = 0;
	// 	info[j].threshold = 0;
	// 	trackSpikes[j] = false;
	// 	newSpikeFound[j] = false;
	// 	newThresholdFound[j] = false;
	// 	trackThresholds[j] = false;
	// 	deleteSpike[j] = false;
	// 	keybind[j] = false;
	// }
	// spikeAlreadyTracked = false;
	// thresholdAlreadyTracked = false;
}

SpikeGroupTableContent::~SpikeGroupTableContent()
{
}
//void SpikeGroupTableContent::ButtonListener::buttonClicked (Button* button){};
void SpikeGroupTableContent::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{

	g.setColour(Colours::black); // [5]
	Font font = 12.0f;
	g.setFont(font);

	if (columnId == Columns::spike_id_info)
	{
		auto text = std::to_string(rowNumber + 1);

		g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true); // [6]
	}

	g.setColour(Colours::transparentWhite);
	g.fillRect(width - 1, 0, 1, height);
}

void SpikeGroupTableContent::paintRowBackground(Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{

	if (rowIsSelected)
	{
		g.fillAll(colorWheel[rowNumber % 4]);
	}
	else
	{
		g.fillAll(Colours::lightgrey);
	}
}

Component *SpikeGroupTableContent::refreshComponentForCell(int rowNumber, int columnId, bool rowIsSelected, Component *existingComponetToUpdate)
{
	if (rowNumber < getNumRows())
	{
		auto spikeGroup = processor->getSpikeGroup(rowNumber);;
		if (columnId == Columns::delete_button)
		{
			auto *deleteButton = static_cast<DeleteComponent *>(existingComponetToUpdate);

			if (deleteButton == nullptr)
			{
				deleteButton = new DeleteComponent(*this);
				//deleteButton->addListener(this);
			}
			return deleteButton;
		}
		if (columnId == Columns::track_spike_button)
		{
			auto *selectionBox = static_cast<SelectableColumnComponent *>(existingComponetToUpdate);

			if (selectionBox == nullptr)
			{
				selectionBox = new SelectableColumnComponent(*this, rowNumber, SpikeGroupTableContent::SelectableColumnComponent::Action::ACTIVATE_SPIKE, processor);
			}
			selectionBox->setSpikeID(rowNumber);
			selectionBox->setToggleState(spikeGroup->isActive, juce::NotificationType::dontSendNotification);
			return selectionBox;
		}
		if (columnId == Columns::threshold_spike_button)
		{
			auto *selectionBox = static_cast<SelectableColumnComponent *>(existingComponetToUpdate);
			if (selectionBox == nullptr)
			{
				selectionBox = new SelectableColumnComponent(*this, rowNumber, SpikeGroupTableContent::SelectableColumnComponent::Action::TRACK_SPIKE, processor);
			}
			selectionBox->setSpikeID(rowNumber);
			selectionBox->setToggleState(spikeGroup->isTracking, juce::NotificationType::dontSendNotification);
			return selectionBox;
		}
		if (columnId == Columns::location_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			label->setText(std::to_string(spikeGroup->templateSpike.spikeSampleLatency));
			label->repaint();
			return label;
		}
		if (columnId == Columns::firing_probability_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			label->setText(std::to_string(100*std::count(spikeGroup->recentHistory.begin(), spikeGroup->recentHistory.end(),true)/spikeGroup->recentHistory.size()));
			label->repaint();

			return label;
		}
		if (columnId == Columns::threshold_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			label->setText(std::to_string(spikeGroup->templateSpike.threshold));
			label->repaint();

			return label;
		}
	}
	jassert(existingComponetToUpdate == nullptr);
	return nullptr;
}

void SpikeGroupTableContent::buttonClicked (juce::Button* button) {
	button->getToggleState();
} 
SpikeGroupTableContent::UpdatingTextColumnComponent::UpdatingTextColumnComponent(SpikeGroupTableContent &tcon, int rowNumber, int columnNumber) : owner(tcon)
{
	addAndMakeVisible(value = new TextEditor("_"));
}

SpikeGroupTableContent::UpdatingTextColumnComponent::~UpdatingTextColumnComponent()
{
	value = nullptr;
}

SpikeGroupTableContent::SelectableColumnComponent::SelectableColumnComponent(SpikeGroupTableContent& tcon, int spikeID, Action action,LfpLatencyProcessor* processor): owner(tcon), spikeID(spikeID), action(action)
{
	this->processor = processor;
	addAndMakeVisible(toggleButton = new ToggleButton);
	this->addListener(this);
	//toggleButton->addListener(this);
}
void SpikeGroupTableContent::SelectableColumnComponent::buttonClicked (juce::Button* b){
	auto state = b->getToggleState();
	for (int x=0;x<this->processor->getSpikeGroupCount();x++){

		auto sg = this->processor->getSpikeGroup(x);
		if (x==this->spikeID){
			if(action == Action::ACTIVATE_SPIKE)
				sg->isActive=state;
			else if (action == Action::TRACK_SPIKE)
				sg->isTracking=state;
		}
		else{
			if(action == Action::ACTIVATE_SPIKE)
				sg->isActive = false;
			else if (action == Action::TRACK_SPIKE)
				sg->isTracking= false;

		}
	}


}
void SpikeGroupTableContent::SelectableColumnComponent::setSpikeID(int spikeID){
	this->spikeID = spikeID;
}
SpikeGroupTableContent::SelectableColumnComponent::~SelectableColumnComponent()
{
	toggleButton = nullptr;
}

SpikeGroupTableContent::DeleteComponent::DeleteComponent(SpikeGroupTableContent &tcon) : owner(tcon)
{
	addAndMakeVisible(del = new TextButton);
	del->setColour(TextButton::ColourIds::buttonColourId, Colours::white);
}

SpikeGroupTableContent::DeleteComponent::~DeleteComponent()
{
	del = nullptr;
}
