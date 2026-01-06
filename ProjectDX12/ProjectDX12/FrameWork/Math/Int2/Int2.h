#pragma once

/// <summary>
/// intŒ^‚Ì2ŸŒ³ƒxƒNƒgƒ‹
/// </summary>
struct Int2
{
    int x;
    int y;

    /// <summary>
    /// Int2“¯m‚Ì‘«‚µZ
    /// </summary>
    /// <param name="rhs"></param>
    /// <returns></returns>
    Int2 operator+(const Int2& rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    /// <summary>
    /// Int2“¯m‚Ì”’l‚ª“™‚µ‚¢‚©
    /// </summary>
    /// <param name="rhs"></param>
    /// <returns></returns>
    bool operator==(const Int2& rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    static const Int2 Up;
    static const Int2 Down;
    static const Int2 Left;
    static const Int2 Right;
};