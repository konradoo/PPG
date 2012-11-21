#include <GLTools.h>            // OpenGL toolkit
#include <GLFrustum.h>
#include <GLFrame.h>
#include <StopWatch.h>
#include <GLMatrixStack.h>

#define FREEGLUT_STATIC
#include "glut.h"            // Windows FreeGlut equivalent

#define pi 3.1415

///////////////////////////////////////////////////////////////////////////////
GLuint shader;
int viewMatrixLocation;

GLFrustum viewFrustum; 
GLFrame viewFrame; 
GLMatrixStack stack;

M3DMatrix44f viewMatrix, Camera;
CStopWatch timer; 

float rotation = 0;
float scale;
short int scaleup = 1;
///////////////////////////////////////////////////////////////////////////////



void SetUpFrame(GLFrame &frame,const M3DVector3f origin, const M3DVector3f forward, const M3DVector3f up)
{
	frame.SetOrigin(origin);
	frame.SetForwardVector(forward);	
	M3DVector3f side,oUp;
	m3dCrossProduct3(side,forward,up);
	m3dCrossProduct3(oUp,side,forward);
	frame.SetUpVector(oUp);
	frame.Normalize();
}

void LookAt(GLFrame &frame, const M3DVector3f eye, const M3DVector3f at, const M3DVector3f up) 
{
    M3DVector3f forward;
    m3dSubtractVectors3(forward, at, eye);
    SetUpFrame(frame, eye, forward, up);
}

void ChangeSize(int w, int h) 
{
	if (h == 0)
		h = 1;
    glViewport(0, 0, w, h);

	viewFrustum.SetPerspective(45.0, float(w)/float(h), 1, 1000.0f);
}

void drawPiramid()
{
	glBegin(GL_TRIANGLES);
		glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 1.0, 0.0);
			glVertex3f(0, 1, 0);
			glVertex3f(-1, 0, 1);
			glVertex3f(1, 0, 1);

		glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);
			glVertex3f(0, 1, 0);
			glVertex3f(1, 0, -1);
			glVertex3f(-1, 0, -1);

		glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
			glVertex3f(0, 1, 0);
			glVertex3f(-1, 0, 1);
			glVertex3f(-1, 0, -1);
		
		glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
			glVertex3f(0, 1, 0);
			glVertex3f(1, 0, -1);
			glVertex3f(1, 0, 1);
	glEnd();
}

void SetupRC() 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shader = gltLoadShaderPairWithAttributes("shader.vp", "shader.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");
	
	fprintf(stdout, "GLT_ATTRIBUTE_VERTEX : %d\nGLT_ATTRIBUTE_COLOR : %d \n", GLT_ATTRIBUTE_VERTEX, GLT_ATTRIBUTE_COLOR);
	
	viewMatrixLocation=glGetUniformLocation(shader,"viewMatrix");
	if(viewMatrixLocation == -1)
		fprintf(stderr,"uniform viewMatrix could not be found\n");

    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


// kamerka
	M3DVector3f origin, at, up;
	m3dLoadVector3(origin, 6.0f, 3, 0.0f);
	m3dLoadVector3(at, 0.0f, 0.0f, 0.0f);
	m3dLoadVector3(up, 0.0f, 1.0f, 0.0f);

	LookAt(viewFrame, origin, at, up);
}

void RenderScene(void) 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

// Poruszaj¹ca siê kamerka	
	M3DVector3f at = {0, 0, 0};
	M3DVector3f up = {0, 1, 0};
	M3DVector3f eye;
	float angle = timer.GetElapsedSeconds();

	eye[0] = 7.8f * cos(angle);
	eye[1] = 3.5f;	
	eye[2] = 7.0f * sin(angle);

	LookAt(viewFrame, eye, at, up);

	rotation += 0.2;

	scale = scale + (0.001 * scaleup);
	if (scale >= 1)
		scaleup = -1;

	else if (scale <= 0)
		scaleup = 1;

	//

	stack.LoadMatrix(viewFrustum.GetProjectionMatrix()); 
	stack.PushMatrix(); 

	viewFrame.GetCameraMatrix(Camera); 

	stack.MultMatrix(Camera); 
	stack.GetMatrix(viewMatrix); 

    glUseProgram(shader);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, viewMatrix); 

    glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 1, 0.0);
	drawPiramid();

	//

	stack.PushMatrix();
	stack.Rotate(rotation, 0, 1, 0);

	stack.Translate(2, 0, 0);
	stack.GetMatrix(viewMatrix);

	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, viewMatrix);
	
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);
	drawPiramid();

	//

	stack.PopMatrix();
	stack.PushMatrix();
	stack.Translate(-4, 0, 0);
	stack.Scale(scale, scale, scale);
	stack.GetMatrix(viewMatrix);

	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, viewMatrix);
	glVertexAttrib3f(GLT_ATTRIBUTE_COLOR, 1.0, 0.0, 0.0);
	drawPiramid();



	stack.PopMatrix();
	stack.PopMatrix();

	glUseProgram(0);

    glutSwapBuffers();
	glutPostRedisplay();
}
int main(int argc, char* argv[]) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
    glutInitWindowSize(800, 480);
    glutCreateWindow("Solar system");
    glutReshapeFunc(ChangeSize);
    glutDisplayFunc(RenderScene);
    
    glutIdleFunc(RenderScene);

    GLenum err = glewInit();
    if (GLEW_OK != err) 
	{
        fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    SetupRC();
    glutMainLoop();
    return 0;
}
