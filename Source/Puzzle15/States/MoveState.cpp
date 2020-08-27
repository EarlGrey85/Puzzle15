#include "MoveState.h"
#include "Puzzle15/Tile.h"


void MoveState::Tick(float deltaTime)
{
    _tile->MoveStateTick(deltaTime);
}
