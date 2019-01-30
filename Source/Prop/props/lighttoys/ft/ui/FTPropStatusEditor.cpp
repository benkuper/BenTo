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
	isConnectedUI = ftp->isConnected->createToggle();
	addAndMakeVisible(isConnectedUI);
	voltageUI = ftp->voltage->createSlider();
	addAndMakeVisible(voltageUI);
	flashUI = ftp->flash->createButtonUI();
	addAndMakeVisible(flashUI);
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
