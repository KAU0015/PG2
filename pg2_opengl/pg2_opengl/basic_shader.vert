#version 460 core
#define PI 3.1415926538

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 ambient;
layout (location = 3) in vec3 diffuse;
layout (location = 4) in vec3 specular;
layout (location = 5) in vec2 tex_coord;
//layout (location = 6) in vec3 tangent;

uniform mat4 mvp;
uniform mat4 mvn;
uniform mat4 mv;
uniform vec3 viewPos;


out vec3 out_normal;
out vec3 out_position;
out vec3 out_ambient;
out vec3 out_diffuse;
out vec3 out_specular;
out vec2 out_tex_coord;



void main( void )
{
	gl_Position = mvp * vec4(position, 1.0);

	out_normal = normalize(( mvn * vec4( normal.xyz, 1.0f ) ).xyz);
	out_position = (mv * vec4(position, 1.0)).xyz;

	vec3 omega_o = normalize(viewPos - out_position);
	float cos_theta_o = dot(out_normal, omega_o);

	out_normal = (cos_theta_o < 0.0) ? -out_normal : out_normal;

	out_tex_coord = tex_coord;
	out_ambient = ambient;
	out_diffuse= diffuse;
	out_specular = specular;
}