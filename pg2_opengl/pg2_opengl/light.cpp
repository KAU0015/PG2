#include "pch.h"
#include "light.h"


Light::Light(const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at, float n, float f)
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	n_ = n;
	f_ = f;

	Update();

}

Vector3 Light::view_from() const
{
	return view_from_;
}


float Light::focal_length() const
{
	return f_y_;
}

void Light::set_fov_y(const float fov_y)
{
	assert(fov_y > 0.0);

	fov_y_ = fov_y;
}

void Light::Update()
{

	f_y_ = height_ / (2.0f * tanf(fov_y_ * 0.5f));

	UpdateView();
	UpdateProjection();
}


void Light::UpdateView()
{
	Vector3 z_c = (view_from_ - view_at_);
	z_c.Normalize();
	Vector3 x_c = up_.CrossProduct(z_c);
	x_c.Normalize();
	Vector3 y_c = z_c.CrossProduct(x_c);
	y_c.Normalize();


	view_ = Matrix4x4(x_c, y_c, z_c, view_from_);
/*	view_ = Matrix4x4(
			0.9969558f, 0.0015221f, 0.0779545f, 0.0f,
			0.0015221f, 0.9992390f, -0.0389772f, 0.0f,
			-0.0779545f, 0.0389772f, 0.9961947f, 1000.0f,
			0.0f, 0.0f, 0.0f, 1.0f);*/
	//printf("%s\n", view_.toString().c_str());
	view_.EuclideanInverse();
	//printf("%s\n", view_.toString().c_str());
}

void Light::UpdateProjection()
{
/*	float aspect = (float)width_ / (float)height_;
	float fov_x = 2.0f * atanf(tanf(fov_y_ / 2.0f) * aspect);

	float h = 2.0f * n_ * tanf(fov_y_ / 2.0f);
	float w = 2.0f * n_ * tanf(fov_x / 2.0f);

	float a = (n_ + f_) / (n_ - f_);
	float b = (2.0f * n_ * f_) / (n_ - f_);

	Matrix4x4 N = Matrix4x4(
		2.0f / w, 0, 0, 0,
		0, 2.0f / h, 0, 0,
		0, 0, 1.0f, 0,
		0, 0, 0, 1.0f
	);

	Matrix4x4 M = Matrix4x4(
		n_, 0, 0, 0,
		0, n_, 0, 0,
		0, 0, a, b,
		0, 0, -1.0f, 0
	);

	//projection_ = N * M;
	projection_ = Matrix4x4(
		1.0f/(tan(fov_x/2.0f)), 0, 0, 0,
		0, 1.0f / (tan(fov_y_ / 2.0f)), 0, 0,
		0, 0, a, b,
		0, 0, -1.0f, 0
	);*/

	float w = 300;
	float h = 300;
	float a = 2.0f / (n_ - f_);
	float b = (f_ + n_) / (n_ - f_);

	projection_ = Matrix4x4(
		2.0f / w, 0, 0, 0,
		0, 2.0f / h, 0, 0,
		0, 0, a, b,
		0, 0, 0, 1);
}

Matrix4x4 Light::view() {
	return view_;
}


Matrix4x4 Light::projection() {
	return projection_;
}


Vector3 Light::viewFrom() {
	return view_from_;
}
