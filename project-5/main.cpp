#include <GLTools.h>

#define FREEGLUT_STATIC
#include "glut.h"
#include "freeglut_ext.h"

#include "SOIL.h"
#include <vector>

using namespace std;
#pragma region globals

GLuint phongShader;

GLuint uniformMatModelview[2];
GLuint uniformMatProjection[2];
GLuint uniformMatNormal[2];

M3DMatrix44f matProjection;
M3DMatrix44f matModelview;
M3DMatrix33f matNormal;

M3DVector3f lightPos = { 0.0f };
M3DVector3f lightColor = { 1.0f, 0.0f, 0.0f };
GLuint uniformLightPos[2];
GLuint uniformLightColor[2];

GLuint texID;

M3DVector3f camPos = { 0.0f, 0.0f, 8.0 };
M3DVector3f camVel = { 0.0f };

bool animationOn = true;
float animRings = true;
float animParam = 0.0f;

const int ELEMENT_COUNT = 3+3+2;
vector<float> vertices;
GLuint vertexBufferID = 0;

const int NUM_SIDES = 8;
const float RAD = 1.0f;
const float HEIGHT = 0.05f;
const float DELTA_ANG = (float)M3D_PI * 2.0f / (float)NUM_SIDES;
const float DU = 1.0f/(float)NUM_SIDES; 
const int NUM_RINGS = 24;
const float H_SCALE = 1.5f;
const float Y_START = (float)NUM_RINGS*HEIGHT*H_SCALE;

bool wire = false;
int activeShader = 0;
float angle_x = 0.0f;
float angle_y = 0.0f;
enum shader{FRAGMENT, VERTEX};
float dist=5;

#pragma endregion


#pragma region input
void PressNormalKeys(unsigned char key, int x, int y) 
{

}

void ReleaseNormalKeys(unsigned char key, int x, int y) 
{
    if (key == ' ')
        animationOn = !animationOn;
    else if(key == 'w')
        wire = !wire;
    else if (key == 13)
    {
        camPos[0] = 0.0f;
        camPos[1] = 0.0f;
        camPos[2] = 5.0f;
    }
	else if (key == 27) 
		exit(0);
	else if (key == 'k')
		dist += 2;
	else if (key == 'l')
		dist -= 2;
}

void PressSpecialKey(int key, int x, int y) 
{
    if (key == GLUT_KEY_UP)
        camVel[2] = -0.1f;
    else if (key == GLUT_KEY_DOWN)
        camVel[2] = 0.1f;
    else if (key == GLUT_KEY_LEFT)
        camVel[0] = -0.1f;
    else if (key == GLUT_KEY_RIGHT)
        camVel[0] = 0.1f;
}

void ReleaseSpecialKey(int key, int x, int y) 
{
    if (key == GLUT_KEY_UP)
        camVel[2] = 0.0;
    else if (key == GLUT_KEY_DOWN)
        camVel[2] = 0.0;
    else if (key == GLUT_KEY_LEFT)
        camVel[0] = 0.0f;
    else if (key == GLUT_KEY_RIGHT)
        camVel[0] = 0.0f;
}

#pragma endregion

#pragma region init and cleanup

void CreateVertexBuffers()
{    
    float ang;
    vertices.clear();
    for (int i = 0; i <= NUM_SIDES; ++i)
    {
        ang = (float)i*DELTA_ANG;
        {
            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(-HEIGHT);
            vertices.push_back(sinf(ang)*RAD);

            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(0.0f);
            vertices.push_back(sinf(ang)*RAD);

            vertices.push_back((float)i*DU);
            vertices.push_back(0.0f);

            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(HEIGHT);
            vertices.push_back(sinf(ang)*RAD);

            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(0.0f);
            vertices.push_back(sinf(ang)*RAD);

            vertices.push_back((float)i*DU);
            vertices.push_back(1.0f);
        }
    }

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
}

