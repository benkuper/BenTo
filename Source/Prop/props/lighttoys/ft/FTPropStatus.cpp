/*
  ==============================================================================

    FTPropStatus.cpp
    Created: 30 Jan 2019 5:47:03pm
    Author:  bkupe

  ==============================================================================
*/

#include "FTPropStatus.h"
#include "ui/FTPropStatusEditor.h"

FTPropStatus::FTPropStatus(int propID) :
	ControllableContainer("Device "+String(propID)),
	propID(propID)
{
	isConnected = addBoolParameter("Is Connected", "Is this prop connected", false);
	isConnected->setControllableFeedbackOnly(true);
	voltage = addFloatParameter("Voltage ", "Prop voltage", 0, 0, 15);
	voltage->setControllableFeedbackOnly(true);
	flash = addTrigger("Flash", "Flash the prop");

}

FTPropStatus::~FTPropStatus()
{
}

InspectableEditor * FTPropStatus::getEditor(bool isRoot)
{
	return new FTPropStatusEditor(this, isRoot);
}
