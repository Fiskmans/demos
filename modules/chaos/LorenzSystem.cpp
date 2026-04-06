#include "LorenzSystem.h"

#include <random>

LorenzSystem::LorenzSystem(float s, float p, float b)
{
    this->s = s;
    this->p = p;
    this->b = b;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution dist(-10.f, 10.f);

    myAt.x = dist(rng);
    myAt.y = dist(rng);
    myAt.z = dist(rng);

    myMax = myAt;
    myMin = myAt;
}

void LorenzSystem::Update()
{
    Vec3 delta {
        s * (myAt.y - myAt.x),
        myAt.x * (p - myAt.z) - myAt.y,
        myAt.x * myAt.y - b * myAt.z
    };

    myAt += delta * StepSize;
    myPoints.push_back(myAt);

    myMax.x = std::max(myAt.x, myMax.x);
    myMax.y = std::max(myAt.y, myMax.y);
    myMax.z = std::max(myAt.z, myMax.z);

    myMin.x = std::min(myAt.x, myMin.x);
    myMin.y = std::min(myAt.y, myMin.y);
    myMin.z = std::min(myAt.z, myMin.z);
}

void LorenzSystem::Imgui()
{
    myAt.ImGui();
    Plot("xy", [](Vec3 v) -> ImVec2 { return {v.x, v.y};}, {300,300});
    ImGui::SameLine();
    Plot("yz", [](Vec3 v) -> ImVec2 { return {v.y, v.z};}, {300,300});
    ImGui::SameLine();
    Plot("xz", [](Vec3 v) -> ImVec2 { return {v.x, v.z};}, {300,300});
}

void LorenzSystem::Plot(const char* aLabel, std::function<ImVec2(Vec3)> aReduction, ImVec2 aSize)
{
    if (ImGui::BeginChild(aLabel, aSize))
    {
        ImGui::TextUnformatted(aLabel);
        ImVec2 base = ImGui::GetCursorScreenPos();
        ImGui::Dummy(aSize);

        if (myPoints.size() > 0)
        {
            ImDrawList* dl = ImGui::GetWindowDrawList();

            Vec3 scale = myMax - myMin;

            std::vector<ImVec2> points;

            for (Vec3 v : myPoints)
            {
                ImVec2 p = aReduction((v - myMin) / scale);

                points.push_back({p.x * aSize.x + base.x, p.y * aSize.y + base.y});
            }

            dl->AddPolyline(points.data(), points.size(), ImColor(255,255,255), 0, 1);
        }
    }

    ImGui::EndChild();
    
}
