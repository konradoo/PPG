// w10_light.cpp
// arrows: movement
// enter: reset cam
// space: animation

#include <GLTools.h>            // OpenGL toolkit

#define FREEGLUT_STATIC
#include "glut.h"            // Windows FreeGlut equivalent
#include "freeglut_ext.h"

#include "SOIL.h"
#include <vector>

#include "Ico_Tables.h"

using namespace std;
#pragma region globals


///////////////////////////////////////////////////////////////////////////////
// global variables
GLuint shaderID;

//
// matrices
//
GLint uniformMatModelview;
GLint uniformMatProjection;
GLint uniformMatNormal;

M3DMatrix44f matProjection;
M3DMatrix44f matModelview;
M3DMatrix33f matNormal;

// 
// light
//
M3DVector3f lightPos = { 0.0f }; // etap 1
M3DVector3f lightColor = { 1.0f, 0.0f, 0.0f }; // -||-
GLint uniformLightPos;
GLint uniformLightColor;

//
// textures
//
GLuint texID;

//
// camera
//
M3DVector3f camPos = { 0.0f, 0.0f, 8.0 }; // position in 3D space
M3DVector3f camVel = { 0.0f }; // velocity: obtained from keyboard

// 
// animation
//
bool animationOn = true;
float animRings = true;
float animParam = 0.0f;

//
// geometry
//
const int ELEMENT_COUNT = 3+3+2;        // pos(3) + normal(3) + tex(2)
vector<float> vertices;
GLuint vertexBufferID = 0;

//
// geometry params
//
const int NUM_SIDES = 128;
const float RAD = 1.0f;
const float HEIGHT = 0.05f;
const float DELTA_ANG = (float)M3D_PI * 2.0f / (float)NUM_SIDES;
const float DU = 1.0f/(float)NUM_SIDES; 
const int NUM_RINGS = 24;
const float H_SCALE = 1.5f;
const float Y_START = (float)NUM_RINGS*HEIGHT*H_SCALE;

//
// other
//
bool wire = false;
float angle_x = 0.0f;
float angle_y = 0.0f;

#pragma endregion




#pragma region input

///////////////////////////////////////////////////////////////////////////////
// normal keys == keys that have ascii code...
void PressNormalKeys(unsigned char key, int x, int y) 
{

}

///////////////////////////////////////////////////////////////////////////////
// normal keys == keys that have ascii code...
void ReleaseNormalKeys(unsigned char key, int x, int y) 
{
    if (key == ' ')
        animationOn = !animationOn;
    else if(key == 'r')
        animRings = !animRings;
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
}

///////////////////////////////////////////////////////////////////////////////
// special == arrows, alt, F1...12 etc, etc
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

///////////////////////////////////////////////////////////////////////////////
// special == arrows, alt, F1...12 etc, etc
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

///////////////////////////////////////////////////////////////////////////////
void CreateVertexBuffers()
{    
    float ang;
    vertices.clear();
    for (int i = 0; i <= NUM_SIDES; ++i)
    {
        ang = (float)i*DELTA_ANG;

        // first vertex:
        {
            // pos:
            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(-HEIGHT);
            vertices.push_back(sinf(ang)*RAD);

            // normal:
            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(0.0f);
            vertices.push_back(sinf(ang)*RAD);

            // texcoord:
            vertices.push_back((float)i*DU);
            vertices.push_back(0.0f);
        }
        // second vertex:
        {
            // pos:
            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(HEIGHT);
            vertices.push_back(sinf(ang)*RAD);

            // normal:
            vertices.push_back(cosf(ang)*RAD);
            vertices.push_back(0.0f);
            vertices.push_back(sinf(ang)*RAD);

            // texcoord:
            vertices.push_back((float)i*DU);
            vertices.push_back(1.0f);
        }
    }

    //
    // buffer objects
    //

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), &vertices[0], GL_STATIC_DRAW);
}

///////////////////////////////////////////////////////////////////////////////
// This function does any needed initialization on the rendering context.
// This is the first opportunity to do any OpenGL related tasks.
void SetupRC() 
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);

    //
    // init shaderID
    //
    shaderID = gltLoadShaderPairWithAttributes("shaders/w10_shader.vp", "shaders/w10_shader.fp", 0);   

    glUseProgram(shaderID);

    // get uniform locations:
    uniformMatModelview = glGetUniformLocation(shaderID, "matModelview");   
    uniformMatProjection = glGetUniformLocation(shaderID, "matProjection");
    uniformMatNormal = glGetUniformLocation(shaderID, "matNormal");

    // short way:
    glUniform1i(glGetUniformLocation(shaderID, "texture0"), 0);

    //
    // light params
    //
    uniformLightPos   = glGetUniformLocation(shaderID, "light0.pos");
    uniformLightColor = glGetUniformLocation(shaderID, "light0.color");

    //
    // generate texture:
    //

    //
    // geometry
    //
    CreateVertexBuffers();
	
	glUseProgram(0);
}

