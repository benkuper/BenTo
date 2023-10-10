DeclareComponent(Dummy, "dummy", )

DeclareBoolParam(param1, false);
DeclareBoolParam(param2, true);
DeclareIntParam(param3, 1);
DeclareFloatParam(param4, 3.5f);
DeclareStringParam(param5, "cool");


bool initInternal(JsonObject o) override;

HandleSetParamInternalStart
    CheckAndSetParam(param1);
CheckAndSetParam(param2);
CheckAndSetParam(param3);
CheckAndSetParam(param4);
CheckAndSetParam(param5);
HandleSetParamInternalEnd;

FillSettingsInternalStart
    FillSettingsParam(param1);
    FillSettingsParam(param2);
    FillSettingsParam(param3);
    FillSettingsParam(param4);
    FillSettingsParam(param5);
FillSettingsInternalEnd

FillOSCQueryInternalStart
    FillOSCQueryBoolParam(param1);
    FillOSCQueryBoolParam(param2);
    FillOSCQueryIntParam(param3);
    FillOSCQueryFloatParam(param4);
    FillOSCQueryStringParam(param5);
FillOSCQueryInternalEnd

EndDeclareComponent



//Manager

DeclareComponentSingleton(DummyManager, "dummies", )

    DeclareRangeConfigParameter(numDummies, DUMMY_MAX_COUNT, 0, DUMMY_MAX_COUNT);

DeclareIntParam(numItems, 1);
DummyComponent items[DUMMY_MAX_COUNT];

bool initInternal(JsonObject o) override;



EndDeclareComponent


