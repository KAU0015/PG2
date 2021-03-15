#include "pch.h"
#include "camera.h"


float deg2rad(float degree)
{
	float pi = 3.14159265358979323846f;
	return (degree * pi / 180.0f);
}

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

	updateCameraVectors();
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

void Camera::MoveBackward(const float dt)
{
	Vector3 ds = view_at_ - view_from_;
	ds.Normalize();
	ds *= dt;

	view_from_ -= ds;
	view_at_ -= ds;
}

void Camera::MoveLeft(const float dt)
{
	Vector3 ds = (view_at_ - view_from_).CrossProduct(up_);
	ds.Normalize();
	ds *= dt;

	view_from_ -= ds;
	view_at_ -= ds;
}

void Camera::MoveRight(const float dt)
{
	Vector3 ds = (view_at_ - view_from_).CrossProduct(up_);
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

void Camera::setViewAt(Vector3 view_at) {
	view_at_ = view_at;
}



void Camera::mouseMovement(GLFWwindow* window, double mouseX, double mouseY)
{

	if (first_mouse_)
	{
		last_x_ = mouseX;
		last_y_ = mouseY;
		first_mouse_ = false;
	}

	double xOffSet = mouseX - last_x_;
	double yOffSet = last_y_ - mouseY;
	last_x_ = mouseX;
	last_y_ = mouseY;

	if (clicked_)
	{


		float sensitivtiy = 0.05f;
		xOffSet *= sensitivtiy;
		yOffSet *= sensitivtiy;

		yaw_ += xOffSet;
		pitch_ += yOffSet;
		if (pitch_ > 89.0f)
		{
			pitch_ = 89.0f;
		}
		if (pitch_ < -89.0f)
		{
			pitch_ = -89.0f;
		}
		updateCameraVectors();

	}
	else if (clicked_left_)
	{

	}

}

void Camera::mouseClick(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		printf("clicked right\n");
		clicked_ = true;
		//camera->changeDirection(0.5f,0.5f);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		clicked_ = false;
		printf("released right\n");
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{

	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{


	}
}

void Camera::processInput(GLFWwindow* window, float deltaTime)
{
	float velocity = 0.25f/* * deltaTime*/;


	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveForward(velocity);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveBackward(velocity);
	}
		
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		MoveLeft(velocity);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		MoveRight(velocity);
	}
		
}


void Camera::updateCameraVectors()
{

	Vector3 vFrom = Vector3{ 0, 0 , 0 };
	Vector3 vAt = view_at_ - view_from_;
	Vector3 dir = vAt - vFrom;
	float pt = -deg2rad(pitch_);
	float yaw = -deg2rad(yaw_);

	Matrix3x3 xRot = { 1.0f, 0.0f, 0.0f, 0.0f, cosf(pt), -sinf(pt), 0.0f, sinf(pt), cosf(pt) };
	Matrix3x3 zRot = { cosf(yaw), -sinf(yaw), 0.0f, sinf(yaw), cosf(yaw), 0.0f, 0.0f, 0.0f, 1.0f };

	float distance = sqrt(pow(vAt.x - vFrom.x, 2.0) + pow(vAt.y - vFrom.y, 2.0) + pow(vAt.z - vFrom.z, 2.0));

	Vector3 result = zRot * xRot * Vector3(0.0f, distance, 0.0f);

	view_at_ = result + view_from_;
}