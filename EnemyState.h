#pragma once
#include <stdint.h>
#include "Vector3.h"

class Enemy;

class BaseEnemyState
{
public:
	virtual ~BaseEnemyState() {};
	virtual void Initialize(Enemy* enemy) = 0;
	virtual void Update() = 0;

private:
	Enemy* enemy_ = nullptr;
};

class EnemyStateApproach : public BaseEnemyState
{
public:
	EnemyStateApproach(Enemy* enemy);
	inline void Initialize(Enemy* enemy) { enemy_ = enemy; };
	void Update();

private:
	Enemy* enemy_ = nullptr;
	int32_t fireTimer = 0;
};

class EnemyStateLeave : public BaseEnemyState {
public:
	EnemyStateLeave(Enemy* enemy);
	inline void Initialize(Enemy* enemy) { enemy_ = enemy; };
	void Update();

private:
	Enemy* enemy_ = nullptr;

};