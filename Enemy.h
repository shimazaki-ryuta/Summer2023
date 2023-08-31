#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "EnemyBullet.h"
#include "EnemyState.h"
#include <list>
#include "Collider.h"

#include "TimedCall.h"
#include "Primitive3D.h"
enum class Phase {
	Approach,
	Leave,
};

class Player;

class GameScene;

///< summary>
/// 敵
///</summary>
class Enemy : public Collider {
public:
	const float Radius = 1.0f;
	~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>]
	/// <param name="positon">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(const Vector3& position, const Vector3& velocity);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	///フェーズ
	void Approach();
	void Leave();
	//フェーズ初期化
	void ApproachInitialize();

	void ChangeState(BaseEnemyState*);

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 弾を発射しタイマーをリセットする
	/// </summary>
	void FireCall();

	/// <summary>
	/// 移動
	/// </summary>
	void Move(const Vector3& velocity);



	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	inline void SetPlayer(Player* player) { player_ = player; }
	inline void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	Vector3 GetWorldPosition() override;
	inline Matrix4x4 GetWorldTransformMatrix() { return worldTransForm_.matWorld_; };
	inline float GetRadius() override { return Radius; };
	//inline Vector3 GetPosition() { return worldTransForm_.translation_; };
	inline Vector3 GetApproachVelocity() { return approachvelocity_; };
	inline Vector3 GetLeaveVelocity() { return leavevelocity_; };
	inline std::list<TimedCall*>& GetFireCalls() { return timedCalls_; };

	//衝突時に呼び出されるコールバック関数
	void OnCollision() override;

	inline bool IsDead() const { return isDead_; };
	// 弾リストを取得
	//inline std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets_; }


private:
	WorldTransform worldTransForm_;
	
	uint32_t textureHandle_ = 0u;
	Vector3 velocity_;

	Vector3 approachvelocity_;
	Vector3 leavevelocity_;

	Phase phase_ = Phase::Approach;

	//void (Enemy::*pFunc)();
	//static void(Enemy::*phaseTable[])();

	BaseEnemyState* state_ = nullptr;

	//std::list<std::unique_ptr<EnemyBullet>> bullets_;
	//std::list<std::unique_ptr<EnemyBullet>>::iterator iterator;

	int32_t fireTimer = 0;

	std::list<TimedCall*> timedCalls_;

	//自キャラ
	Player* player_ = nullptr;

	GameScene* gameScene_;
	Primitive3D* model_;
	bool isDead_ = false;
};