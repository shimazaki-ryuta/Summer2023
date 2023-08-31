#include "Player.h"
#include "CommonFiles/WinApp.h"
#include <cassert>
#include <algorithm>
#include "TextureManager.h"
#include"MatrixFunction.h"
#include"VectorFunction.h"
#include <numbers>
#include "DeltaTime.h"
Player::~Player() {
	// Bulletの解放
	// delete bullet_;
	for (PlayerBullet* bullet : bullets_)
	{
		delete bullet;
	}
	delete sprite2DReticle_;
}
void Player::Initialize(uint32_t textureHandle, const Vector3& position) {
	this->textureHandle_ = textureHandle;
	worldTransForm_.translation_ = position;
	worldTransForm_.Initialize();
	input_ = Input::GetInstance();

	worldTransform3DReticle_.Initialize();

	//textureReticle_ = TextureManager::Load("2DReticle.png");

	//sprite2DReticle_ = Sprite::Create(textureReticle_, Vector2(float(WinApp::kWindowWidth) / 2.0f, float(WinApp::kWindowHeight) / 2.0f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.5f, 0.5f));

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(~kCollisionAttributePlayer);

	model_ = Primitive3D::Create(Vector2{ 0.0f,0.0f }, Vector2{ 640.0f,360.0f }, Vector4{ 0.0f,0.0f,1.0f,1.0f });
}

void Player::Update(const ViewProjection& viewProjection) {

	//ゲームパッドの状態をえる
	//XINPUT_STATE joyState;



	//デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead())
		{
			delete bullet;
			return true;
		}
		return false;
		});

	target_.remove_if([](Target* target) {
		if (!target->enemy.lock())
		{
			return true;
		}
		return false;
		});

	Vector3 move = { 0,0,0 };
	//移動処理
	const float kCharacterSpeed = 0.2f;
	/* if (input_->PushKey(DIK_LEFT))
	{
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT))
	{
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}*/
	/*if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += float(joyState.Gamepad.sThumbLX) / SHRT_MAX * kCharacterSpeed;
		move.y += float(joyState.Gamepad.sThumbLY) / SHRT_MAX * kCharacterSpeed;
	}
	*/
	if (Input::GetKey(DIK_RIGHT))
	{
		move.x += 3.0f * DeltaTime::GetDeltaTime();
	}
	if (Input::GetKey(DIK_LEFT))
	{
		move.x -= 3.0f * DeltaTime::GetDeltaTime();
	}
	if (Input::GetKey(DIK_UP))
	{
		move.y += 3.0f * DeltaTime::GetDeltaTime();
	}
	if (Input::GetKey(DIK_DOWN))
	{
		move.y -= 3.0f * DeltaTime::GetDeltaTime();
	}
	worldTransForm_.translation_ += move;

	const float kMoveLimitX = 32.0f;
	const float kMoveLimitY = 18.0f;

	worldTransForm_.translation_.x =
		std::clamp(worldTransForm_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransForm_.translation_.y =
		std::clamp(worldTransForm_.translation_.y, -kMoveLimitY, kMoveLimitY);
	//旋回処理
	Rotate();


	//行列更新
	//worldTransForm_.matWorld_ =
	//    MakeAffineMatrix(worldTransForm_.scale_, worldTransForm_.rotation_, worldTransForm_.translation_);

	//worldTransForm_.TransferMatrix();

	worldTransForm_.UpdateMatrix();

	//攻撃処理
	Attack();
	//弾更新
	for (PlayerBullet* bullet : bullets_)
	{
		bullet->Update();
	}

	//キャラクタの座標を表示する
	float* slider3[3] = {
		&worldTransForm_.translation_.x, &worldTransForm_.translation_.y,
		&worldTransForm_.translation_.z };
	/*ImGui::Begin("Player");
	ImGui::SliderFloat3("", *slider3, -100.0f, 100.0f);
	ImGui::End();
	*/

	//3Dレティクル
	{
		const float kDistancePlayer3DReticle = 50.0f;

		Vector3 offset = { 0,0,1.0f };

		offset = TransformNormal(offset, worldTransForm_.matWorld_);

		offset = Nomalize(offset) * kDistancePlayer3DReticle;

		worldTransform3DReticle_.translation_ = GetWorldPosition() + offset;
		worldTransform3DReticle_.UpdateMatrix();
	}
	//2Dレティクル
	{
		Vector3 positionReticle;
		positionReticle.x = worldTransform3DReticle_.matWorld_.m[3][0];
		positionReticle.y = worldTransform3DReticle_.matWorld_.m[3][1];
		positionReticle.z = worldTransform3DReticle_.matWorld_.m[3][2];

		//Matrix4x4 matViewport = MakeViewportMatrix(0, 0, Window::kClientWidth_, Window::GetClientHeight(), 0, 1);

		//Matrix4x4 matViewProjectionViewport = viewProjection.matView * viewProjection.matProjection * matViewport;

		//positionReticle = Transform(positionReticle, matViewProjectionViewport);

		//sprite2DReticle_->SetPosition(Vector2(positionReticle.x,positionReticle.y));
	}
	//マウスカーソル
	ScreenToWorld(viewProjection);
}

