DeclareComponent(Dummy, "dummy", )

DeclareConfigParameter(dummyParam1,0); 
DeclareConfigParameter(dummyParam2,0); 
DeclareConfigParameter(dummyParam3,0); 
// DeclareConfigParameter(dummyParam4,0); 
// DeclareConfigParameter(dummyParam5,0); 

int t1;
int t2;
int t3;
int t4;
int t5;
int t6;
int t7;
int t8;
int t9;
int t10;
int t11;
int t12;

bool initInternal(JsonObject o) override;

EndDeclareComponent

DeclareComponentSingleton(DummyManager, "dummies", )

    DeclareRangeConfigParameter(numDummies, DUMMY_MAX_COUNT, 0, DUMMY_MAX_COUNT);

DummyComponent dummies[DUMMY_MAX_COUNT];

bool initInternal(JsonObject o) override;

EndDeclareComponent


