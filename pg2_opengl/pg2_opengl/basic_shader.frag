#version 460 core
out vec4 FragColor;

uniform vec3 viewPos;

in vec3 unified_normal_es;
in vec4 hit_es;
in vec3 out_ambient;
in vec3 out_diffuse;
in vec3 out_specular;


void main( void )
{
//	FragColor = vec4( unified_normal_es, 1.0f );

   /* vec3 lightColor = vec3( 1.0f, 1.0f, 1.0f );
    vec3 lightPos = vec3( 200.0f, -305.0f, 100.0f);

    vec3 viewDir = normalize(viewPos - hit_es.xyz);

    vec3 norm = normalize(unified_normal_es);
    vec3 lightDir = normalize(lightPos- hit_es.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);


    vec3 ambient = out_ambient;
    vec3 diffuse = out_diffuse * diff;
    vec3 specular = out_specular * spec;

    vec3 result = (ambient + diffuse + specular) * lightColor;*/


    vec3 norm = normalize(unified_normal_es);
    vec3 lightColor = vec3( 1.0f, 1.0f, 1.0f );
    vec3 lightPos = vec3( 200.0f, -250.0f, 200.0f);

    vec3 viewDir = normalize(viewPos - hit_es.xyz);
	vec3 v_light = normalize(lightPos - hit_es.xyz);
	float dotNormalLight = dot(norm, v_light);

	vec3 v_lightRefl = (2.0f * dotNormalLight) * norm - v_light;
	float specularComponent = pow(max(dot(viewDir, v_lightRefl), 0), 32);

	vec3 diffuseC = out_diffuse * dotNormalLight;
	vec3 specularC = out_specular * specularComponent;
	vec3 ambientC = out_ambient;

	vec3 result = lightColor * (ambientC + (diffuseC + specularC));


    /*
	// ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(unified_normal_es);
    vec3 lightDir = normalize(lightPos - hit_es.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - hit_es.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
        
    vec3 result = (ambient + diffuse + specular) * out_color;*/
    FragColor = vec4(result, 1.0);
}
