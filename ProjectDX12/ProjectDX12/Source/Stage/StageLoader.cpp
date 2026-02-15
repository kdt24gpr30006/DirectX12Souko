#include "StageLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

StageData StageLoader::Load(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open stage file: " + filePath);
    }

    StageData data;
    std::string line;

    while (std::getline(file, line))
    {
        // 空行スキップ
        if (line.empty())
            continue;

        // 末尾の\rを除去（Windows改行対応）
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty())
            continue;

        std::vector<CellType> row;
        std::istringstream ss(line);
        std::string token;
        int x = 0;

        while (std::getline(ss, token, ','))
        {
            // 前後の空白を除去
            while (!token.empty() && token.front() == ' ') token.erase(token.begin());
            while (!token.empty() && token.back() == ' ') token.pop_back();

            // 空トークン → ステージ外
            if (token.empty())
            {
                row.push_back(CellType::None);
                x++;
                continue;
            }

            if (token == "1")
            {
                row.push_back(CellType::Wall);
            }
            else if (token == "G")
            {
                row.push_back(CellType::Goal);
            }
            else if (token == "E")
            {
                row.push_back(CellType::Explosion);
            }
            else if (token == "P")
            {
                data.playerStart = { x, static_cast<int>(data.cells.size()) };
                row.push_back(CellType::Empty);
            }
            else if (token == "B")
            {
                data.blockStarts.push_back({ x, static_cast<int>(data.cells.size()) });
                row.push_back(CellType::Empty);
            }
            else // "0" or anything else
            {
                row.push_back(CellType::Empty);
            }

            x++;
        }

        data.cells.push_back(std::move(row));
    }

    // 全行の最大幅を算出
    int maxWidth = 0;
    for (const auto& row : data.cells)
    {
        int rowSize = static_cast<int>(row.size());
        if (rowSize > maxWidth)
            maxWidth = rowSize;
    }

    // 短い行を CellType::None でパディング
    for (auto& row : data.cells)
    {
        while (static_cast<int>(row.size()) < maxWidth)
            row.push_back(CellType::None);
    }

    data.height = static_cast<int>(data.cells.size());
    data.width = maxWidth;

    return data;
}
