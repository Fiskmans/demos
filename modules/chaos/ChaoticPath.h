
#pragma once

#include <vector>

#include "imgui.h"

struct Vec3
{
	float x;
	float y;
	float z;

	Vec3& operator+=(Vec3 aOther)
	{
		x += aOther.x;
		y += aOther.y;
		z += aOther.z;

        return *this;
	}

	Vec3& operator/=(Vec3 aOther)
	{
		x /= aOther.x;
		y /= aOther.y;
		z /= aOther.z;

        return *this;
	}

	Vec3& operator-=(Vec3 aOther)
	{
		x -= aOther.x;
		y -= aOther.y;
		z -= aOther.z;

        return *this;
	}

	Vec3 operator/(Vec3 aOther)
	{
		Vec3 v(*this);
		v /= aOther;
		return v;
	}

	Vec3 operator+(Vec3 aOther)
	{
		Vec3 v(*this);
		v += aOther;
		return v;
	}

	Vec3 operator-(Vec3 aOther)
	{
		Vec3 v(*this);
		v -= aOther;
		return v;
	}

	Vec3 operator*(float s)
	{
		return {x * s, y * s, z * s};
	}

	void ImGui()
	{
		ImGui::Text("( x:%f y:%f, z:%f", x, y, z);
	}
};

class ChaoticPath
{
public:
    virtual ~ChaoticPath() = default;
	virtual void Update() = 0;
	virtual void Imgui() = 0;
	virtual std::vector<Vec3>& GetPath() = 0;
};