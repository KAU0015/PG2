#version 460 core

layout ( location = 0 ) in vec3 in_position_ms;

// Projection (P_l)*Light (V_l)*Model (M) matrix
uniform mat4 mlp;

void main( void )
{
	gl_Position = mlp * vec4(in_position_ms, 1.0);
}