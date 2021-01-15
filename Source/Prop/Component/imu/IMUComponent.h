/*
  ==============================================================================

    IMUComponent.h
    Created: 8 May 2020 3:08:55pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../PropComponent.h"

class IMUPropComponent :
    public PropComponent
{
public:
    IMUPropComponent(Prop* prop, var params);
    ~IMUPropComponent();

    IntParameter* updateRate;

    Point3DParameter* orientation;
    
    EnumParameter * sendLevel;

    Point3DParameter* accel;
    Point3DParameter* linearAccel;
    Point3DParameter* gyro;
    //Point3DParameter* gravity;


    FloatParameter* projectedAngle;
    EnumParameter* throwState;

    ControllableContainer angleConfigCC;
    Trigger* calibrate;
    FloatParameter* offset;
    FloatParameter* angleOffset;
    BoolParameter* invert;

    ControllableContainer throwConfigCC;
    Point2DParameter* flatThresholds;
    Point2DParameter* accelThresholds;
    FloatParameter  * diffThreshold;
    FloatParameter* semiFlatThreshold;
    FloatParameter* loftieThreshold;
    FloatParameter* singleThreshold;
    FloatParameter* weightSmoothing;
    FloatParameter* weightSmoothing2;

    FloatParameter* flatWeight;
    FloatParameter* throwWeight;
    FloatParameter* throwTime;

    float timeAtThrow;


    void handePropConnected() override;

    void onContainerParameterChanged(Parameter* p) override;
    void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

    void computeAngle();

    void update() override;
};