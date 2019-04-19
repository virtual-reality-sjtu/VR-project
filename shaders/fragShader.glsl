#version 330

in vec3 LightIntensity;
uniform sampler2D texture1;
uniform int useDiffuseTex;

layout (location = 0) out vec4 FragColor;

in vec2 TexCoord;

void main()
{
    FragColor = vec4(LightIntensity, 1.0);
	if(bool(useDiffuseTex))
	{ 
		FragColor = vec4(texture(texture1, TexCoord));
	}
}
