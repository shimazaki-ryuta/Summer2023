#include "Vector3.h"
#include "CommonFiles/WinApp.h"
#include "VectorFunction.h"
#include "RailCamera.h"
#include "MatrixFunction.h"
#include <algorithm>
#include "Curve.h"

void RailCamera::Initialize(const Vector3& position, const Vector3& rotation) {
	worldTransForm_.translation_ = position;
	worldTransForm_.rotation_ = rotation;
	viewProjection_.farZ = 2000.0f;
	viewProjection_.Initialize(1280,720);
	worldTransForm_.Initialize();
	t = 0;
}

void RailCamera::Update()
{
	Vector3 eye, target;

	eye = CatmullRomComplement(controlPoints_, t);
	target = CatmullRomComplement(controlPoints_, t + (1.0f / divisionRailCount));
	//worldTransForm_.translation_.z+=0.1f;
	//worldTransForm_.rotation_.y += 0.005f;
	//worldTransForm_.translation_.z = std::clamp(worldTransForm_.translation_.z, -100.0f, 100.0f);
	Vector3 foward = target - eye;

	worldTransForm_.translation_ = eye;
	//  y軸周り角度(θy)
	worldTransForm_.rotation_.y = std::atan2(foward.x, foward.z);
	Vector3 rectXZ{ foward.x, 0.0f, foward.z };
	float besage = Length(rectXZ);
	worldTransForm_.rotation_.x = std::atan2(-foward.y, besage);

	// 行列を更新
	worldTransForm_.UpdateMatrix();
	viewProjection_.matView = Inverse(worldTransForm_.matWorld_);
	//t += 1.0f / divisionRailCount;
	//t = std::clamp(t,0.0f,1.0f);
}