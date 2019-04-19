#version 330

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 texCoords;
layout (location = 5) in vec3 color;

out vec3 LightIntensity;
out vec2 TexCoord;

struct LightInfo
{
    vec4 Position; //ligth position in eye coords
    vec3 La; //ambient light intensity
    vec3 Ld; //diffuse light intensity
    vec3 Ls; // specular light intensity
};
uniform LightInfo Light;

struct MaterialInfo
{
    vec3 Ka; // ambient reflectivity
    vec3 Kd; // diffuse reflectivity
    vec3 Ks; // specular reflectivity
    float Shininess; // specular shininess factor
};
uniform MaterialInfo Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
//uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	TexCoord = texCoords;
    vec3 tnorm = normalize(NormalMatrix * VertexNormal);
    vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition, 1.0);
    vec3 s = normalize(vec3(Light.Position - eyeCoords));
    vec3 v = normalize(-eyeCoords.xyz);
    vec3 r = reflect(-s, tnorm);
    float sDotN = max(0.0, dot(s, tnorm));
    vec3 ambient = Light.La * Material.Ka;
    vec3 diffuse = Light.Ld * Material.Kd * sDotN;
    vec3 spec = vec3(0.0);
    if(sDotN > 0.0)
    {
        spec = Light.Ls * Material.Ks *
                pow(max(0.0,dot(r, v)), Material.Shininess);
    }
    LightIntensity = ambient + diffuse + spec;
    gl_Position = MVP *vec4(VertexPosition, 1.0);
}
