/*
  ==============================================================================

    LightBlockIncludes.cpp
    Created: 20 May 2021 8:59:15am
    Author:  bkupe

  ==============================================================================
*/

#include "LightBlockIncludes.h"

#include "Node/NodeIncludes.h"
#include "Prop/PropIncludes.h"
#include "Timeline/TimelineIncludes.h"
#include "Video/VideoIncludes.h"

#include "LightBlock.cpp"
#include "model/LightBlockColorProvider.cpp"
#include "model/LightBlockModel.cpp"
#include "model/LightBlockModelGroup.cpp"
#include "model/LightBlockModelLibrary.cpp"
#include "model/LightBlockModelParameter.cpp"
#include "model/LightBlockModelParameterManager.cpp"
#include "model/UserLightBlockModelManager.cpp"
#include "model/blocks/dmx/DMXBlock.cpp"
#include "model/blocks/filters/LightBlockFilter.cpp"
#include "model/blocks/filters/generic/brightnesscontrast/BrightnessContrastBlockFilter.cpp"
#include "model/blocks/filters/generic/hsv/HSVBlockFilter.cpp"
#include "model/blocks/filters/generic/remap/RemapBlockFilter.cpp"
#include "model/blocks/filters/script/ScriptBlockFilter.cpp"
#include "model/blocks/node/NodeBlock.cpp"
#include "model/blocks/node/ui/NodeBlockUI.cpp"
#include "model/blocks/pattern/PatternBlock.cpp"
#include "model/blocks/picture/PictureBlock.cpp"
#include "model/blocks/picture/ui/PictureBlockUI.cpp"
#include "model/blocks/script/ScriptBlock.cpp"
#include "model/blocks/shape/PropShapeBlock.cpp"
#include "model/blocks/timeline/TimelineBlock.cpp"
#include "model/blocks/timeline/ui/TimelineBlockUI.cpp"
#include "model/blocks/video/LiveFeedBlock.cpp"
#include "model/blocks/video/TextureBlock.cpp"
#include "model/blocks/video/VideoBlock.cpp"
#include "model/blocks/video/ui/LiveFeedBlockUI.cpp"
#include "model/preset/LightBlockModelPreset.cpp"
#include "model/preset/LightBlockModelPresetManager.cpp"
#include "model/ui/LightBlockModelGroupUI.cpp"
#include "model/ui/LightBlockModelLibraryUI.cpp"
#include "model/ui/LightBlockModelManagerUI.cpp"
#include "model/ui/LightBlockModelParameterEditor.cpp"
#include "model/ui/LightBlockModelParameterManagerEditor.cpp"
#include "model/ui/LightBlockModelUI.cpp"