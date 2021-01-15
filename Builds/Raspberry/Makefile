# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# build with "V=1" for verbose builds
ifeq ($(V), 1)
V_AT =
else
V_AT = @
endif

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef STRIP
  STRIP=strip
endif

ifndef AR
  AR=ar
endif

ifndef CONFIG
  CONFIG=Release
endif

JUCE_ARCH_LABEL := $(shell uname -m)

ifeq ($(CONFIG),Release)
  JUCE_BINDIR := build
  JUCE_LIBDIR := build
  JUCE_OBJDIR := build/intermediate/Release
  JUCE_OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  JUCE_CPPFLAGS := $(DEPFLAGS) "-DLINUX=1" "-DNDEBUG=1" "-DJUCER_LINUX_MAKE_9C1B7400=1" "-DJUCE_APP_VERSION=1.3.0b6" "-DJUCE_APP_VERSION_HEX=0x10300" $(shell pkg-config --cflags alsa freetype2 libcurl webkit2gtk-4.0 gtk+-x11-3.0) -pthread -I../../JuceLibraryCode -I../../Modules -I$(HOME)/JUCE/modules -I../../../../../../Softwares/JUCE/modules -I../../External/serial/include -I../../External/servus/include -I../../Source $(CPPFLAGS)
  JUCE_CPPFLAGS_APP :=  "-DJucePlugin_Build_VST=0" "-DJucePlugin_Build_VST3=0" "-DJucePlugin_Build_AU=0" "-DJucePlugin_Build_AUv3=0" "-DJucePlugin_Build_RTAS=0" "-DJucePlugin_Build_AAX=0" "-DJucePlugin_Build_Standalone=0" "-DJucePlugin_Build_Unity=0"
  JUCE_TARGET_APP := BenTo

  JUCE_CFLAGS += $(JUCE_CPPFLAGS) $(TARGET_ARCH) -O3 $(CFLAGS)
  JUCE_CXXFLAGS += $(JUCE_CFLAGS) -std=c++14 $(CXXFLAGS)
  JUCE_LDFLAGS += $(TARGET_ARCH) -L$(JUCE_BINDIR) -L$(JUCE_LIBDIR) -L../../External/servus/lib/raspberry -L/usr/lib/arm-linux-gnueabihf $(shell pkg-config --libs alsa freetype2 libcurl) -fvisibility=hidden -Wl,-rpath,"lib" -Wl,--as-needed -lrt -ldl -lpthread -lGL -lcurl -lServus $(LDFLAGS)

  CLEANCMD = rm -rf $(JUCE_OUTDIR)/$(TARGET) $(JUCE_OBJDIR)
endif

