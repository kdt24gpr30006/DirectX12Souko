#pragma once
#include "../Entity.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Int2/Int2.h"
#include "../../Stage/GameTypes.h"

class Stage;

/// <summary>
/// �S�[���܂ŉ^�ԃu���b�N
/// </summary>
class Block : public Entity
{
	// �O���b�h��̘_�����W
	Int2 gridPos{ 0, 0 };

	// �ړ���̃O���b�h���W
	Int2 targetGridPos{ 0, 0 };

	// �ړ��J�n�n�_
	Math::Vector3 startWorldPos;

	// �ړ��ڕW�n�_
	Math::Vector3 targetWorldPos;

	// �ړ�����
	bool bIsMoving = false;

	// �ړ����x
	const float MOVE_SPEED = 25.0f;

	// �ړ�����
	MoveEndResult lastMoveResult = MoveEndResult::None;

public:

	Block();
	~Block() { Release(); }

	// �R�s�[�֎~()
	Block(const Block&) = delete;
	Block& operator=(const Block&) = delete;

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="stage"></param>
	/// <returns></returns>
	bool Init() override;

	/// <summary>
	/// �X�V����
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="stage"></param>
	void Update(float deltaTime, const Stage& stage);

	/// <summary>
	/// �I������
	/// </summary>
	void Release() override;

	/// <summary>
	/// �O���b�h�ʒu�̃Q�b�^�[
	/// </summary>
	/// <returns></returns>
	const Int2& GetGridPos() const;

	/// <summary>
	/// �O���b�h�ʒu�̃Z�b�^�[
	/// </summary>
	/// <param name="pos"></param>
	void SetGridPos(const Int2& pos);

	/// <summary>
	/// �w�肵�������ւ̈ړ����J�n
	/// </summary>
	/// <param name="dir"></param>
	void StartMove(const Int2& dir, const Stage& stage);

	/// <summary>
	/// �ړ����ʂ�none�ł͂Ȃ���
	/// </summary>
	/// <returns></returns>
	bool HasMoveResult() const
	{
		return lastMoveResult != MoveEndResult::None;
	}

	/// <summary>
	/// �ړ����ʂ��擾�����Z�b�g����
	/// </summary>
	/// <returns></returns>
	MoveEndResult ConsumeMoveResult()
	{
		MoveEndResult r = lastMoveResult;
		lastMoveResult = MoveEndResult::None;
		return r;
	}

	/// <summary>
	/// �ړ������@
	/// ���ʂ�Ԃ�
	/// </summary>
	/// <param name="stage"></param>
	/// <returns></returns>
	MoveEndResult FinishMove(const Stage& stage);

	/// <summary>
	/// �ړ�����
	/// </summary>
	/// <returns></returns>
	bool IsMoving() const { return bIsMoving; }
};
