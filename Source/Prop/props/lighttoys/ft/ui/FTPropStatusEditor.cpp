/*
  ==============================================================================

	FTPropStatusEditor.cpp
	Created: 30 Jan 2019 5:46:49pm
	Author:  bkupe

  ==============================================================================
*/

#include "FTPropStatusEditor.h"

FTPropStatusEditor::FTPropStatusEditor(FTPropStatus * ftp, bool isRoot) :
	InspectableEditor(ftp, isRoot)
{
	isConnectedUI.reset(ftp->isConnected->createToggle());
	addAndMakeVisible(isConnectedUI.get());
	voltageUI.reset( ftp->voltage->createSlider());
	addAndMakeVisible(voltageUI.get());
	flashUI.reset(ftp->flash->createButtonUI());
	addAndMakeVisible(flashUI.get());
	setSize(100, 14);
}

FTPropStatusEditor::~FTPropStatusEditor()
{
}

void FTPropStatusEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	isConnectedUI->setBounds(r.removeFromLeft(r.getHeight()));
	r.removeFromLeft(2);
	flashUI->setBounds(r.removeFromRight(50));
	r.removeFromRight(2);

	voltageUI->setBounds(r);
}
