#pragma once
#include "../Stage/GameTypes.h"

class Entity
{
public:
	Int2 pos{ 0, 0 };

	virtual ~Entity() = default;

	void Move(const Int2 delta)
	{
		pos.x += delta.x;
		pos.y += delta.y;
	}
};

/// <summary>
/// ゴールまで運ぶブロック
/// </summary>
class Block : public Entity
{

};