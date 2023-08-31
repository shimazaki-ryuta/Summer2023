#pragma once
#include "Vector2.h"
#include "Sprite.h"

#include <sstream>
#include <vector>



#include "Collider.h"
#include "CollisionManager.h"

#include "Input.h"

#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Player.h"
#include "Enemy.h"

#include "RailCamera.h"

#include "Collider.h"
#include "CollisionManager.h"

#include "CommonFiles/DirectXCommon.h"

class GameScene
{
public:
	struct Transforms
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	GameScene();
	~GameScene();
	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw3D();
	void Draw2D();

	/// <summary>
	/// 衝突判定と応答
	/// </summary> 
	void CheckAllCollisions();

	void AddEnemyBullet(std::unique_ptr<EnemyBullet>);


	/// <summary>
	/// 敵発生
	/// </summary>
	void EnemyPop(const Vector3&);


	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	/// <summary>
	/// 衝突マネージャの利用
	/// </summary>
	void UseCollisionManager();

	std::shared_ptr<Enemy> GetLockonEnemy(const Vector2&);


private:
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	uint32_t textureHandle_ = 0;


	//ビュープロジェクション
	ViewProjection viewProjection_;

	//自キャラ
	Player* player_ = nullptr;

	//敵
	//Enemy* enemy_ = nullptr;
	std::list<std::shared_ptr<Enemy>> enemys_;


	//敵弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;
	//std::list<std::unique_ptr<EnemyBullet>>::iterator iterator;

	//レールカメラ
	RailCamera* railCamera_ = nullptr;

	//敵発生コマンド
	std::stringstream enemyPopCommands;
	bool isWait_ = false;
	int32_t waitTime_;

	//衝突マネージャ
	CollisionManager* collisionManager_ = nullptr;

	std::vector<Vector3> controlPoints_;

	//Matrix4x4 spriteUVTransform = MakeIdentity4x4();
	Vector3 uvTranslate = { 0.0f,0.0f,0.0f };
	Vector3 uvScale = { 1.0f,1.0f,1.0f };
	Vector3 uvRotate = { 0.0f,0.0f,0.0f };

	uint32_t sTextureHandle;
	uint32_t gTextureHandle;
	uint32_t nowTextureHandle;

	struct Transforms starttrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,300.0f,0.0f } };
	Sprite* start;

	struct Transforms goaltrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,180.0f,0.0f } };
	Sprite* goal;

	struct Transforms nowtrans { {1.0f, 1.0f, 1.0f}, { 0.0f,0.0f,0.0f }, { 320.0f,180.0f,0.0f } };
	Sprite* now;
	Vector3 s = { 0.0f,0.0f,0.0f };
	Vector3 g = { 0.0f,0.0f,200.0f };
	float t = 0.0f;

};

