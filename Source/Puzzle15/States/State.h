#pragma once

class ATile;

class State
{
public:
    virtual ~State() = default;

    explicit State(ATile* tile);

    virtual void Tick(float deltaTime) = 0;

protected:
    ATile* _tile;
    
};
