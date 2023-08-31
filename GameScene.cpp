#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <list>
#include <fstream>
#include "Curve.h"
#include  "Primitive3D.h"
GameScene::GameScene() {}

GameScene::~GameScene() {

	delete player_;
	//delete enemy_;
	delete railCamera_;
	delete collisionManager_;
}

void GameScene::Initialize(DirectXCommon* dxCommon) {

	dxCommon_ = dxCommon;
	input_ = Input::GetInstance();

	//レティクルのテクスチャ
	//TextureManager::Load("2DReticle.png");


	viewProjection_.Initialize(1280,720);
Vector3 playerPosition(0, 0, 30.0f);
	player_ = new Player();
	player_->Initialize(textureHandle_, playerPosition);
	player_->SetGameScene(this);

	//敵発生コマンドの読み取り
	LoadEnemyPopData();
	/*
	//敵の生成
	std::unique_ptr<Enemy> enemy;
	enemy.reset(new Enemy());
	enemy->SetPlayer(player_);
	enemy->SetGameScene(this);
	enemy->Initialize(model_, Vector3(10.0f, 2.0f, 50.0f), Vector3(0.0f, 0.0f, -0.1f));
	enemys_.push_back(std::move(enemy));
	*/


	controlPoints_ = {
		{0,  0,  0},
		{0, 0, 100},
		
	};

	//レールカメラ
	railCamera_ = new RailCamera();
	railCamera_->Initialize(Vector3{ 0.0f, 0.0f, -10.0f }, Vector3{ 0.0f, 0.0f, 0.0f });
	railCamera_->SetControlPoints(controlPoints_);
	//親子関係
	player_->SetParent(&railCamera_->GetWorldTransform());

	collisionManager_ = new CollisionManager();


	sTextureHandle = TextureManager::LoadTexture("s.png");
	gTextureHandle = TextureManager::LoadTexture("g.png");
	nowTextureHandle = TextureManager::LoadTexture("now.png");
	Vector4 spriteColor = { 1.0f,1.0f,1.0f,1.0f };

	start = Sprite::Create(sTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);

	goal = Sprite::Create(gTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);

	now = Sprite::Create(nowTextureHandle, Vector2{ 0.0f,0.0f }, Vector2{ 36.0f,36.0f }, spriteColor);
}

void GameScene::Update() {

	if (false)
	{
	}
	else
	{
		//viewProjection_.UpdateMatrix();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		//viewProjection_.TransferMatrix();
	}

	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		if (bullet->IsDead()) {
			return true;
		}
		return false;
		});
	// デスフラグの立ったEnemyを削除
	enemys_.remove_if([](std::shared_ptr<Enemy>& enemy) {
		if (enemy->IsDead()) {
			return true;
		}
		return false;
		});

	player_->Update(viewProjection_);
	/*
	if (enemy_)
	{
		enemy_->Update();
	}
	*/

	//敵発生コマンド
	UpdateEnemyPopCommands();

	//敵更新
	for (std::list<std::shared_ptr<Enemy>>::iterator iterator = enemys_.begin();
		iterator != enemys_.end(); iterator++) {
		(*iterator)->Update();
	}
	//敵弾更新
	for (std::list<std::unique_ptr<EnemyBullet>>::iterator iterator = enemyBullets_.begin();
		iterator != enemyBullets_.end(); iterator++) {
		(*iterator)->Update();
	}

	//CheckAllCollisions();
	UseCollisionManager();

	//skydome_->Update();

	//debugCamera_->Update();
	railCamera_->Update();


	//Sprite用のworldviewProjection
	//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	//Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
	Matrix4x4 uvTransformSprite = MakeAffineMatrix(uvScale, uvRotate, uvTranslate);

	
	Matrix4x4 worldMatrixs = MakeAffineMatrix(starttrans.scale, starttrans.rotate, starttrans.translate);
	Matrix4x4 worldViewProjectionMatrixs = Multiply(worldMatrixs, Multiply(viewMatrixSprite, projectionMatrixSprite));
	start->SetWVP(worldViewProjectionMatrixs);
	start->SetUVTransform(uvTransformSprite);


	Matrix4x4 worldMatrixg = MakeAffineMatrix(goaltrans.scale, goaltrans.rotate, goaltrans.translate);
	Matrix4x4 worldViewProjectionMatrixg = Multiply(worldMatrixg, Multiply(viewMatrixSprite, projectionMatrixSprite));
	goal->SetWVP(worldViewProjectionMatrixg);
	goal->SetUVTransform(uvTransformSprite);


	nowtrans.translate = (1.0f - t) * starttrans.translate + t * goaltrans.translate;

	Matrix4x4 worldMatrixn = MakeAffineMatrix(nowtrans.scale, nowtrans.rotate, nowtrans.translate);
	Matrix4x4 worldViewProjectionMatrixn = Multiply(worldMatrixn, Multiply(viewMatrixSprite, projectionMatrixSprite));
	now->SetWVP(worldViewProjectionMatrixn);
	now->SetUVTransform(uvTransformSprite);
}

void GameScene::Draw2D() {

	//player_->DrawUI();
	start->Draw();
	goal->Draw();
	now->Draw();
}

void GameScene::Draw3D() {
	Primitive3D::PreDraw(dxCommon_->GetCommandList());

	
	player_->Draw(viewProjection_);
	/* if (enemy_) {
		enemy_->Draw(viewProjection_);
	}*/
	for (std::list<std::shared_ptr<Enemy>>::iterator iterator = enemys_.begin();
		iterator != enemys_.end(); iterator++) {
		(*iterator)->Draw(viewProjection_);
	}
	for (std::list<std::unique_ptr<EnemyBullet>>::iterator iterator = enemyBullets_.begin();
		iterator != enemyBullets_.end(); iterator++) {
		(*iterator)->Draw(viewProjection_);
	}
	Primitive3D::PostDraw();
}

