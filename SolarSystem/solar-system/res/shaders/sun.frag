#version 330 core
in vec2 TextCoord;  

out vec4 frag_color; 
uniform vec3 lightColor; 
uniform sampler2D lightTexture; 

void main()
{
    // Sample the texture using texture coordinates
    vec4 textureColor = texture(lightTexture, TextCoord);
    
    // Multiply the texture color with the light color
    frag_color = vec4(textureColor.rgb * lightColor, 1.0f); 
}


