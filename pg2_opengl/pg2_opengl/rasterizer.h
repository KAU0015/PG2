#pragma once

#include "pch.h"
#include "vector3.h"
#include "camera.h"
#include "utils.h"
#include "surface.h"
#include "triangle.h"
#include "objloader.h"
#include "glutils.h"
#include "mymath.h"
#include "vertex.h"
#include "texture.h"
#include "light.h"

using namespace std;


class Rasterizer {

public:
	Rasterizer(const int width, const int height, float fov, Vector3 eye, Vector3 target, float n, float f);

	int InitDevice();
	void InitPrograms();
	void InitShadowPrograms();
	void LoadScene(const char* file_name);
	void InitBuffers();
	void InitMaterials(int material);
	void InitIrradianceMap(const char* file_name); // inicializace předpočítaných map pro Cook-Torrance GGX shader 
	void InitPrefilteredEnvMap(vector<const char*> file_names);
	void InitGGXIntegrMap(const char* file_name);
	void MainLoop();

	char* LoadShader(const char* file_name);
	GLint CheckShader(const GLenum shader);

	void  CreateBindlessTexture(GLuint& texture, GLuint64& handle, const int width, const int height, const GLvoid* data);
	void InitShadowDepthbuffer();
	int Resize(const int width, const int height);

	//Mouse* mouse_;

	//void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	//void cursor_pos_callback(GLFWwindow* window, double mouseX, double mouseY);

private:
	int width_;
	int height_;
	float fov_;
	Vector3 eye_;
	Vector3 target_;
	float n_;
	float f_;
	Camera camera_;
	GLuint shader_program_;
	GLuint vertex_shader_;
	GLuint fragment_shader_;

	GLuint vao_ = 0;
	GLuint vbo_ = 0;
	GLuint ebo_ = 0;

	GLuint64 handle_brdf_map_{ 0 };
	GLuint64 handle_env_map_{ 0 };
	GLuint64 handle_ir_map_{ 0 };

	GLuint tex_brdf_map_{ 0 };
	GLuint tex_env_map_{ 0 };
	GLuint tex_ir_map_{ 0 };

	int shadow_width_{ 2048 }; // shadow map resolution
	int shadow_height_{ shadow_width_ };
	GLuint fbo_shadow_map_{ 0 }; // shadow mapping FBO
	GLuint tex_shadow_map_{ 0 }; // shadow map texture
	GLuint shadow_program_{ 0 }; // collection of shadow mapping shaders
	GLuint shadow_vertex_shader_;
	GLuint shadow_fragment_shader_;

	Light light_;

	GLFWwindow* window;
	int no_triangles;
	vector<Surface*> surfaces_;
	vector<Material*> materials_;

	vector<GLfloat> object_data_;

	int vertices_length_ = 0;
	int number_of_verticies_ = 0;
	int vertex_stride_ = 0;





};