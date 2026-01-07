#pragma once
#include <array>

class FbxMesh;

class Stage;

class Field
{
public:
    bool Initialize(Stage* stage);
    void Update();
    void Render();
    void Release();

private:
    static constexpr int GridW = 9;
    static constexpr int GridH = 9;

    std::array<FbxMesh*, GridW* GridH> Cells;
};

