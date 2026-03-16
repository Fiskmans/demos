#include "modules/chaos/ChaosModule.h"

#include <functional>

#include "SDL3/SDL.h"
#include "ChaosModule.h"


ChaosModule::ChaosModule(Engine* aEngine)
{
    myUpdateHandle = aEngine->OnUpdate.Register(std::bind(&ChaosModule::Update, this, std::placeholders::_1));
    myPaintHandle = aEngine->OnPaint.Register(std::bind(&ChaosModule::Paint, this));
}

void ChaosModule::Update(Engine::TimeDelta aDelta)
{
}

void ChaosModule::Paint()
{
}