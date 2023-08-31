#include "EnemyBullet.h"
#include <assert.h>
#include "VectorFunction.h"
#include "MatrixFunction.h"
#include "Player.h"
void EnemyBullet::Initialize(const Vector3& position, const Vector3& velocity) {
	
	//textureHandle_ = TextureManager::Load("white1x1.png");
	worldTransForm_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransForm_.translation_ = position;
	worldTransForm_.scale_.x = 0.5f;
	worldTransForm_.scale_.y = 0.5f;
	worldTransForm_.scale_.z = 1.0f;

	velocity_ = velocity;
	//解法2
	//  y軸周り角度(θy)
	worldTransForm_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{ velocity_.x, 0.0f, velocity_.z };
	float besage = Length(velocityXZ);
	worldTransForm_.rotation_.x = std::atan2(-velocity_.y, besage);

	SetCollisionAttribute(kCollisionAttributeEnemy);
	SetCollisionMask(~kCollisionAttributeEnemy);
	model_ = Primitive3D::Create(Vector2{ 0.0f,0.0f }, Vector2{ 640.0f,360.0f }, Vector4{ 1.0f,0.0f,0.0f,1.0f });
}

void EnemyBullet::Update()
{
	Vector3 toPlayer = player_->GetWorldPosition() - worldTransForm_.translation_;

	float speed = 0.3f;

	velocity_ = Slerp(velocity_, toPlayer, 0.05f) * speed;

	// y軸周り角度(θy)
	worldTransForm_.rotation_.y = std::atan2(velocity_.x, velocity_.z);
	Vector3 velocityXZ{ velocity_.x, 0.0f, velocity_.z };
	float besage = Length(velocityXZ);
	worldTransForm_.rotation_.x = std::atan2(-velocity_.y, besage);

	worldTransForm_.translation_ += velocity_;
	worldTransForm_.UpdateMatrix();

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	Matrix4x4 worldViewProjectionMatrix = worldTransForm_.matWorld_ * viewProjection.matView * viewProjection.matProjection;
	model_->Draw(worldViewProjectionMatrix, Vector4{ 1.0f,0.0f,0.0f,1.0f });

}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos;
	worldPos.x = worldTransForm_.matWorld_.m[3][0];
	worldPos.y = worldTransForm_.matWorld_.m[3][1];
	worldPos.z = worldTransForm_.matWorld_.m[3][2];
	return worldPos;
}


void EnemyBullet::OnCollision() { isDead_ = true; }