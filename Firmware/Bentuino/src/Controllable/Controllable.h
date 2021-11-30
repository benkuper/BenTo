#pragma once

class Controllable
{
public:
    Controllable(String name) : name(name) {}
    virtual ~Controllable(){}
    
   String name;
};