#pragma once
#include "Math/Vector3/Vector3.h"

class Entity
{
protected:
    Math::Vector3 position{ 0,0,0 };

public:
    virtual ~Entity() = default;

    const Math::Vector3& GetPosition() const
    {
        return position;
    }

    void SetPosition(const Math::Vector3& pos)
    {
        position = pos;
    }
};