OBJECTS_APP := \
  $(JUCE_OBJDIR)/IdentificationRule_c7b013cc.o \
  $(JUCE_OBJDIR)/IdentificationRuleManager_8bd0c2cb.o \
  $(JUCE_OBJDIR)/serial_82f97e54.o \
  $(JUCE_OBJDIR)/list_ports_linux_203b2613.o \
  $(JUCE_OBJDIR)/list_ports_osx_d93ade33.o \
  $(JUCE_OBJDIR)/list_ports_win_e6502c6b.o \
  $(JUCE_OBJDIR)/unix_d5f5c79d.o \
  $(JUCE_OBJDIR)/win_4c1bdbbb.o \
  $(JUCE_OBJDIR)/cobs_e77e740f.o \
  $(JUCE_OBJDIR)/SerialDevice_e404848a.o \
  $(JUCE_OBJDIR)/SerialDeviceParameter_2de97789.o \
  $(JUCE_OBJDIR)/SerialManager_fd829543.o \
  $(JUCE_OBJDIR)/ZeroconfManager_68129d03.o \
  $(JUCE_OBJDIR)/AudioManagerHardwareEditor_ce81cbaf.o \
  $(JUCE_OBJDIR)/AudioManager_d6082e39.o \
  $(JUCE_OBJDIR)/SpatializerPanel_f5827a0a.o \
  $(JUCE_OBJDIR)/SpatItemUI_cf9cb39f.o \
  $(JUCE_OBJDIR)/Spatializer_263d474f.o \
  $(JUCE_OBJDIR)/SpatItem_684aa1c0.o \
  $(JUCE_OBJDIR)/SpatLayout_5d4ec057.o \
  $(JUCE_OBJDIR)/VideoClipUI_7af85240.o \
  $(JUCE_OBJDIR)/VideoLayerTimeline_8ef6f4e0.o \
  $(JUCE_OBJDIR)/VideoClip_b6070cb9.o \
  $(JUCE_OBJDIR)/VideoLayer_9d9df52.o \
  $(JUCE_OBJDIR)/LightBlockClipManagerUI_8be8afe9.o \
  $(JUCE_OBJDIR)/LightBlockClipUI_a91aa7d6.o \
  $(JUCE_OBJDIR)/LightBlockLayerPanel_2eb25725.o \
  $(JUCE_OBJDIR)/LightBlockLayerTimeline_386b4a8a.o \
  $(JUCE_OBJDIR)/LightBlockClip_fe937171.o \
  $(JUCE_OBJDIR)/LightBlockClipManager_20fcb186.o \
  $(JUCE_OBJDIR)/LightBlockEffect_f42f5432.o \
  $(JUCE_OBJDIR)/LightBlockLayer_d2da119a.o \
  $(JUCE_OBJDIR)/TimelineBlockSequence_b60f4d48.o \
  $(JUCE_OBJDIR)/TimelineEditor_4cddf71d.o \
  $(JUCE_OBJDIR)/ColorRemapNode_4a14980f.o \
  $(JUCE_OBJDIR)/CompositeNode_f78ff15f.o \
  $(JUCE_OBJDIR)/ModelNodeUI_f4feefd6.o \
  $(JUCE_OBJDIR)/ModelNode_c9682223.o \
  $(JUCE_OBJDIR)/ParameterNodeViewUI_d2914a3c.o \
  $(JUCE_OBJDIR)/ParameterNode_ade2ed23.o \
  $(JUCE_OBJDIR)/PositionRemapNode_d665d2d1.o \
  $(JUCE_OBJDIR)/PropNode_928ace6f.o \
  $(JUCE_OBJDIR)/ColorNodeViewUI_b2913851.o \
  $(JUCE_OBJDIR)/NodeBlockEditor_59a127eb.o \
  $(JUCE_OBJDIR)/NodeBlockEditorToolbox_c6a1e852.o \
  $(JUCE_OBJDIR)/NodeConnectionManagerUI_222198d4.o \
  $(JUCE_OBJDIR)/NodeConnectionUI_4441214b.o \
  $(JUCE_OBJDIR)/NodeBlockEditorToolItem_d4105556.o \
  $(JUCE_OBJDIR)/NodeManagerUI_b8fab212.o \
  $(JUCE_OBJDIR)/NodeViewUI_fa05b6d2.o \
  $(JUCE_OBJDIR)/Node_65f22bf0.o \
  $(JUCE_OBJDIR)/ColorNode_6989fa1.o \
  $(JUCE_OBJDIR)/NodeConnection_db7eca2e.o \
  $(JUCE_OBJDIR)/NodeConnectionSlot_ebcf84cc.o \
  $(JUCE_OBJDIR)/NodeConnectionManager_e5c7a29.o \
  $(JUCE_OBJDIR)/NodeManager_772dfa27.o \
  $(JUCE_OBJDIR)/BlockVizPanel_f0c2e5be.o \
  $(JUCE_OBJDIR)/LightBlockModelGroup_e729d26c.o \
  $(JUCE_OBJDIR)/LightBlockColorProvider_fc2799a8.o \
  $(JUCE_OBJDIR)/LightBlockModelPreset_24526780.o \
  $(JUCE_OBJDIR)/LightBlockModelPresetManager_20ce9c97.o \
  $(JUCE_OBJDIR)/LightBlockModelGroupUI_241748c7.o \
  $(JUCE_OBJDIR)/LightBlockModelLibraryUI_b2ea4643.o \
  $(JUCE_OBJDIR)/LightBlockModelManagerUI_e697b3b5.o \
  $(JUCE_OBJDIR)/LightBlockModelParameterEditor_fa2e916a.o \
  $(JUCE_OBJDIR)/LightBlockModelParameterManagerEditor_47e83067.o \
  $(JUCE_OBJDIR)/LightBlockModelUI_9ea7118a.o \
  $(JUCE_OBJDIR)/PropShapeBlock_19846211.o \
  $(JUCE_OBJDIR)/LightBlockFilter_b3294d37.o \
  $(JUCE_OBJDIR)/ScriptBlockFilter_2a4ec35e.o \
  $(JUCE_OBJDIR)/BrightnessContrastBlockFilter_2ee8e776.o \
  $(JUCE_OBJDIR)/HSVBlockFilter_d854981e.o \
  $(JUCE_OBJDIR)/RemapBlockFilter_fcc569a2.o \
  $(JUCE_OBJDIR)/PictureBlockUI_8a35e63.o \
  $(JUCE_OBJDIR)/PictureBlock_4a597ad4.o \
  $(JUCE_OBJDIR)/DMXBlock_ca1094f2.o \
  $(JUCE_OBJDIR)/VideoBlockUI_ef7208c3.o \
  $(JUCE_OBJDIR)/VideoBlock_5117b69a.o \
  $(JUCE_OBJDIR)/PatternBlock_a30b5830.o \
  $(JUCE_OBJDIR)/TimelineBlockUI_53d3cb49.o \
  $(JUCE_OBJDIR)/TimelineBlock_f311c7b2.o \
  $(JUCE_OBJDIR)/NodeBlockUI_b4f22607.o \
  $(JUCE_OBJDIR)/NodeBlock_220d3312.o \
  $(JUCE_OBJDIR)/ScriptBlock_dea52f2.o \
  $(JUCE_OBJDIR)/LightBlockModel_c089add.o \
  $(JUCE_OBJDIR)/LightBlockModelLibrary_b26834a8.o \
  $(JUCE_OBJDIR)/UserLightBlockModelManager_852221c5.o \
  $(JUCE_OBJDIR)/LightBlockModelParameter_4dedce96.o \
  $(JUCE_OBJDIR)/LightBlockModelParameterManager_893b30c1.o \
  $(JUCE_OBJDIR)/LightBlock_43a72d10.o \
  $(JUCE_OBJDIR)/VideoPreviewPanel_e2748d5e.o \
  $(JUCE_OBJDIR)/PropShapeCurveEasingUI_6031bbfa.o \
  $(JUCE_OBJDIR)/PropShapePanel_5cefd2f0.o \
  $(JUCE_OBJDIR)/PropShapeView_3c8b67c3.o \
  $(JUCE_OBJDIR)/PropShape_99b69cad.o \
  $(JUCE_OBJDIR)/PropShapeCurve_5462c50c.o \
  $(JUCE_OBJDIR)/PropShapeCurveEasing_fa24f397.o \
  $(JUCE_OBJDIR)/PropShapeGroup_ce04729c.o \
  $(JUCE_OBJDIR)/PropFamily_1b08e5d4.o \
  $(JUCE_OBJDIR)/PropTargetFilterManager_c46f408.o \
  $(JUCE_OBJDIR)/PropTargetFilter_a9a21d2f.o \
  $(JUCE_OBJDIR)/PropClusterGroupManagerUI_53e41580.o \
  $(JUCE_OBJDIR)/PropCluster_70507f1f.o \
  $(JUCE_OBJDIR)/PropClusterGroup_98d0476a.o \
  $(JUCE_OBJDIR)/PropClusterGroupManager_2f9ae16d.o \
  $(JUCE_OBJDIR)/BentoProp_c5777cde.o \
  $(JUCE_OBJDIR)/IRPropComponent_fb70abca.o \
  $(JUCE_OBJDIR)/BatteryComponent_e5130d1.o \
  $(JUCE_OBJDIR)/ButtonsComponent_3f2f3ae9.o \
  $(JUCE_OBJDIR)/FilesComponent_11c51d7d.o \
  $(JUCE_OBJDIR)/IMUComponent_3e444b49.o \
  $(JUCE_OBJDIR)/RGBComponent_9a91a51.o \
  $(JUCE_OBJDIR)/PropComponent_28bd02e5.o \
  $(JUCE_OBJDIR)/PropEditor_5217fd88.o \
  $(JUCE_OBJDIR)/PropManagerUI_843f2390.o \
  $(JUCE_OBJDIR)/PropUI_e53ff20f.o \
  $(JUCE_OBJDIR)/PropViz_32565136.o \
  $(JUCE_OBJDIR)/Prop_c0de1090.o \
  $(JUCE_OBJDIR)/PropManager_8695a187.o \
  $(JUCE_OBJDIR)/BentoSettings_c196016a.o \
  $(JUCE_OBJDIR)/BentoEngine_f46ef489.o \
  $(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o \
  $(JUCE_OBJDIR)/BentoComponentCommands_28169f08.o \
  $(JUCE_OBJDIR)/Main_90ebc5c2.o \
  $(JUCE_OBJDIR)/BinaryData_ce4232d4.o \
  $(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoComponent_c6ded2e4.o \
  $(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoReader_94a84586.o \
  $(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoWriter_6efe3936.o \
  $(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o \
  $(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o \
  $(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o \
  $(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o \
  $(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o \
  $(JUCE_OBJDIR)/include_juce_core_f26d17db.o \
  $(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o \
  $(JUCE_OBJDIR)/include_juce_events_fd7d695.o \
  $(JUCE_OBJDIR)/include_juce_graphics_f817e147.o \
  $(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o \
  $(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o \
  $(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o \
  $(JUCE_OBJDIR)/include_juce_organicui_2c36794d.o \
  $(JUCE_OBJDIR)/include_juce_organicui2_5ab8c64f.o \
  $(JUCE_OBJDIR)/include_juce_organicui3_5ac6ddd0.o \
  $(JUCE_OBJDIR)/include_juce_organicui4_5ad4f551.o \
  $(JUCE_OBJDIR)/include_juce_osc_f3df604d.o \
  $(JUCE_OBJDIR)/include_juce_sharedtexture_e491fa4.o \
  $(JUCE_OBJDIR)/include_juce_timeline_e6183ffd.o \
  $(JUCE_OBJDIR)/include_juce_video_be78589.o \

.PHONY: clean all strip

all : $(JUCE_OUTDIR)/$(JUCE_TARGET_APP)

$(JUCE_OUTDIR)/$(JUCE_TARGET_APP) : $(OBJECTS_APP) $(RESOURCES)
	@command -v pkg-config >/dev/null 2>&1 || { echo >&2 "pkg-config not installed. Please, install it."; exit 1; }
	@pkg-config --print-errors alsa freetype2 libcurl
	@echo Linking "BenTo - App"
	-$(V_AT)mkdir -p $(JUCE_BINDIR)
	-$(V_AT)mkdir -p $(JUCE_LIBDIR)
	-$(V_AT)mkdir -p $(JUCE_OUTDIR)
	$(V_AT)$(CXX) -o $(JUCE_OUTDIR)/$(JUCE_TARGET_APP) $(OBJECTS_APP) $(JUCE_LDFLAGS) $(JUCE_LDFLAGS_APP) $(RESOURCES) $(TARGET_ARCH)

$(JUCE_OBJDIR)/IdentificationRule_c7b013cc.o: ../../Source/Identification/IdentificationRule.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IdentificationRule.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/IdentificationRuleManager_8bd0c2cb.o: ../../Source/Identification/IdentificationRuleManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IdentificationRuleManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/serial_82f97e54.o: ../../Source/Common/Serial/lib/serial/serial.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling serial.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_linux_203b2613.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_linux.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling list_ports_linux.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_osx_d93ade33.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_osx.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling list_ports_osx.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/list_ports_win_e6502c6b.o: ../../Source/Common/Serial/lib/serial/impl/list_ports/list_ports_win.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling list_ports_win.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/unix_d5f5c79d.o: ../../Source/Common/Serial/lib/serial/impl/unix.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling unix.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/win_4c1bdbbb.o: ../../Source/Common/Serial/lib/serial/impl/win.cc
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling win.cc"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/cobs_e77e740f.o: ../../Source/Common/Serial/lib/cobs/cobs.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling cobs.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SerialDevice_e404848a.o: ../../Source/Common/Serial/SerialDevice.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SerialDevice.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SerialDeviceParameter_2de97789.o: ../../Source/Common/Serial/SerialDeviceParameter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SerialDeviceParameter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SerialManager_fd829543.o: ../../Source/Common/Serial/SerialManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SerialManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ZeroconfManager_68129d03.o: ../../Source/Common/Zeroconf/ZeroconfManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ZeroconfManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioManagerHardwareEditor_ce81cbaf.o: ../../Source/Audio/ui/AudioManagerHardwareEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioManagerHardwareEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/AudioManager_d6082e39.o: ../../Source/Audio/AudioManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling AudioManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatializerPanel_f5827a0a.o: ../../Source/Video/ui/SpatializerPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpatializerPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatItemUI_cf9cb39f.o: ../../Source/Video/ui/SpatItemUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpatItemUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Spatializer_263d474f.o: ../../Source/Video/Spatializer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Spatializer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatItem_684aa1c0.o: ../../Source/Video/SpatItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpatItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/SpatLayout_5d4ec057.o: ../../Source/Video/SpatLayout.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling SpatLayout.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoClipUI_7af85240.o: ../../Source/Timeline/layers/Video/ui/VideoClipUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoClipUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoLayerTimeline_8ef6f4e0.o: ../../Source/Timeline/layers/Video/ui/VideoLayerTimeline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoLayerTimeline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoClip_b6070cb9.o: ../../Source/Timeline/layers/Video/VideoClip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoClip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoLayer_9d9df52.o: ../../Source/Timeline/layers/Video/VideoLayer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockClipManagerUI_8be8afe9.o: ../../Source/Timeline/layers/Block/ui/LightBlockClipManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockClipManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockClipUI_a91aa7d6.o: ../../Source/Timeline/layers/Block/ui/LightBlockClipUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockClipUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockLayerPanel_2eb25725.o: ../../Source/Timeline/layers/Block/ui/LightBlockLayerPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockLayerPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockLayerTimeline_386b4a8a.o: ../../Source/Timeline/layers/Block/ui/LightBlockLayerTimeline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockLayerTimeline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockClip_fe937171.o: ../../Source/Timeline/layers/Block/LightBlockClip.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockClip.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockClipManager_20fcb186.o: ../../Source/Timeline/layers/Block/LightBlockClipManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockClipManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockEffect_f42f5432.o: ../../Source/Timeline/layers/Block/LightBlockEffect.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockEffect.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockLayer_d2da119a.o: ../../Source/Timeline/layers/Block/LightBlockLayer.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockLayer.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineBlockSequence_b60f4d48.o: ../../Source/Timeline/TimelineBlockSequence.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineBlockSequence.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineEditor_4cddf71d.o: ../../Source/Timeline/TimelineEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColorRemapNode_4a14980f.o: ../../Source/Node/nodes/colorRemap/ColorRemapNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColorRemapNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/CompositeNode_f78ff15f.o: ../../Source/Node/nodes/composite/CompositeNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling CompositeNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModelNodeUI_f4feefd6.o: ../../Source/Node/nodes/model/ui/ModelNodeUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModelNodeUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ModelNode_c9682223.o: ../../Source/Node/nodes/model/ModelNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ModelNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ParameterNodeViewUI_d2914a3c.o: ../../Source/Node/nodes/parameter/ParameterNodeViewUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ParameterNodeViewUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ParameterNode_ade2ed23.o: ../../Source/Node/nodes/parameter/ParameterNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ParameterNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PositionRemapNode_d665d2d1.o: ../../Source/Node/nodes/positionRemap/PositionRemapNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PositionRemapNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropNode_928ace6f.o: ../../Source/Node/nodes/prop/PropNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColorNodeViewUI_b2913851.o: ../../Source/Node/ui/ColorNodeViewUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColorNodeViewUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeBlockEditor_59a127eb.o: ../../Source/Node/ui/NodeBlockEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeBlockEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeBlockEditorToolbox_c6a1e852.o: ../../Source/Node/ui/NodeBlockEditorToolbox.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeBlockEditorToolbox.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeConnectionManagerUI_222198d4.o: ../../Source/Node/ui/NodeConnectionManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeConnectionManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeConnectionUI_4441214b.o: ../../Source/Node/ui/NodeConnectionUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeConnectionUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeBlockEditorToolItem_d4105556.o: ../../Source/Node/ui/NodeBlockEditorToolItem.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeBlockEditorToolItem.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeManagerUI_b8fab212.o: ../../Source/Node/ui/NodeManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeViewUI_fa05b6d2.o: ../../Source/Node/ui/NodeViewUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeViewUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Node_65f22bf0.o: ../../Source/Node/Node.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Node.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ColorNode_6989fa1.o: ../../Source/Node/ColorNode.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ColorNode.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeConnection_db7eca2e.o: ../../Source/Node/NodeConnection.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeConnection.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeConnectionSlot_ebcf84cc.o: ../../Source/Node/NodeConnectionSlot.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeConnectionSlot.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeConnectionManager_e5c7a29.o: ../../Source/Node/NodeConnectionManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeConnectionManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeManager_772dfa27.o: ../../Source/Node/NodeManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BlockVizPanel_f0c2e5be.o: ../../Source/BlockViz/BlockVizPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BlockVizPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelGroup_e729d26c.o: ../../Source/LightBlock/model/LightBlockModelGroup.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelGroup.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockColorProvider_fc2799a8.o: ../../Source/LightBlock/model/LightBlockColorProvider.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockColorProvider.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelPreset_24526780.o: ../../Source/LightBlock/model/preset/LightBlockModelPreset.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelPreset.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelPresetManager_20ce9c97.o: ../../Source/LightBlock/model/preset/LightBlockModelPresetManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelPresetManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelGroupUI_241748c7.o: ../../Source/LightBlock/model/ui/LightBlockModelGroupUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelGroupUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelLibraryUI_b2ea4643.o: ../../Source/LightBlock/model/ui/LightBlockModelLibraryUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelLibraryUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelManagerUI_e697b3b5.o: ../../Source/LightBlock/model/ui/LightBlockModelManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelParameterEditor_fa2e916a.o: ../../Source/LightBlock/model/ui/LightBlockModelParameterEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelParameterEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelParameterManagerEditor_47e83067.o: ../../Source/LightBlock/model/ui/LightBlockModelParameterManagerEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelParameterManagerEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelUI_9ea7118a.o: ../../Source/LightBlock/model/ui/LightBlockModelUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeBlock_19846211.o: ../../Source/LightBlock/model/blocks/shape/PropShapeBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockFilter_b3294d37.o: ../../Source/LightBlock/model/blocks/filters/LightBlockFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScriptBlockFilter_2a4ec35e.o: ../../Source/LightBlock/model/blocks/filters/script/ScriptBlockFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScriptBlockFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BrightnessContrastBlockFilter_2ee8e776.o: ../../Source/LightBlock/model/blocks/filters/generic/brightnesscontrast/BrightnessContrastBlockFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BrightnessContrastBlockFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/HSVBlockFilter_d854981e.o: ../../Source/LightBlock/model/blocks/filters/generic/hsv/HSVBlockFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling HSVBlockFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RemapBlockFilter_fcc569a2.o: ../../Source/LightBlock/model/blocks/filters/generic/remap/RemapBlockFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RemapBlockFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PictureBlockUI_8a35e63.o: ../../Source/LightBlock/model/blocks/picture/ui/PictureBlockUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PictureBlockUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PictureBlock_4a597ad4.o: ../../Source/LightBlock/model/blocks/picture/PictureBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PictureBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/DMXBlock_ca1094f2.o: ../../Source/LightBlock/model/blocks/dmx/DMXBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling DMXBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoBlockUI_ef7208c3.o: ../../Source/LightBlock/model/blocks/video/ui/VideoBlockUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoBlockUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoBlock_5117b69a.o: ../../Source/LightBlock/model/blocks/video/VideoBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PatternBlock_a30b5830.o: ../../Source/LightBlock/model/blocks/pattern/PatternBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PatternBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineBlockUI_53d3cb49.o: ../../Source/LightBlock/model/blocks/timeline/ui/TimelineBlockUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineBlockUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/TimelineBlock_f311c7b2.o: ../../Source/LightBlock/model/blocks/timeline/TimelineBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling TimelineBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeBlockUI_b4f22607.o: ../../Source/LightBlock/model/blocks/node/ui/NodeBlockUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeBlockUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/NodeBlock_220d3312.o: ../../Source/LightBlock/model/blocks/node/NodeBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling NodeBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ScriptBlock_dea52f2.o: ../../Source/LightBlock/model/blocks/script/ScriptBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ScriptBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModel_c089add.o: ../../Source/LightBlock/model/LightBlockModel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelLibrary_b26834a8.o: ../../Source/LightBlock/model/LightBlockModelLibrary.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelLibrary.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/UserLightBlockModelManager_852221c5.o: ../../Source/LightBlock/model/UserLightBlockModelManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling UserLightBlockModelManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelParameter_4dedce96.o: ../../Source/LightBlock/model/LightBlockModelParameter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelParameter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlockModelParameterManager_893b30c1.o: ../../Source/LightBlock/model/LightBlockModelParameterManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlockModelParameterManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/LightBlock_43a72d10.o: ../../Source/LightBlock/LightBlock.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling LightBlock.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/VideoPreviewPanel_e2748d5e.o: ../../Source/VideoPreview/VideoPreviewPanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling VideoPreviewPanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeCurveEasingUI_6031bbfa.o: ../../Source/Prop/Shape/ui/PropShapeCurveEasingUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeCurveEasingUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapePanel_5cefd2f0.o: ../../Source/Prop/Shape/ui/PropShapePanel.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapePanel.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeView_3c8b67c3.o: ../../Source/Prop/Shape/ui/PropShapeView.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeView.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShape_99b69cad.o: ../../Source/Prop/Shape/PropShape.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShape.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeCurve_5462c50c.o: ../../Source/Prop/Shape/PropShapeCurve.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeCurve.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeCurveEasing_fa24f397.o: ../../Source/Prop/Shape/PropShapeCurveEasing.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeCurveEasing.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropShapeGroup_ce04729c.o: ../../Source/Prop/Shape/PropShapeGroup.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropShapeGroup.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropFamily_1b08e5d4.o: ../../Source/Prop/PropFamily.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropFamily.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropTargetFilterManager_c46f408.o: ../../Source/Prop/TargetFilter/PropTargetFilterManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropTargetFilterManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropTargetFilter_a9a21d2f.o: ../../Source/Prop/TargetFilter/PropTargetFilter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropTargetFilter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropClusterGroupManagerUI_53e41580.o: ../../Source/Prop/Cluster/ui/PropClusterGroupManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropClusterGroupManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropCluster_70507f1f.o: ../../Source/Prop/Cluster/PropCluster.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropCluster.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropClusterGroup_98d0476a.o: ../../Source/Prop/Cluster/PropClusterGroup.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropClusterGroup.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropClusterGroupManager_2f9ae16d.o: ../../Source/Prop/Cluster/PropClusterGroupManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropClusterGroupManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BentoProp_c5777cde.o: ../../Source/Prop/props/bento/BentoProp.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BentoProp.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/IRPropComponent_fb70abca.o: ../../Source/Prop/Component/ir/IRPropComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IRPropComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BatteryComponent_e5130d1.o: ../../Source/Prop/Component/battery/BatteryComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BatteryComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/ButtonsComponent_3f2f3ae9.o: ../../Source/Prop/Component/buttons/ButtonsComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling ButtonsComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/FilesComponent_11c51d7d.o: ../../Source/Prop/Component/files/FilesComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling FilesComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/IMUComponent_3e444b49.o: ../../Source/Prop/Component/imu/IMUComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling IMUComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/RGBComponent_9a91a51.o: ../../Source/Prop/Component/rgb/RGBComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling RGBComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropComponent_28bd02e5.o: ../../Source/Prop/Component/PropComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropEditor_5217fd88.o: ../../Source/Prop/ui/PropEditor.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropEditor.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropManagerUI_843f2390.o: ../../Source/Prop/ui/PropManagerUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropManagerUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropUI_e53ff20f.o: ../../Source/Prop/ui/PropUI.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropUI.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropViz_32565136.o: ../../Source/Prop/ui/PropViz.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropViz.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Prop_c0de1090.o: ../../Source/Prop/Prop.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Prop.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/PropManager_8695a187.o: ../../Source/Prop/PropManager.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling PropManager.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BentoSettings_c196016a.o: ../../Source/BentoSettings.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BentoSettings.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BentoEngine_f46ef489.o: ../../Source/BentoEngine.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BentoEngine.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling MainComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BentoComponentCommands_28169f08.o: ../../Source/BentoComponentCommands.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BentoComponentCommands.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling Main.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling BinaryData.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoComponent_c6ded2e4.o: ../../JuceLibraryCode/include_filmstro_ffmpeg_FFmpegVideoComponent.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_filmstro_ffmpeg_FFmpegVideoComponent.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoReader_94a84586.o: ../../JuceLibraryCode/include_filmstro_ffmpeg_FFmpegVideoReader.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_filmstro_ffmpeg_FFmpegVideoReader.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_filmstro_ffmpeg_FFmpegVideoWriter_6efe3936.o: ../../JuceLibraryCode/include_filmstro_ffmpeg_FFmpegVideoWriter.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_filmstro_ffmpeg_FFmpegVideoWriter.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_basics_8a4e984a.o: ../../JuceLibraryCode/include_juce_audio_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_devices_63111d02.o: ../../JuceLibraryCode/include_juce_audio_devices.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_devices.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_formats_15f82001.o: ../../JuceLibraryCode/include_juce_audio_formats.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_formats.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_processors_10c03666.o: ../../JuceLibraryCode/include_juce_audio_processors.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_processors.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_audio_utils_9f9fb2d6.o: ../../JuceLibraryCode/include_juce_audio_utils.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_audio_utils.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_core_f26d17db.o: ../../JuceLibraryCode/include_juce_core.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_core.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_data_structures_7471b1e3.o: ../../JuceLibraryCode/include_juce_data_structures.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_data_structures.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_events_fd7d695.o: ../../JuceLibraryCode/include_juce_events.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_events.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_graphics_f817e147.o: ../../JuceLibraryCode/include_juce_graphics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_graphics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_basics_e3f79785.o: ../../JuceLibraryCode/include_juce_gui_basics.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_basics.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_gui_extra_6dee1c1a.o: ../../JuceLibraryCode/include_juce_gui_extra.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_gui_extra.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_opengl_a8a032b.o: ../../JuceLibraryCode/include_juce_opengl.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_opengl.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui_2c36794d.o: ../../JuceLibraryCode/include_juce_organicui.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_organicui.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui2_5ab8c64f.o: ../../JuceLibraryCode/include_juce_organicui2.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_organicui2.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui3_5ac6ddd0.o: ../../JuceLibraryCode/include_juce_organicui3.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_organicui3.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_organicui4_5ad4f551.o: ../../JuceLibraryCode/include_juce_organicui4.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_organicui4.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_osc_f3df604d.o: ../../JuceLibraryCode/include_juce_osc.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_osc.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_sharedtexture_e491fa4.o: ../../JuceLibraryCode/include_juce_sharedtexture.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_sharedtexture.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_timeline_e6183ffd.o: ../../JuceLibraryCode/include_juce_timeline.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_timeline.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

$(JUCE_OBJDIR)/include_juce_video_be78589.o: ../../JuceLibraryCode/include_juce_video.cpp
	-$(V_AT)mkdir -p $(JUCE_OBJDIR)
	@echo "Compiling include_juce_video.cpp"
	$(V_AT)$(CXX) $(JUCE_CXXFLAGS) $(JUCE_CPPFLAGS_APP) $(JUCE_CFLAGS_APP) -o "$@" -c "$<"

clean:
	@echo Cleaning BenTo
	$(V_AT)$(CLEANCMD)

strip:
	@echo Stripping BenTo
	-$(V_AT)$(STRIP) --strip-unneeded $(JUCE_OUTDIR)/$(TARGET)

-include $(OBJECTS_APP:%.o=%.d)
