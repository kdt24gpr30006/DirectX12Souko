#pragma once
#include "../SceneStateBase.h"

class StateTitle : public SceneStateBase
{
public:
	void Enter();
	void Update(float dt);
	void Render(float dt);
};