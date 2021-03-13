#include "pch.h"
#include "camera.h"


Camera::Camera( const int width, const int height, const float fov_y,
	const Vector3 view_from, const Vector3 view_at, float n, float f )
{
	width_ = width;
	height_ = height;
	fov_y_ = fov_y;

	view_from_ = view_from;
	view_at_ = view_at;

	n_ = n;
	f_ = f;

	// TODO compute focal lenght based on the vertical field of view and the camera resolution
	
	// TODO build M_c_w_ matrix

	Update();
}

Vector3 Camera::view_from() const
{
	return view_from_;
}

Matrix3x3 Camera::M_c_w() const
{
	return M_c_w_;
}

float Camera::focal_length() const
{
	return f_y_;
}

void Camera::set_fov_y( const float fov_y )
{
	assert( fov_y > 0.0 );

	fov_y_ = fov_y;
}

void Camera::Update()
{
	f_y_ = height_ / ( 2.0f * tanf( fov_y_ * 0.5f ) );

	Vector3 z_c = view_from_ - view_at_;
	z_c.Normalize();
	Vector3 x_c = up_.CrossProduct( z_c );
	x_c.Normalize();
	Vector3 y_c = z_c.CrossProduct( x_c );
	y_c.Normalize();
	M_c_w_ = Matrix3x3( x_c, y_c, z_c );

	UpdateView();
	UpdateProjection();
}

void Camera::MoveForward( const float dt )
{
	Vector3 ds = view_at_ - view_from_;
	ds.Normalize();
	ds *= dt;

	view_from_ += ds;
	view_at_ += ds;
}

void Camera::UpdateView() 
{
	Vector3 z_c = view_from_ - view_at_;
	z_c.Normalize();
	Vector3 x_c = up_.CrossProduct(z_c);
	x_c.Normalize();
	Vector3 y_c = z_c.CrossProduct(x_c);
	y_c.Normalize();
	

	view_ = Matrix4x4(x_c, y_c, z_c, view_from_);
	view_.EuclideanInverse();
}

void Camera::UpdateProjection()
{
	float aspect = (float)width_ / (float)height_;
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

	projection_ = N * M;
}

Matrix4x4 Camera::view() {
	return view_;
}


Matrix4x4 Camera::projection() {
	return projection_;
}


Vector3 Camera::viewFrom() {
	return view_from_;
}
