#pragma once

#include <vector>
#include "GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"
#include "Math/Vector3/Vector3.h"

class Field;

/// <summary>
/// ステージ（グリッド＋ブロック管理）
/// </summary>
class Stage
{
public:
    static constexpr int GRID_SIZE = 9;
    static constexpr float CELL_SIZE = 1.0f;

private:
    // グリッド情報
    CellType grid[GRID_SIZE][GRID_SIZE]{};

    // ブロック一覧
    std::vector<Block> blocks;

    // 爆発フラグ
    bool bHasExplosion = false;

    // 床
    Field* field = nullptr;

private:
    /// <summary>
    /// グリッド内判定
    /// </summary>
    bool IsInside(const Int2& p) const;

public:
    Stage() = default;

    /// <summary>
    /// ステージ初期化
    /// </summary>
    void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime);

    /// <summary>
    /// 描画処理
    /// </summary>
    void Render();

    void Release();

    /// <summary>
    /// グリッド座標 → ワールド座標
    /// </summary>
    Math::Vector3 GridToWorld(const Int2& p) const;

    /// <summary>
    /// セル情報取得
    /// </summary>
    CellType GetCellType(const Int2& p) const;

    /// <summary>
    /// 指定グリッドにあるブロック取得
    /// </summary>
    Block* GetBlockAt(const Int2& p);

    /// <summary>
    /// 押し処理（1マス単位）
    /// </summary>
    MoveResult TryPush(Block& block, const Int2& dir);

    /// <summary>
    /// ブロック一覧取得
    /// </summary>
    const std::vector<Block>& GetBlocks() const { return blocks; }

    /// <summary>
    /// 爆発発生通知
    /// </summary>
    void OnBlockExploded(Block& block);

    /// <summary>
    /// 爆発したか
    /// </summary>
    bool HasExplosion() const;
};
