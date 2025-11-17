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
#include "Sequence/SequenceIncludes.h"
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
#include "model/blocks/node/NodeBlock.cpp"
#include "model/blocks/node/ui/NodeBlockUI.cpp"
#include "model/blocks/pattern/PatternBlock.cpp"
#include "model/blocks/picture/PictureBlock.cpp"
#include "model/blocks/picture/ui/PictureBlockUI.cpp"
#include "model/blocks/streamingscript/StreamingScriptBlock.cpp"
#include "model/blocks/embeddedscript/SimplexNoise.cpp"
#include "model/blocks/embeddedscript/EmbeddedScriptBlock.cpp"
#include "model/blocks/embeddedscript/WasmEngine.cpp"
#include "model/blocks/shape/PropShapeBlock.cpp"
#include "model/blocks/sequence/SequenceBlock.cpp"
#include "model/blocks/sequence/ui/SequenceBlockUI.cpp"
#include "model/blocks/video/SharedTextureBlock.cpp"
#include "model/blocks/video/VideoBlock.cpp"
#include "model/blocks/video/VideoFileBlock.cpp"
#include "model/blocks/video/ui/SharedTextureBlockUI.cpp"
#include "model/preset/LightBlockModelPreset.cpp"
#include "model/preset/LightBlockModelPresetManager.cpp"
#include "model/ui/LightBlockModelGroupUI.cpp"
#include "model/ui/LightBlockModelLibraryUI.cpp"
#include "model/ui/LightBlockModelManagerUI.cpp"
#include "model/ui/LightBlockModelParameterEditor.cpp"
#include "model/ui/LightBlockModelParameterManagerEditor.cpp"
#include "model/ui/LightBlockColorProviderEditor.cpp"

#include "model/ui/LightBlockModelUI.cpp"