#include "pch.h"
#include "tutorials.h"
#include "utils.h"
#include "rasterizer.h"

float degTorad(float degree)
{
	float pi = 3.14159265358979323846f;
	return (degree * (pi / 180.0f));
}

/* create a window and initialize OpenGL context */
int tutorial_1( const int width, const int height )
{
	Rasterizer rasterizer(1600, 900, degTorad(45.0f), Vector3(200, 100, 100), Vector3(0, 0.0f, 30.0f), 0.1f, 2000.0f);
	//Rasterizer rasterizer(1600, 900, degTorad(45.0f), Vector3(40, -30, 50), Vector3(0, 0, 0), 0.1f, 1000.0f);
	rasterizer.InitDevice();
	rasterizer.InitPrograms();
	rasterizer.InitShadowPrograms();
	rasterizer.InitShadowDepthbuffer();
	//rasterizer.LoadScene("../../data/6887_allied_avenger/6887_allied_avenger_gi2.obj");
	//rasterizer.LoadScene("../../data/test_scene_whitted.obj");
	rasterizer.LoadScene("../../data/piece_02/piece_02.obj");
	rasterizer.InitBuffers();
	rasterizer.InitIrradianceMap("../../data/lebombo_irradiance_map.exr");
	rasterizer.InitPrefilteredEnvMap({
		"../../../pg2/data/lebombo_prefiltered_env_map_001_2048.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_010_1024.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_100_512.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_250_256.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_500_128.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_750_64.exr",
		"../../../pg2/data/lebombo_prefiltered_env_map_999_32.exr" });
	rasterizer.InitGGXIntegrMap("../../data/brdf_integration_map_ct_ggx.png");
	rasterizer.MainLoop();

	return EXIT_SUCCESS;
}
