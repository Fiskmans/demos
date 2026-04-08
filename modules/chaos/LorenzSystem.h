#pragma once

#include "modules/chaos/ChaoticPath.h"
#include "imgui.h"
#include <functional>

class LorenzSystem : public ChaoticPath
{
public:
    static constexpr float StepSize = 0.01f;

    LorenzSystem(float s, float p, float b);

    void Update() override;
    void Imgui() override;
	std::vector<Vec3>& GetPath() override
	{
		return myPoints;
	}

private:

    void Plot(const char* aLabel, std::function<ImVec2(Vec3)> aReduction, ImVec2 aSize = {0,0});

    int myTailLength;
    float s;
    float p;
    float b;

    Vec3 myMax;
    Vec3 myMin;

    Vec3 myAt;
	std::vector<Vec3> myPoints;
};