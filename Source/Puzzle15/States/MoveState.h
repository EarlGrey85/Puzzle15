#pragma once
#include "State.h"

class MoveState : public State
{
public:
    explicit MoveState(ATile* tile)
        : State(tile)
    {
    }

    virtual void Tick(float deltaTime) override;
    
};
