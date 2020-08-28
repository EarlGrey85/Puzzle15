#pragma once

struct Coord
{
    int X;
    int Y;

    bool operator == (const Coord& other) const
    {
        return X == other.X && Y == other.Y;
    }

    size_t operator()(const Coord& pointToHash) const noexcept
    {
        const size_t hash = pointToHash.X + pointToHash.Y * 10;
        return hash;
    }

    Coord()
    {
    }
    
    Coord(int x, int y)
    {
        X = x;
        Y = y;
    }
};
