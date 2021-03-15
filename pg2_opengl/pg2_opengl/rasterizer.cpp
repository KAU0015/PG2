#include "pch.h"
#include "rasterizer.h"
#include "matrix4x4.h"
#include "glutils.h"

bool check_gl(const GLenum error = glGetError())
{
	if (error != GL_NO_ERROR)
	{
		//const GLubyte * error_str;
		//error_str = gluErrorString( error );
		//printf( "OpenGL error: %s\n", error_str );

		return false;
	}

	return true;
}

Rasterizer::Rasterizer(const int width, const int height, float fov, Vector3 eye, Vector3 target, float n, float f) {
	width_ = width;
	height_ = height;
	fov_ = fov;
	eye_ = eye;
	target_ = target;
	n_ = n;
	f_ = f;
	camera_ = Camera(width, height,  fov, eye, target, n, f);
	//mouse_ = new Mouse(camera_);
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/* invoked when window is resized */
void framebuffer_resize_clb(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void glfw_callback(const int error, const char* description)
{
	printf("GLFW Error (%d): %s\n", error, description);
}

/* OpenGL messaging callback */
void GLAPIENTRY gl_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user_param)
{
	printf("GL %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "Error" : "Message"),
		type, severity, message);
}

extern void cursor_pos_callback(GLFWwindow * window, double mouseX, double mouseY)
{
	((Camera*)(glfwGetWindowUserPointer(window)))->mouseMovement(window, mouseX, mouseY);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	((Camera*)(glfwGetWindowUserPointer(window)))->mouseClick(window, button, action, mods);
}


int Rasterizer::InitDevice() {
	

	glfwSetErrorCallback(glfw_callback);

	if (!glfwInit())
	{
		return(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

	window = glfwCreateWindow(width_, height_, "PG2 OpenGL", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		if (!gladLoadGL())
		{
			return EXIT_FAILURE;
		}
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(gl_callback, nullptr);

	printf("OpenGL %s, ", glGetString(GL_VERSION));
	printf("%s", glGetString(GL_RENDERER));
	printf(" (%s)\n", glGetString(GL_VENDOR));
	printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	check_gl();

	glEnable(GL_MULTISAMPLE);

	// map from the range of NDC coordinates <-1.0, 1.0>^2 to <0, width> x <0, height>
	glViewport(0, 0, width_, height_);
	// GL_LOWER_LEFT (OpenGL) or GL_UPPER_LEFT (DirectX, Windows) and GL_NEGATIVE_ONE_TO_ONE or GL_ZERO_TO_ONE
	glClipControl(GL_UPPER_LEFT, GL_NEGATIVE_ONE_TO_ONE);

	glfwSetWindowUserPointer(window, (void*)&camera_);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}



void Rasterizer::InitPrograms() {

	vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_shader_source = LoadShader("basic_shader.vert");
	glShaderSource(vertex_shader_, 1, &vertex_shader_source, nullptr);
	glCompileShader(vertex_shader_);
	SAFE_DELETE_ARRAY(vertex_shader_source);
	CheckShader(vertex_shader_);

	fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_shader_source = LoadShader("basic_shader.frag");
	glShaderSource(fragment_shader_, 1, &fragment_shader_source, nullptr);
	glCompileShader(fragment_shader_);
	SAFE_DELETE_ARRAY(fragment_shader_source);
	CheckShader(fragment_shader_);

	shader_program_ = glCreateProgram();
	glAttachShader(shader_program_, vertex_shader_);
	glAttachShader(shader_program_, fragment_shader_);
	glLinkProgram(shader_program_);
	// TODO check linking
	glUseProgram(shader_program_);


	glPointSize(10.0f);
	glLineWidth(2.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// TODO Dopsano přednáška 5
	// Vypnutí vykreslování trianglů, které nemůžou být viděny z pozice camery
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glProvokingVertex(GL_LAST_VERTEX_CONVENTION);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

char* Rasterizer::LoadShader(const char* file_name)
{
	FILE* file = fopen(file_name, "rt");

	if (file == NULL)
	{
		printf("IO error: File '%s' not found.\n", file_name);

		return NULL;
	}

	size_t file_size = static_cast<size_t>(GetFileSize64(file_name));
	char* shader = NULL;

	if (file_size < 1)
	{
		printf("Shader error: File '%s' is empty.\n", file_name);
	}
	else
	{
		shader = new char[file_size + 1];
		memset(shader, 0, sizeof(*shader) * (file_size + 1));

		size_t bytes = 0;

		do
		{
			bytes += fread(shader, sizeof(char), file_size, file);
		} while (!feof(file) && (bytes < file_size));

		if (!feof(file) && (bytes != file_size))
		{
			printf("IO error: Unexpected end of file '%s' encountered.\n", file_name);
		}
	}

	fclose(file);
	file = NULL;

	return shader;
}

GLint Rasterizer::CheckShader(const GLenum shader)
{
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	printf("Shader compilation %s.\n", (status == GL_TRUE) ? "was successful" : "FAILED");

	if (status == GL_FALSE)
	{
		int info_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
		char* info_log = new char[info_length];
		memset(info_log, 0, sizeof(*info_log) * info_length);
		glGetShaderInfoLog(shader, info_length, &info_length, info_log);

		printf("Error log: %s\n", info_log);

		SAFE_DELETE_ARRAY(info_log);
	}

	return status;
}


void Rasterizer::LoadScene(const char* file_name) {


	LoadOBJ(file_name, surfaces_, materials_);

	for (int i = 0; i < surfaces_.size(); i++) {
		Surface* surface = surfaces_.at(i);

		int no_triangles = surface->no_triangles();
		Triangle* triangles = surface->get_triangles();

		for (int j = 0; j < no_triangles; j++) {
			Triangle  triangle = triangles[j];

			for (int k = 0; k < 3; k++) {
				Vertex vertex = triangle.vertex(k);
				number_of_verticies_++;

				// Push position data
				object_data_.push_back(vertex.position.x);
				object_data_.push_back(vertex.position.y);
				object_data_.push_back(vertex.position.z);


				object_data_.push_back(vertex.normal.x);
				object_data_.push_back(vertex.normal.y);
				object_data_.push_back(vertex.normal.z);

				// Push texture coords
				//vertex_data.push_back(vertex.texture_coords->u);
				//vertex_data.push_back(vertex.texture_coords->v);
			}
		}
	}


	//number_of_verticies_ = object_data_.size() / 3;
	vertex_stride_ = (object_data_.size() * sizeof(GLfloat)) / number_of_verticies_;
	vertices_length_ = object_data_.size() * sizeof(GLfloat);

}


void Rasterizer::InitBuffers() {
	vao_ = 0;
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	vbo_ = 0;
	glGenBuffers(1, &vbo_); // generate vertex buffer object (one of OpenGL objects) and get the unique ID corresponding to that buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo_); // bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBufferData(GL_ARRAY_BUFFER, vertices_length_, object_data_.data(), GL_STATIC_DRAW); // copies the previously defined vertex data into the buffer's memory
	// vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertex_stride_, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertex_stride_, (void*)(sizeof(GLfloat)*3));

	glEnableVertexAttribArray(1);
}

void Rasterizer::MainLoop() {

	

	// Turn off VSYNC
	glfwSwapInterval(0);
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	Matrix4x4 model = Matrix4x4();
	Matrix4x4 normal = Matrix4x4();

	//camera_.Update();

	// main loop
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwGetKey(window, GLFW_KEY_Q) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		float deltaTime = currentTime - lastTime;

		if ( deltaTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			std::printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}


		camera_.processInput(window, deltaTime);
		camera_.Update();
		

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // sc_str()tate setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // state using function

		glUseProgram(shader_program_);

		Matrix4x4 mvp = camera_.projection() * camera_.view() * model;

		SetMatrix4x4(shader_program_, mvp.data(), "mvp");
		SetMatrix4x4(shader_program_, normal.data(), "mvn");
		SetMatrix4x4(shader_program_, model.data(), "mv");
		SetVector3(shader_program_, camera_.viewFrom(), "viewPos");

		glBindVertexArray(vao_);
		glBindVertexArray(vbo_);

		//Draw triangles
		glDrawArrays(GL_TRIANGLES, 0, number_of_verticies_ * 3);
	//	glDrawArrays(GL_POINTS, 0, number_of_verticies_*3);
		//glDrawArrays(GL_LINE_LOOP, 0, number_of_verticies_ * 3);
		//glDrawElements( GL_TRIANGLES, no_of_verticies * 3, GL_UNSIGNED_INT, 0 ); // optional - render from an index buffer

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteShader(vertex_shader_);
	glDeleteShader(fragment_shader_);
	glDeleteProgram(shader_program_);

	glDeleteBuffers(1, &vbo_);
	glDeleteVertexArrays(1, &vao_);


	glfwTerminate();

	printf("Safely terminated.");
	
}



