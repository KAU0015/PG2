#version 460 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvp;
uniform mat4 mvn;
uniform mat4 mv;
out vec3 unified_normal_es;
out vec4 hit_es;


void main( void )
{
	gl_Position = mvp * position;

	unified_normal_es = normalize(( mvn * vec4( normal.xyz, 0.0f ) ).xyz);
//	unified_normal_es = transpose(inverse(mat3(mv))) * normal;

/*	vec4 hit_es = mv * position;
	vec3 omega_i_es = normalize( hit_es.xyz / hit_es.w );

	if ( dot( unified_normal_es, omega_i_es ) > 0.0f )
	{
		unified_normal_es *= -1.0f;
	}*/
}
