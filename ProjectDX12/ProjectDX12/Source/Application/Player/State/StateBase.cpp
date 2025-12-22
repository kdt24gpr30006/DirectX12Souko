#include "StateBase.h"
#include "../Player.h"

StateBase::StateBase()
	: stateMachine(nullptr)
{
}

StateBase::~StateBase()
{
}

void StateBase::SetStateMachine(StateMachine* stateMachine)
{
	this->stateMachine = stateMachine;
}