#pragma once

#include "engine/Engine.h"
#include "tools/Event.h"

class ChaosModule
{
public:
    ChaosModule(Engine* aEngine);
    ~ChaosModule() = default;

    void Update(Engine::TimeDelta aDelta);
    void Paint();
private:
    fisk::tools::EventReg myUpdateHandle;
    fisk::tools::EventReg myPaintHandle;
};