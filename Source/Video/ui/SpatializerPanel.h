/*
  ==============================================================================

    SpatializerPanel.h
    Created: 23 Apr 2018 9:42:51pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

class TextureBlock;

class SpatLayoutView :
	public BaseManagerShapeShifterUI<BaseManager<SpatItem>, SpatItem, SpatItemUI>
{
public:
	SpatLayoutView(Spatializer * spat, SpatLayout * layout);
	~SpatLayoutView();

	SpatItemUI * createUIForItem(SpatItem * i) override;

	Spatializer * spat;
	SpatLayout * layout;
	void resized() override;

	Point<float> getPositionForRelative(Point<float> relPos);
	Point<float> getRelativeForPosition(Point<float> absolutePos);
};


class SpatializerPanel :
	public ShapeShifterContentComponent,
	public ContainerAsyncListener,
	public SpatializerAsyncListener,
	public Timer
{ 
public:
	SpatializerPanel(const String &name);
	~SpatializerPanel();

	TextureBlock * textureBlock;
	Image bgImage;
	bool needsRepaint;

	std::unique_ptr<SpatLayoutView> currentLayoutView;
	void setCurrentLayoutView(SpatLayout * newView);

	void setTextureBlock(TextureBlock* b);

	void paint(Graphics &g) override;
	void resized() override;
	
	void mouseDown(const MouseEvent& e) override;

	void newMessage(const ContainerAsyncEvent &e) override;
	void newMessage(const SpatializerEvent &e) override;

	void timerCallback() override;

	static const String getTypeStringStatic() { return "Spatializer"; }
	static SpatializerPanel * create(const String &name = SpatializerPanel::getTypeStringStatic()) { return new SpatializerPanel(name); }
};