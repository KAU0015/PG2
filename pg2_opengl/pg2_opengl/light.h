#pragma once

#include "vector3.h"
#include "matrix3x3.h"
#include "matrix4x4.h"


class Light
{
public:
	Light() { }

	Light(const int width, const int height, const float fov_y,
		const Vector3 view_from, const Vector3 view_at, float n, float f);

	Vector3 view_from() const;
	Matrix3x3 M_c_w() const;
	float focal_length() const;

	void set_fov_y(const float fov_y);

	void Update();
	void UpdateView();
	void UpdateProjection();

	Matrix4x4 projection();
	Matrix4x4 view();

	Vector3 viewFrom();


private:
	int width_{ 640 }; // image width (px)
	int height_{ 480 };  // image height (px)
	float fov_y_{ 0.785f }; // vertical field of view (rad)

	Vector3 view_from_; // ray origin or eye or O
	Vector3 view_at_; // target T
	Vector3 up_{ Vector3(0.0f, 0.0f, 1.0f) }; // up vector

	float f_y_{ 1.0f }; // focal lenght (px)

	float n_;
	float f_;

	Matrix4x4 view_;
	Matrix4x4 projection_;
};


