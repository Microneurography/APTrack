#include "SpikeGroupTableContent.h"

int SpikeGroupTableContent::getNumRows()
{
	return this->processor->getSpikeGroupCount();
}

SpikeGroupTableContent::SpikeGroupTableContent(LfpLatencyProcessor* processor)
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
				selectionBox = new SelectableColumnComponent(*this);
				//selectionBox->addListener(this);
			}
			return selectionBox;
		}
		if (columnId == Columns::threshold_spike_button)
		{
			auto *selectionBox = static_cast<SelectableColumnComponent *>(existingComponetToUpdate);
			if (selectionBox == nullptr)
			{
				selectionBox = new SelectableColumnComponent(*this);
				//selectionBox->addListener(this);
			}
			return selectionBox;
		}
		if (columnId == Columns::location_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			//label->setText(to_string(info[rowNumber].location));
			//label->repaint();
			return label;
		}
		if (columnId == Columns::firing_probability_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			// label->setText(to_string(info[rowNumber].firingProb));
			// label->repaint();
			
			return label;
		}
		if (columnId == Columns::threshold_info)
		{
			auto *label = dynamic_cast<UpdatingTextColumnComponent *>(existingComponetToUpdate);

			if (label == nullptr)
			{
				label = new UpdatingTextColumnComponent(*this, rowNumber, columnId);
			}
			// label->setText(to_string(info[rowNumber].threshold));
			// label->repaint();
			
			return label;
		}
	}
	jassert(existingComponetToUpdate == nullptr);
	return nullptr;
}

SpikeGroupTableContent::UpdatingTextColumnComponent::UpdatingTextColumnComponent(SpikeGroupTableContent &tcon, int rowNumber, int columnNumber) : owner(tcon)
{
	addAndMakeVisible(value = new TextEditor("_"));
}

SpikeGroupTableContent::UpdatingTextColumnComponent::~UpdatingTextColumnComponent()
{
	value = nullptr;
}

SpikeGroupTableContent::SelectableColumnComponent::SelectableColumnComponent(SpikeGroupTableContent &tcon) : owner(tcon)
{
	addAndMakeVisible(toggleButton = new ToggleButton);
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
