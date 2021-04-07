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

in vec3 position_lcs;
uniform sampler2D shadow_map;

const vec2 invAtan = vec2(0.1591, 0.3183);

uniform uvec2 irradianceMap;
uniform uvec2 ggxMap;
uniform uvec2 environmentMap;
uniform vec3 viewPos;
uniform int maxLevel;


flat in int out_material_index;
in mat3 out_TBN;

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

float saturate(in float value)
{
    return clamp(value, 0.0, 1.0);
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


	vec3 albedo = texture(sampler2D(materials[out_material_index].tex_diffuse_handle), out_tex_coord ).rgb * materials[out_material_index].diffuse;

	vec3 norm = normalize(out_normal);
	if(materials[out_material_index].normal.x < 0)
		norm = normalize(out_TBN * normalize(texture(sampler2D(materials[out_material_index].tex_normal_handle), out_tex_coord ).rgb * 2.0 - 1.0));


	vec4 rmaTexture = texture(sampler2D(materials[out_material_index].tex_rma_handle), out_tex_coord );
	float roughness = rmaTexture.r * materials[out_material_index].rma.x;
	float metallicity = rmaTexture.g * materials[out_material_index].rma.y;
	roughness = saturate(roughness); 
	float ior = materials[out_material_index].rma.z;
	float ambientOcclusion =  saturate(rmaTexture.b);


	vec3 N = norm;
    vec3 V = normalize(viewPos - out_position);
    vec3 R = reflect(-V, N); 

	float bias = 0.00015f;

	vec2 shadow_texel_size = 1.0f / textureSize( shadow_map, 0 ); // size of a single texel in tex coords
	const int r = 5; // search radius, try different values
	float shadow = 0.0f; // cumulative shadowing coefficient
	for ( int y = -r; y <= r; ++y ) {
		for ( int x = -r; x <= r; ++x ) {
			vec2 a_tc = ( position_lcs.xy + vec2( 1.0f ) ) * 0.5f;
			a_tc += vec2( x, y ) * shadow_texel_size;
			float depth = texture( shadow_map, a_tc ).r;
			depth = depth * 2.0f - 1.0f;
			shadow += ( (depth + bias) >= position_lcs.z )? 1.0f : 0.25f;
		}
	}
	shadow *= ( 1.0f / ( ( 2 * r + 1 ) * ( 2 * r + 1 ) ) );

    float F0 = pow((1.0-ior)/(1.0+ior), 2.0);
	float kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0);
	float kD = (1.0 - kS)*(1.0 - metallicity);	
	vec2 brdf = texture(sampler2D(ggxMap), vec2(max(dot(N, V), 0.0), roughness)).rg;

	vec3 irradiance = texture(sampler2D(irradianceMap), SampleSphericalMap(N)).rgb;
	vec3 prefilteredColor = textureLod( sampler2D(environmentMap), SampleSphericalMap(R), roughness*maxLevel).rgb;
	

	vec3 Ld = (albedo/PI) * irradiance;
	vec3 Ls = prefilteredColor;

	
	vec3 Lo =( kD * Ld + (kS * brdf.x + brdf.y) * Ls) * ambientOcclusion;
	

	//vec3 Li = ( 0.5*kD * Ld + 0.5*kS);

	vec3 color = /* Li +*/ Lo*shadow;

	//gamma correction
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2)); 

	FragColor = vec4(color, 1.0); //PBR

//	FragColor = vec4(roughness, roughness, roughness, 1.0); 

//	FragColor = vec4( vec3(texture( shadow_map, gl_FragCoord.xy / 640.0f ).r ), 1.0f );


}

