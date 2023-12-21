#version 330 core
out vec4 FragColor;

in vec2 TextCoord;

// Add a uniform sampler2D for your texture
uniform sampler2D introSampler;

void main()
{
    // Sample the texture and set the fragment color
    FragColor = texture(introSampler, TextCoord);
}