void Player::LockOn()
{
	enemy_ = gameScene_->GetLockonEnemy(sprite2DReticle_->GetPosition());
	if (enemy_.lock()) {
		for (Target* target : target_)
		{
			if (target->enemy.lock() == enemy_.lock())
			{
				return;
			}
		}
		// isLockon = true;
		/*std::unique_ptr<Sprite> sprite;
		sprite.reset(Sprite::Create(
			textureReticle_,
			Vector2(float(WinApp::kWindowWidth) / 2.0f, float(WinApp::kWindowHeight) / 2.0f),
			Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector2(0.5f, 0.5f)));
			*/
		Target* target = new Target;
		//*target = { enemy_, std::move(sprite) };
		//target_.push_back(target);
	}
	else {
		//isLockon = false;
	}
}

void Player::ScreenToWorld(const ViewProjection& viewProjection)
{
	/*// ゲームパッドの状態をえる
	XINPUT_STATE joyState;

	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	Vector2 reticleMove(0, 0);

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		reticleMove.x += float(joyState.Gamepad.sThumbRX) / SHRT_MAX * 10.0f;
		reticleMove.y -= float(joyState.Gamepad.sThumbRY) / SHRT_MAX * 10.0f;
		spritePosition.x += reticleMove.x;
		spritePosition.y += reticleMove.y;
	}
	Vector4 color(1.0f, 1.0f, 1.0f, 1.0f);
	LockOn();
	for (Target* target : target_)
	{
		Vector2 position = target->cursor_->GetPosition();
		std::shared_ptr<Enemy> enemy = target->enemy.lock();
		if (enemy) {
			Matrix4x4 matViewport =
				MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
			Matrix4x4 matViewProjectionViewport =
				viewProjection.matView * viewProjection.matProjection * matViewport;
			Vector3 enemyPosition3d = enemy->GetWorldPosition();
			enemyPosition3d = Transform(enemyPosition3d, matViewProjectionViewport);
			Vector2 screenEnemyPosition(enemyPosition3d.x, enemyPosition3d.y);
			position = screenEnemyPosition;
			color = Vector4{ 1.0f, 0.0f, 0.0f, 1.0f };
			target->cursor_->SetPosition(position);
			target->cursor_->SetColor(color);
		}
	}
	
	sprite2DReticle_->SetPosition(
		spritePosition);
	//sprite2DReticle_->SetColor(color);
	//POINT mousePosition;
	//GetCursorPos(&mousePosition);

	//HWND hwnd = WinApp::GetInstance()->GetHwnd();
	//ScreenToClient(hwnd,&mousePosition);

	//Vector2 clientMousePos = Vector2(float(mousePosition.x), float(mousePosition.y));

	//sprite2DReticle_->SetPosition(clientMousePos);
	Matrix4x4 matViewport =
		MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	Matrix4x4 matVPV = viewProjection.matView * viewProjection.matProjection * matViewport;

	Matrix4x4 matInverseVPV = Inverse(matVPV);

	//スクリーン座標
	Vector3 posNear =
		Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 0);
	Vector3 posFar =
		Vector3(sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y, 1);

	//スクリーン座標系からワールド座標形へ
	posNear = Transform(posNear, matInverseVPV);
	posFar = Transform(posFar, matInverseVPV);

	//マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;
	mouseDirection = Nomalize(mouseDirection);

	const float kDistanceTestObject = 100.0f;

	worldTransform3DReticle_.translation_ = (mouseDirection * kDistanceTestObject) + posNear;
	worldTransform3DReticle_.UpdateMatrix();

	ImGui::Begin("Player");
	ImGui::Text(
		"2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text(
		"3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();
	*/
}

