#pragma once
#include "State.h"

class IdleState : public State
{
public:
    explicit IdleState(ATile* tile)
        : State(tile)
    {
    }

    virtual void Tick(float deltaTime) override;
};
