#pragma once

#include "Win32Builder.h"
#include "Global.h"

class Base
{
public:
    virtual void Start() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    virtual void Resize(uint32 width, uint32 height) = 0;

    HWND hwnd;

};

