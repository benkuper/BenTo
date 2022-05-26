/*
  ==============================================================================

    LightBlockIncludes.h
    Created: 20 May 2021 8:59:15am
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "Node/NodeIncludes.h"
#include "Video/VideoIncludes.h"

#include "model/LightBlockModelParameter.h"
#include "model/LightBlockModelParameterManager.h"

#include "model/LightBlockColorProvider.h"

#include "model/preset/LightBlockModelPreset.h"
#include "model/preset/LightBlockModelPresetManager.h"

#include "model/LightBlockModel.h"

#include "LightBlock.h"

#include "model/LightBlockModelGroup.h"
#include "model/UserLightBlockModelManager.h"

#include "model/blocks/filters/LightBlockFilter.h"
#include "model/blocks/filters/generic/brightnesscontrast/BrightnessContrastBlockFilter.h"
#include "model/blocks/filters/generic/hsv/HSVBlockFilter.h"
#include "model/blocks/filters/generic/remap/RemapBlockFilter.h"
#include "model/blocks/filters/script/ScriptBlockFilter.h"

#include "model/blocks/dmx/DMXBlock.h"

#include "model/blocks/node/NodeBlock.h"

#include "model/blocks/pattern/PatternBlock.h"

#include "model/blocks/picture/PictureBlock.h"

#include "model/blocks/script/ScriptBlock.h"
#include "model/blocks/wasm/WasmBlock.h"

#include "model/blocks/shape/PropShapeBlock.h"

#include "model/blocks/timeline/TimelineBlock.h"

#include "model/blocks/video/TextureBlock.h"
#include "model/blocks/video/LiveFeedBlock.h"
#include "model/blocks/video/VideoBlock.h"

#include "model/LightBlockModelLibrary.h"

#include "model/ui/LightBlockModelUI.h"
#include "model/ui/LightBlockModelManagerUI.h"

#include "model/blocks/video/ui/LiveFeedBlockUI.h"
#include "model/blocks/timeline/ui/TimelineBlockUI.h"
#include "model/blocks/node/ui/NodeBlockUI.h"
#include "model/blocks/picture/ui/PictureBlockUI.h"

#include "model/ui/LightBlockModelParameterEditor.h"
#include "model/ui/LightBlockModelParameterManagerEditor.h"






#include "model/ui/LightBlockModelGroupUI.h"
#include "model/ui/LightBlockModelLibraryUI.h"

#include "ui/LightBlockManagerUI.h"