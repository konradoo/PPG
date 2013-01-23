#version 330

struct PointLight 
{
    vec3  pos; //in eye space !!
    vec3  color;
};

////////////////////////////////////////////////////////////////////////////////
// uniforms
uniform PointLight light0;

////////////////////////////////////////////////////////////////////////////////
// input
smooth in vec3 eyeNormal;
smooth in vec3 fromPoint2Light;

////////////////////////////////////////////////////////////////////////////////
// output
out vec4 vFragColor;

void main() 
{	
    vec3 lightRef = normalize(reflect(-fromPoint2Light, eyeNormal));
    
    
// calculate diffuse color   
    float diffuse = max(0.0, dot(normalize(eyeNormal), normalize(fromPoint2Light))); // fromPoint2Light i normal musz¹ byæ znormalizowane!
	//vec3 diffuseColor = max(diffuse, 0.0f) * light0.color;
    vec3 diffuseColor = diffuse * light0.color; //mod
    
    float distance = length(fromPoint2Light);
    
    
// calculate specular:
	float specular = 0;    
    vec3 specularColor = vec3(0.0f);
    
	
	if (diffuse > 0.0f)
	{		
	    float specular = pow(max(0.0, dot(vec3(0,0,1), lightRef)), 64.0f);
	    
	    specularColor = vec3(1.0f) * max(specular, 0.0f); // bia³e œwiat³o odbicia
	}	
	
	vFragColor = (vec4(0.2f) + vec4(diffuseColor + specularColor, 1.0f)) /(1 + distance*0.045 + pow(distance, 2) * 0.0075);
}