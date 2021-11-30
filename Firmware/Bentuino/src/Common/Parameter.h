
#pragma once

class Parameter
{
public:
    Parameter(const String &name, var val) : name(name), val(val) {}
    virtual ~Parameter() {}

    String name;
    var val;
    void set(const var &v) { val = v; }
};