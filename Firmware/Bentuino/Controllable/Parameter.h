
#pragma once

template<class T>
class Parameter :
 public Controllable
{
public:
    Parameter(String name, T val) : Controllable(name), val(val) {}
    virtual ~Parameter(){}

   T val;
   void set(T v) { val = v; }
};