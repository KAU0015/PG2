#include "pch.h"
#include "tutorials.h"
#include "utils.h"
#include "rasterizer.h"

float deg2rad(float degree)
{
	float pi = 3.14159265358979323846f;
	return (degree * (pi / 180.0f));
}

/* create a window and initialize OpenGL context */
int tutorial_1( const int width, const int height )
{
	//Rasterizer rasterizer(640, 480, deg2rad(45.0f), Vector3(200, 100, 100), Vector3(0, 0, 30), 0.1f, 1000.0f);
	Rasterizer rasterizer(640, 480, deg2rad(45.0f), Vector3(40, -30, 50), Vector3(0, 0, 0), 0.1f, 1000.0f);
	rasterizer.InitDevice();
	rasterizer.InitPrograms();
	//rasterizer.LoadScene("../../data/6887_allied_avenger/6887_allied_avenger_gi2.obj");
	rasterizer.LoadScene("../../data/piece_02/piece_02.obj");
	rasterizer.InitBuffers();
	rasterizer.MainLoop();

	return EXIT_SUCCESS;
}
