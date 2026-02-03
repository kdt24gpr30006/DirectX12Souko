#pragma once

#include <vector>
#include <memory>
#include "GameTypes.h"
#include "../Entity/Block/Block.h"
#include "Math/Int2/Int2.h"
#include <Math/Vector3/Vector3.h>

class Field;

/// <summary>
/// �X�e�[�W�i�O���b�h�{�u���b�N�Ǘ��j
/// </summary>
class Stage
{
    // �O���b�h�̃T�C�Y
    static constexpr int GRID_SIZE = 9;
    // 1�Z���̃T�C�Y
    static constexpr float CELL_SIZE = 10.0f;

    // �O���b�h���
    CellType grid[GRID_SIZE][GRID_SIZE]{};

    // �u���b�N�ꗗ
    std::vector<std::unique_ptr<Block>> blocks;

    // �����t���O
    bool bHasExplosion = false;

    // �S�[���t���O
    bool bHasGoal = false;

    // ��
    Field* field = nullptr;

    /// <summary>
    /// �O���b�h��������
    /// </summary>
    bool IsInside(const Int2& p) const;

public:

    static constexpr int GetGridSize() { return GRID_SIZE; }
    static constexpr float GetCellSize() { return CELL_SIZE; }

    Stage() = default;
    ~Stage();

    /// <summary>
    /// �X�e�[�W������
    /// </summary>
    void Init();

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="deltaTime"></param>
	void Update(float deltaTime);

    /// <summary>
    /// �`�揈��
    /// </summary>
    void Draw();

    void Release();

    /// <summary>
    /// �O���b�h���W �� ���[���h���W
    /// </summary>
    Math::Vector3 GridToWorld(const Int2& p) const;

    /// <summary>
    /// ���[���h���W �� �O���b�h���W
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    Int2 WorldToGrid(const Math::Vector3& pos) const;

    /// <summary>
    /// �Z�����擾
    /// </summary>
    CellType GetCellType(const Int2& p) const;

    /// <summary>
    /// �w��O���b�h�ɂ���u���b�N�擾
    /// </summary>
    Block* GetBlockAt(const Int2& p);

    /// <summary>
    /// ���������i1�}�X�P�ʁj
    /// </summary>
    MoveResult TryPush(Block& block, const Int2& dir);

    /// <summary>
    /// �u���b�N�ꗗ�擾
    /// </summary>
    const std::vector<std::unique_ptr<Block>>& GetBlocks() const { return blocks; }

    /// <summary>
    /// ���������ʒm
    /// </summary>
    void OnBlockExploded();

    /// <summary>
    /// �S�[�����B�ʒm
    /// </summary>
    void OnBlockGoal();

    /// <summary>
    /// ����������
    /// </summary>
    bool HasExplosion() const;

    /// <summary>
    /// ゴールしたか
    /// </summary>
    /// <returns></returns>
    bool HasGoal() const;

    /// <summary>
    /// 全ブロックがゴール上にあるか（クリア判定）
    /// </summary>
    bool IsCleared() const;
};
