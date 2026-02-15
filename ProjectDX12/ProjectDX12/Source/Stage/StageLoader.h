#pragma once

#include <vector>
#include <string>
#include "GameTypes.h"
#include "Math/Int2/Int2.h"

struct StageData {
    int width = 0;
    int height = 0;
    std::vector<std::vector<CellType>> cells;
    Int2 playerStart{ 0, 0 };
    std::vector<Int2> blockStarts;
};

class StageLoader
{
public:
    static StageData Load(const std::string& filePath);
};
