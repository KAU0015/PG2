#version 460 core
out vec4 FragColor;

uniform vec3 viewPos;

in vec3 unified_normal_es;
in vec4 hit_es;

void main( void )
{
//	FragColor = vec4( unified_normal_es, 1.0f );

    vec3 lightColor = vec3( 1.0f, 1.0f, 1.0f );
    vec3 objectColor = vec3(0.4f, 0.4f, 0.4f);
    vec3 lightPos = vec3( 40.0f, 40.0f, 50.0f);
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
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}