void SetupRC() 
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

	phongShader = gltLoadShaderPairWithAttributes("shaders/phong_shader.vp", "shaders/phong_shader.fp", 0);

	glUseProgram(phongShader);

    uniformMatModelview[0] = glGetUniformLocation(phongShader, "matModelview");   
    uniformMatProjection[0] = glGetUniformLocation(phongShader, "matProjection");
    uniformMatNormal[0] = glGetUniformLocation(phongShader, "matNormal");

    glUniform1i(glGetUniformLocation(phongShader, "texture0"), 0);

    uniformLightPos[0]   = glGetUniformLocation(phongShader, "light0.pos");
    uniformLightColor[0] = glGetUniformLocation(phongShader, "light0.color");
	
    CreateVertexBuffers();
	
	glUseProgram(0);
}

void cleanUp()
{
    glDeleteTextures(1, &texID);

    glUseProgram(0);
    glDeleteShader(phongShader);

    glDeleteBuffers(1, &vertexBufferID);
}

#pragma endregion

void RenderScene(void) 
{
    m3dAddVectors3(camPos, camPos, camVel);

    if (animationOn)
	{
		angle_x+=0.02f;
		angle_y+=0.01f;
		lightPos[0]=dist*cosf(angle_y);
		lightPos[1]=0.5;
		lightPos[2]=dist*sinf(angle_y);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, wire ? GL_LINE : GL_FILL);

    glLoadIdentity();

    gluLookAt(camPos[0], camPos[1], camPos[2], 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);
	
	glUseProgram(phongShader);
    
    glUniformMatrix4fv(uniformMatModelview[activeShader], 1, false, matModelview); 
    glUniformMatrix4fv(uniformMatProjection[activeShader], 1, false, matProjection);
	
	m3dExtractRotationMatrix33(matNormal, matModelview);
	glUniformMatrix3fv(uniformMatNormal[activeShader], 1, false, matNormal);

	M3DVector3f eyeLight;
	m3dTransformVector3(eyeLight, lightPos, matModelview);

	glUniform3fv(uniformLightPos[activeShader], 1, eyeLight);
	glUniform3fv(uniformLightColor[activeShader], 1, lightColor);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (int i = -10 ; i < 10; ++i)
	{
		glLoadIdentity();
		glTranslatef(0, (float)i/5, 0);

		gluLookAt(camPos[0], camPos[1], camPos[2], 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);

		glUniformMatrix4fv(uniformMatModelview[activeShader], 1, false, matModelview);
		
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3+3+2)*sizeof(float), (void *)((0)*sizeof(float)));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	(3+3+2)*sizeof(float), (void *)((3)*sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,	(3+3+2)*sizeof(float), (void *)((3+3)*sizeof(float)));
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 320);
	}

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
    glDisableVertexAttribArray(2); 

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(matModelview);

	if (!animRings)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINES);
			int id;
			for (int i = 0; i < 32000; ++i)
			{
				id = i * ELEMENT_COUNT;
				glVertex3f(vertices[id+0], vertices[id+1], vertices[id+2]);
				glVertex3f(vertices[id+0]+vertices[id+3], vertices[id+1]+vertices[id+4], vertices[id+2]+vertices[id+5]);
			}
		glEnd();
	}

	glColor3fv(lightColor);
	glPointSize(8.0f);

	glBegin(GL_POINTS);
		glVertex3fv(lightPos);
	glEnd();

	glUseProgram(0);

    glutSwapBuffers();
}

void ChangeSize(int w, int h) 
{
    glViewport(0, 0, w, h);

    m3dMakePerspectiveMatrix(matProjection, (float)M3D_PI*0.25f, (float)w/(float)h, 0.1f, 1000.0f);
}

int main(int argc, char* argv[]) 
{
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitWindowSize(1024, 600);

    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    if (glutCreateWindow("opengl app") < 1)
    {
        printf("cannot create rendering window!");
        return -1;
    }


    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    glutIdleFunc(RenderScene);

    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(PressNormalKeys);
    glutKeyboardUpFunc(ReleaseNormalKeys);
    glutSpecialFunc(PressSpecialKey);
    glutSpecialUpFunc(ReleaseSpecialKey);

    GLenum err = glewInit();
    if (GLEW_OK != err) 
    {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();
    glutMainLoop();
    cleanUp();

    return 0;
}
