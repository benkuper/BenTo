/*
  ==============================================================================

    PropShapePanel.h
    Created: 26 Jun 2020 12:03:57pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class PropShapePanel :
    public ShapeShifterContentComponent,
    public Inspectable::InspectableListener,
    public ComboBox::Listener
{
public:
    PropShapePanel(const String &name);
    ~PropShapePanel();

    ComboBox shapeChooser;
    std::unique_ptr<PropShapeView> currentShapeView;

    void updateChooserList();

    void setCurrentShape(PropShape* p);
    void resized() override;

    // Inherited via Listener
    virtual void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;

    void inspectableDestroyed(Inspectable * i) override;

    static const String getTypeStringStatic() { return "Shape Editor"; }
    static PropShapePanel* create(const String& name) { return new PropShapePanel(name); }

};