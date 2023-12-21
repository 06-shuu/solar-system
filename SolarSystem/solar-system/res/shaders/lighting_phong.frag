
#version 330 core

in vec2 TextCoord; 
in vec3 FragPos; 
in vec3 Normal;

out vec4 frag_color;

uniform sampler2D texSampler1;
uniform vec3 lightPose;; 
uniform vec3 lightColor; 
uniform vec3 viewPos; 

void main()
{
	//we get the equations of ambient, diffuse and specular from the slides

	//Ambient
	float ambientFactor = 0.25f; //play here (it was 0.1)
	vec3 ambient = lightColor * ambientFactor;
	
	//Diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPose - FragPos);
	float NDotL = max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = lightColor * NDotL;

	//specular
	float speculatFactor = 0.3f; 
	float shininess = 32.0f; 
	vec3 viewDir = normalize(viewPos - FragPos); 

	//Phong:
	//vec3 reflectDir = reflect(-lightDir, normal);//- bc the reflected ligth comes in the opposite side
	//float RdotV = max(dot(reflectDir,viewDir), 0.0f ); 
	//vec3 specular = lightColor * speculatFactor * pow(RdotV, shininess);

	// OR

	//Blinn-Phong:
	vec3 halfDir = normalize(lightDir + viewDir); 
	float NDotH = max(dot(normal, halfDir), 0.0f);
	vec3 specular = lightColor * speculatFactor * pow(NDotH, shininess);

	 vec4 texel = texture(texSampler1, TextCoord);
	 frag_color = vec4(ambient+diffuse+specular, 1.0f)* texel;
};
