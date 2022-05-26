/*
  ==============================================================================

	VideoLayerTimeline.cpp
	Created: 4 Jan 2021 11:07:35pm
	Author:  bkupe

  ==============================================================================
*/

VideoLayerTimeline::VideoLayerTimeline(VideoLayer* layer) :
	SequenceLayerTimeline(layer),
	videoLayer(layer)
{
	bgColor = AUDIO_COLOR.withSaturation(.2f).darker(1);

	cmMUI.reset(new VideoLayerClipManagerUI(this, &layer->clipManager));
	addAndMakeVisible(cmMUI.get());

	updateMiniModeUI();

	needle.toFront(false);
	updateContent();
}

VideoLayerTimeline::~VideoLayerTimeline()
{

}

void VideoLayerTimeline::resized()
{
	cmMUI->setBounds(getLocalBounds());
}

void VideoLayerTimeline::updateContent()
{
	cmMUI->updateContent();
}

void VideoLayerTimeline::updateMiniModeUI()
{
	cmMUI->setMiniMode(item->miniMode->boolValue());
}

void VideoLayerTimeline::addSelectableComponentsAndInspectables(Array<Component*>& selectables, Array<Inspectable*>& inspectables)
{
	if (cmMUI == nullptr) return;
	cmMUI->addSelectableComponentsAndInspectables(selectables, inspectables);
}


VideoLayerClipManagerUI::VideoLayerClipManagerUI(VideoLayerTimeline* _timeline, AudioLayerClipManager* manager) :
	LayerBlockManagerUI(_timeline, manager)
{
	addExistingItems();

}

VideoLayerClipManagerUI::~VideoLayerClipManagerUI()
{
}

LayerBlockUI* VideoLayerClipManagerUI::createUIForItem(LayerBlock* item)
{
	VideoClipUI* ui = new VideoClipUI((VideoClip*)item);
	ui->setupThumbnail();
	return ui;
}

void VideoLayerClipManagerUI::mouseDoubleClick(const MouseEvent& e)
{
	addClipWithFileChooserAt(getMouseXYRelative().x);
}

void VideoLayerClipManagerUI::addClipWithFileChooserAt(float position)
{
	FileChooser chooser("Load  video file", File(), "*.mp4;*.mov;*.avi;*.mpeg");
	FileChooser* fc(new FileChooser("Export a block"));
	fc->launchAsync(FileBrowserComponent::FileChooserFlags::openMode, [this, position](const FileChooser& fc)
		{
			File f = fc.getResult();
			delete& fc;
			if (f == File()) return;

			float time = timeline->getTimeForX(position);
			VideoClip* clip = dynamic_cast<VideoClip*>(manager->addBlockAt(time));
			clip->filePath->setValue(f.getFullPathName());
		}
	);
}