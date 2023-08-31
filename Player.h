#pragma once
#include "Collider.h"

#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Input.h"
#include "PlayerBullet.h"
#include "Sprite.h"
#include "Enemy.h"
#include <list>
#include "GameScene.h"
#include "Primitive3D.h"
class Player : public Collider 
{
public:
	const float Radius = 1.0f;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t, const Vector3& position);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection& viewProjection);
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection&);

	Vector3 GetWorldPosition() override;
	inline float GetRadius() override { return Radius; };

	inline void SetParent(const WorldTransform* parent) { worldTransForm_.parent_ = parent; };
	inline void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	//弾リストを取得
	inline const std::list<PlayerBullet*>& GetBullets() { return bullets_; }

	// 衝突時に呼び出されるコールバック関数
	void OnCollision() override;

	void DrawUI();

	void ScreenToWorld(const ViewProjection& viewProjection);

	void LockOn();

private:
	WorldTransform worldTransForm_;
	//Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;

	Input* input_ = nullptr;
	PlayerBullet* bullet_ = nullptr;

	std::list<PlayerBullet*> bullets_;

	//3Dレティクル用
	WorldTransform worldTransform3DReticle_;

	Sprite* sprite2DReticle_ = nullptr;


	//ロックオン
	bool isLockon = false;

	uint32_t textureReticle_;

	//Enemy* enemy_ = nullptr;
	std::weak_ptr<Enemy> enemy_;

	struct Target
	{
		std::weak_ptr<Enemy> enemy;
		std::unique_ptr<Sprite> cursor_;
	};

	std::list<Target*> target_;

	Primitive3D* model_;
	GameScene* gameScene_;
};

