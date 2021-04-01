#ifndef CAMERA_H_
#define CAMERA_H_

#include "vector3.h"
#include "matrix3x3.h"
#include "matrix4x4.h"


class Camera
{
public:
	Camera() { }

	Camera( const int width, const int height, const float fov_y,
		const Vector3 view_from, const Vector3 view_at, float n, float f);

	Vector3 view_from() const;
	Matrix3x3 M_c_w() const;
	float focal_length() const;

	void set_fov_y( const float fov_y );

	void Update();
	void Update(int width, int height);
	void UpdateView();
	void UpdateProjection();

	void MoveForward( const float dt );
	void MoveBackward(const float dt);
	void MoveLeft(const float dt);
	void MoveRight(const float dt);

	Matrix4x4 projection();
	Matrix4x4 view();

	Vector3 viewFrom();

	void setViewAt(Vector3 view_at);

	void mouseMovement(GLFWwindow* window, double mouse_x, double mouse_y);
	void mouseClick(GLFWwindow* window, int button, int action, int mods);
	void processInput(GLFWwindow* window, float deltaTime);
	void updateCameraVectors();
	int width();
	int height();

private:
	int width_{ 640 }; // image width (px)
	int height_{ 480 };  // image height (px)
	float fov_y_{ 0.785f }; // vertical field of view (rad)
	
	Vector3 view_from_; // ray origin or eye or O
	Vector3 view_at_; // target T
	Vector3 up_{ Vector3( 0.0f, 0.0f, 1.0f ) }; // up vector

	float f_y_{ 1.0f }; // focal lenght (px)

	float n_;
	float f_;

	Matrix3x3 M_c_w_; // transformation matrix from CS -> WS	

	Matrix4x4 view_;
	Matrix4x4 projection_;

	bool first_mouse_ = true;
	double last_x_, last_y_;
	float pitch_ = 0.0f; //-90
	bool clicked_ = false;
	float yaw_ = 0.0f; //0
//	float clicked_x_, clicked_y_;
	bool clicked_left_ = false;

	bool rotated_ = false;
};

#endif