///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void cleanUp()
{
    glDeleteTextures(1, &texID);

    // destroy shaderID:
    glUseProgram(0);
    glDeleteShader(shaderID);

    glDeleteBuffers(1, &vertexBufferID);
}

#pragma endregion

void TriangleFace(M3DVector3f a, M3DVector3f b, M3DVector3f c) {
      M3DVector3f normal, bMa, cMa;
      m3dSubtractVectors3(bMa, b, a);
      m3dSubtractVectors3(cMa, c, a);
      m3dCrossProduct3(normal, bMa, cMa);
      m3dNormalizeVector3(normal);
	  
      glVertexAttrib3fv(1, normal);
      glVertex3fv(a);
      glVertex3fv(b);
      glVertex3fv(c);
	  
		
}

void drawTriangles(int n_faces, float *vertices, int *faces) {
	

      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      TriangleFace(vertices + 3 * faces[3 * i], vertices + 3 * faces[3 * i + 1], vertices + 3 * faces[3 * i + 2]);
      glEnd();
      }
	  
	  
      }
 
void drawSmoothTriangles(int n_faces, float *vertices, int *faces) {
      M3DVector3f normal;
      for (int i = 0; i < n_faces; i++) {
      glBegin(GL_TRIANGLES);
      for(int j=0;j<3;++j) {
      m3dCopyVector3(normal,vertices+3*faces[i*3+j]);
      m3dNormalizeVector3(normal);
      glVertexAttrib3fv(1, normal);
      glVertex3fv(vertices+3*faces[i*3+j]);
      
      }
      glEnd();
      }
      }
///////////////////////////////////////////////////////////////////////////////
// Called to draw scene
void RenderScene(void) 
{
    //
    // update camera
    //
    m3dAddVectors3(camPos, camPos, camVel);

    //
    // update animation
    //
	
    if (animationOn)
	{
		angle_x+=0.02f;
		angle_y+=0.01f;
		lightPos[0]=5*cosf(angle_y);
		lightPos[1]=(5/5)*sinf(angle_x);
		lightPos[2]=5*sinf(angle_y);
	}

    //
    // render
    //
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, wire ? GL_LINE : GL_FILL);

    glLoadIdentity();


    gluLookAt(camPos[0], camPos[1], camPos[2], 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0);
	glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);

    glUseProgram(shaderID);   
    
    //
    // uniforms:
    //
    glUniformMatrix4fv(uniformMatModelview, 1, false, matModelview); 
    glUniformMatrix4fv(uniformMatProjection, 1, false, matProjection); 

	m3dExtractRotationMatrix33(matNormal, matModelview); // ekstrakcja z modelView macierzy rotacji (matNormal) // etap 2
	glUniformMatrix3fv(uniformMatNormal, 1, false, matNormal);

	//glBindTexture(GL_TEXTURE_2D, texID);

	// light:
    // send uniforms for light (przekazywanie pozycji œwiat³a do shadera)
	M3DVector3f eyeLight;
	m3dTransformVector3(eyeLight, lightPos, matModelview); // transformujemy do przestrzeni oka
	glUniform3fv(uniformLightPos, 1, eyeLight);
	glUniform3fv(uniformLightColor, 1, lightColor);  // koniec etap 2


    
  
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	
		glLoadIdentity(); // wczytuje maciez jednostkowa do macierzy przeksztalcen (maciez 1)
		glTranslatef(0, 2, 0);

		gluLookAt(camPos[0], camPos[1], camPos[2], 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);

		glUniformMatrix4fv(uniformMatModelview, 1, false, matModelview); 

	
		drawTriangles(20, ico_vertices, ico_faces);

		glLoadIdentity(); // wczytuje maciez jednostkowa do macierzy przeksztalcen (maciez 1)
		glTranslatef(0, -2, 0);

		gluLookAt(camPos[0], camPos[1], camPos[2], 0.0, 0.0f, 0.0, 0.0, 1.0, 0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);

		glUniformMatrix4fv(uniformMatModelview, 1, false, matModelview); 
		drawSmoothTriangles(20, ico_vertices, ico_faces);

	
	// rendering:
 
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
    glDisableVertexAttribArray(2); 




	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(matModelview);
	
	glColor3f(0.0f, 1.0f, 0.0f);
	

	float light_col[]={1,1,1};
	glColor3fv(light_col);
	glPointSize(8.0f);

	glBegin(GL_POINTS);
		glVertex3fv(lightPos);
	glEnd();

	glUseProgram(0);

    glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h) 
{
    glViewport(0, 0, w, h);

    m3dMakePerspectiveMatrix(matProjection, (float)M3D_PI*0.25f, (float)w/(float)h, 0.1f, 1000.0f);
}

///////////////////////////////////////////////////////////////////////////////
// Main entry point for GLUT based programs
int main(int argc, char* argv[]) 
{
   
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
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

    //
    // glew
    //
    GLenum err = glewInit();
    if (GLEW_OK != err) 
    {
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    //
    // init our application
    //
    SetupRC();

    //
    // start rendering
    //
    glutMainLoop();

    cleanUp();

    return 0;
}