void Player::Rotate()
{
	/*const float kRotSpeed = 0.02f;
	if (input_->PushKey(DIK_A))
	{
		worldTransForm_.rotation_.y -= kRotSpeed;
	}
	else if (input_->PushKey(DIK_D))
	{
		worldTransForm_.rotation_.y += kRotSpeed;
	}*/
}

void Player::Attack()
{
	// ゲームパッドの状態をえる
	//XINPUT_STATE joyState;

	
	if (Input::GetKey(DIK_SPACE)) {
		//弾があれば解放
		/* if (bullet_)
		{
			delete bullet_;
			bullet_ = nullptr;
		}*/

		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0.0f, 0.0f, kBulletSpeed);

		Vector3 worldPos3DReticle;
		worldPos3DReticle.x = worldTransform3DReticle_.matWorld_.m[3][0];
		worldPos3DReticle.y = worldTransform3DReticle_.matWorld_.m[3][1];
		worldPos3DReticle.z = worldTransform3DReticle_.matWorld_.m[3][2];

		velocity = worldPos3DReticle - GetWorldPosition();
		velocity = Nomalize(velocity) * kBulletSpeed;


		//velocity = TransformNormal(velocity, worldTransForm_.matWorld_);

		if (target_.empty())
		{
			// 弾を生成、初期化
			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(GetWorldPosition(), velocity, enemy_);

			// bullet_ = newBullet;
			bullets_.push_back(newBullet);
		}

		for (Target* target : target_)
		{
			if (std::shared_ptr<Enemy> enemy = target->enemy.lock())
			{
				velocity.z = -1.0f;
				float theta =
					float(rand()) * float(std::numbers::pi) * 2.0f / float(RAND_MAX) - float(std::numbers::pi);
				velocity.x = cosf(theta);
				velocity.y = sin(theta);
				velocity = Nomalize(velocity) * 3.0f;
				// 弾を生成、初期化
				PlayerBullet* newBullet = new PlayerBullet();
				newBullet->Initialize(GetWorldPosition(), velocity, target->enemy);

				// bullet_ = newBullet;
				bullets_.push_back(newBullet);
			}
		}
		target_.clear();
	}
}

void Player::Draw(ViewProjection& viewProjection)
{
	Matrix4x4 worldViewProjectionMatrix = worldTransForm_.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	model_->Draw(worldViewProjectionMatrix, Vector4{0.0f,0.0f,1.0f,1.0f});

	for (PlayerBullet* bullet : bullets_)
	{
		bullet->Draw(viewProjection);
	}

	//model_->Draw(worldTransform3DReticle_,viewProjection);
}

void Player::DrawUI()
{
	sprite2DReticle_->Draw();
	for (Target* target : target_)
	{
		target->cursor_->Draw();
	}
}

Vector3 Player::GetWorldPosition()
{
	Vector3 worldPos;
	worldPos.x = worldTransForm_.matWorld_.m[3][0];
	worldPos.y = worldTransForm_.matWorld_.m[3][1];
	worldPos.z = worldTransForm_.matWorld_.m[3][2];
	return worldPos;
}

void Player::OnCollision()
{

}