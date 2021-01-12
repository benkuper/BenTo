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

    Point3DParameter* orientation;
    
    BoolParameter* sendRawData;

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
    BoolParameter* computeLocally;
    IntParameter* maxTrail;
    Point2DParameter* throwThresholds;
    Point2DParameter * speedThresholds;
    Point2DParameter * flatThresholds;
    FloatParameter* semiFlatThreshold;
    FloatParameter* loftieThreshold;
    FloatParameter* singleThreshold;
    FloatParameter* weightSmoothing;
    FloatParameter* weightSmoothing2;

    FloatParameter* throwWeight;
    FloatParameter* throwTime;

    int trailIndex;
    Array<Vector3D<float>> trail;
    bool inSpeed;
    Vector3D<float> smoothAccel;
    Vector3D<float> accelSpeed;
    float timeAtThrow;


    void handePropConnected() override;

    void onContainerParameterChanged(Parameter* p) override;
    void onControllableFeedbackUpdate(ControllableContainer* cc, Controllable* c) override;

    void computeAngle();
    void computeThrows();

    void update() override;
};