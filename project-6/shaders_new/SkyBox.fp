// Shader SkyBox
// Shader fragment�w
// Richard S. Wright Jr.
// OpenGL. Ksi�ga eksperta
#version 130

out vec4 vFragColor;

uniform samplerCube  cubeMap;

varying vec3 vVaryingTexCoord;

void main(void)
{ 
	vFragColor = texture(cubeMap, vVaryingTexCoord);
}
    