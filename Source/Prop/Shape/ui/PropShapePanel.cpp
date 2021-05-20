/*
  ==============================================================================

    PropShapePanel.cpp
    Created: 26 Jun 2020 12:03:57pm
    Author:  bkupe

  ==============================================================================
*/

PropShapePanel::PropShapePanel(const String &name) :
    ShapeShifterContentComponent(name)
{
    shapeChooser.addListener(this);
    addAndMakeVisible(shapeChooser);
    updateChooserList();
}

PropShapePanel::~PropShapePanel()
{
}

void PropShapePanel::updateChooserList()
{
    shapeChooser.clear(dontSendNotification);
    shapeChooser.setTextWhenNothingSelected("Select a shape or create a new");
    int id = 1;
    for (auto& s : PropShapeLibrary::getInstance()->items)
    {
        shapeChooser.addItem(s->niceName, id++);
    }

    shapeChooser.addSeparator();
    shapeChooser.addItem("Create new shape...", -1);
}

void PropShapePanel::setCurrentShape(PropShape* p)
{
    if (currentShapeView != nullptr && currentShapeView->shape == p) return;

    if (currentShapeView != nullptr)
    {
        if (currentShapeView->shape != nullptr) currentShapeView->shape->removeInspectableListener(this);
        removeChildComponent(currentShapeView.get());
    }

    if (p != nullptr) currentShapeView.reset(new PropShapeView(p));
    else currentShapeView.reset();

    if (currentShapeView != nullptr)
    {
        currentShapeView->shape->removeInspectableListener(this);
        addAndMakeVisible(currentShapeView.get(), 0);
        resized();
        currentShapeView->updateItemsVisibility();
    }
}

void PropShapePanel::resized()
{
    if (currentShapeView != nullptr) currentShapeView->setBounds(getLocalBounds());
    shapeChooser.setBounds(getLocalBounds().removeFromLeft(150).removeFromTop(30).reduced(5));
}

void PropShapePanel::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    int id = shapeChooser.getSelectedId();
    if (id > 0) setCurrentShape(PropShapeLibrary::getInstance()->items[id - 1]);
    else if (id == -1)
    {
        PropShapeLibrary::getInstance()->addItem();
        updateChooserList();
        shapeChooser.setSelectedId(PropShapeLibrary::getInstance()->items.size());
    }
}


void PropShapePanel::inspectableDestroyed(Inspectable* i)
{
    if (i == currentShapeView->shape) setCurrentShape(nullptr);
}
