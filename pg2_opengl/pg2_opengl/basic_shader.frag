#version 460 core
#define PI 3.1415926538
#extension GL_ARB_bindless_texture : require

out vec4 FragColor;

in vec3 out_normal;
in vec3 out_position;
in vec3 out_ambient;
in vec3 out_diffuse;
in vec3 out_specular;
in vec2 out_tex_coord;

float alpha = 0.025;
const vec2 invAtan = vec2(0.1591, 0.3183);

uniform uvec2 irradianceMap;
uniform uvec2 ggxMap;
uniform uvec2 environmentMap;
uniform vec3 viewPos;
uniform int maxLevel;

struct Material
{
	vec3 diffuse;
    uvec2 tex_diffuse_handle;

	vec3 rma; 
	uvec2 tex_rma_handle;

	vec3 normal;
	uvec2 tex_normal_handle;

};

layout (std430, binding = 0) readonly buffer Materials
{
	Material materials[];
};

float fresnelSchlickRoughness(float cosTheta, float F0)
{
   return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  


vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.y, v.x), asin(v.z));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1.0 - uv.y;
    return uv;
}





vec3 tone_mapping (vec3 color) {
	const float gamma = 2.2;
  
    // reinhard tone mapping
    vec3 mapped = color / (color + vec3(1.0));

    // gamma correct
    mapped = pow(mapped, vec3(1.0 / gamma)); 

	return mapped;
}


void main( void ) {


	vec3 N = normalize(out_normal);
    vec3 V = normalize(viewPos - out_position);
    vec3 R = reflect(-V, N); 



    float F0 = pow((1.0-1.460)/(1.0+1.460), 2.0);
	float kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0);
	float kD = (1.0 - kS)*(1.0 - alpha);	
	vec2 brdf = texture(sampler2D(ggxMap), vec2(max(dot(N, V), 0.0), alpha)).rg;

	vec3 irradiance = texture(sampler2D(irradianceMap), SampleSphericalMap(N)).rgb;
	vec3 prefilteredColor = textureLod( sampler2D(environmentMap), SampleSphericalMap(R), alpha*maxLevel).rgb;
	

	vec3 Ld = (out_diffuse/PI) * irradiance;
	vec3 Ls = prefilteredColor;

	
	vec3 Lo = kD * Ld + (kS * brdf.x + brdf.y) * Ls;
	

	//vec3 Li = ( 0.5*kD * Ld + 0.5*kS);

	vec3 color = /* Li +*/ Lo;

	//gamma correction
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)); 

	FragColor = vec4(color, 1.0); //PBR


}