void GameScene::UseCollisionManager()
{
	collisionManager_->ClearList();

	collisionManager_->PushCollider(player_);
	for (std::list<std::shared_ptr<Enemy>>::iterator enemy = enemys_.begin();
		enemy != enemys_.end(); enemy++) {
		collisionManager_->PushCollider(enemy->get());
	}
	for (PlayerBullet* playerBullet : player_->GetBullets()) {
		collisionManager_->PushCollider(playerBullet);
	}
	for (std::list<std::unique_ptr<EnemyBullet>>::iterator enemyBullet = enemyBullets_.begin();
		enemyBullet != enemyBullets_.end(); enemyBullet++) {
		collisionManager_->PushCollider(enemyBullet->get());
	}

	collisionManager_->CheckAllCollisions();

}

void GameScene::CheckAllCollisions() {
	//Vector3 posA, posB;
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	std::list<Collider*> colliders_;
	colliders_.push_back(player_);
	for (std::list<std::shared_ptr<Enemy>>::iterator enemy = enemys_.begin();
		enemy != enemys_.end(); enemy++) {
		colliders_.push_back(enemy->get());
	}
	for (PlayerBullet* playerBullet : playerBullets) {
		colliders_.push_back(playerBullet);
	}
	for (std::list<std::unique_ptr<EnemyBullet>>::iterator enemyBullet = enemyBullets_.begin();
		enemyBullet != enemyBullets_.end(); enemyBullet++) {
		colliders_.push_back(enemyBullet->get());
	}


	//std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();
	//std::list<std::unique_ptr<EnemyBullet>>::iterator iterator;

#pragma region 自弾と敵キャラの当たり判定

	for (std::list<Collider*>::iterator iteA = colliders_.begin(); iteA != colliders_.end();
		++iteA) {
		Collider* colliderA = *iteA;
		std::list<Collider*>::iterator iteB = iteA;
		iteB++;
		for (; iteB != colliders_.end(); ++iteB)
		{
			Collider* collierB = *iteB;
			CheckCollisionPair(colliderA, collierB);
		}
	}

#pragma endregion
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	if (!(colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) ||
		!(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()))
	{
		return;
	}

	Vector3 posA = colliderA->GetWorldPosition(), posB = colliderB->GetWorldPosition();
	float distance = float(
		std::pow(posB.x - posA.x, 2) + std::pow(posB.y - posA.y, 2) + std::pow(posB.z - posA.z, 2));
	if (distance <= std::pow(colliderA->GetRadius() + colliderB->GetRadius(), 2)) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}


void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> bullet)
{
	enemyBullets_.push_back(std::move(bullet));
}


void GameScene::EnemyPop(const Vector3& position)
{
	// 敵の生成
	std::unique_ptr<Enemy> enemy;
	enemy.reset(new Enemy());
	enemy->SetPlayer(player_);
	enemy->SetGameScene(this);
	enemy->Initialize(position, Vector3(0.0f, 0.0f, -0.1f));
	enemys_.push_back(std::move(enemy));
}

void GameScene::LoadEnemyPopData()
{

	std::ifstream file;
	file.open("./Resources/enemyPop.csv");
	assert(file.is_open());

	enemyPopCommands << file.rdbuf();

	file.close();
}

void GameScene::UpdateEnemyPopCommands()
{
	if (isWait_)
	{
		waitTime_--;
		if (waitTime_ <= 0)
		{
			isWait_ = false;
		}
		return;
	}


	std::string line;

	while (getline(enemyPopCommands, line))
	{
		std::istringstream line_stream(line);
		std::string word;
		getline(line_stream, word, ',');

		if (word.find("//") == 0)
		{
			continue;
		}

		//POPコマンド
		if (word.find("POP") == 0)
		{
			getline(line_stream, word, ',');
			float x = float(std::atof(word.c_str()));
			getline(line_stream, word, ',');
			float y = float(std::atof(word.c_str()));
			getline(line_stream, word, ',');
			float z = float(std::atof(word.c_str()));

			EnemyPop(Vector3(x, y, z));
		}

		//WAITコマンド
		else if (word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');
			int32_t waitTime = atoi(word.c_str());

			isWait_ = true;
			waitTime_ = waitTime;

			break;
		}
	}

}

std::shared_ptr<Enemy> GameScene::GetLockonEnemy(const Vector2& cursorPosition) {
	const float lockonLength = 30;
	for (std::list<std::shared_ptr<Enemy>>::iterator iterator = enemys_.begin();
		iterator != enemys_.end(); iterator++) {
		Vector3 enemyPosition3d = (*iterator)->GetWorldPosition();
		Matrix4x4 matViewport =
			MakeViewportMatrix(0, 0,1280,720, 0, 1);

		Matrix4x4 matViewProjectionViewport =
			viewProjection_.matView * viewProjection_.matProjection * matViewport;

		enemyPosition3d = Transform(enemyPosition3d, matViewProjectionViewport);
		Vector2 screenEnemyPosition(enemyPosition3d.x, enemyPosition3d.y);
		float distance2 = float(std::pow(screenEnemyPosition.x - cursorPosition.x, 2) + std::pow(screenEnemyPosition.y - cursorPosition.y, 2));

		if (distance2 <= float(std::pow(lockonLength, 2)))
		{
			return *iterator;
		}
	}
	return nullptr;
}