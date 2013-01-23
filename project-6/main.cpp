#include <GLTools.h>

#define FREEGLUT_STATIC
#include "glut.h"
#include "freeglut_ext.h"


GLint               locMVPReflect, locMVReflect, locNormalReflect, locInvertedCamera, locMVPSkyBox;
GLTriangleBatch     sphereBatch;

GLint reflectionShader;
GLint skyBoxShader;

GLint uniformMatModelview;
GLint uniformMatProjection;

M3DMatrix44f matMVP;
M3DMatrix44f matProjection;
M3DMatrix44f matModelview;
M3DMatrix33f matNormal;

M3DVector3f camPos = { 0.0f, 0.0f, 2.0f }; 
M3DVector3f camFront = { 0.0f, 0.0f, -1.0f }; 
M3DVector3f camRight = { 1.0f, 0.0f, 0.0f }; 
M3DVector3f camVel = { 0.0f }; 

float camRot = 0.0f; 
float camRotVel = 0.0f; 
float camVelTemp = 0.0f; 
float camRotTemp = 0.0f; 
float x = 0.0f, y = 0.0f, z = 5.0f, r = 20.0f; 

GLuint cubeTexture;


const char *szCubeFaces[6] = { "textures/pos_x.tga", "textures/neg_x.tga", "textures/pos_y.tga", "textures/neg_y.tga", "textures/pos_z.tga", "textures/neg_z.tga" };

GLenum  cube[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


void DrawSkyBox() 
{
    GLfloat fExtent = 15.0f;
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        
        glTexCoord3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-fExtent, -fExtent, fExtent);
        
        glTexCoord3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-fExtent, -fExtent, -fExtent);
        
        glTexCoord3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-fExtent, fExtent, -fExtent);
        
        glTexCoord3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-fExtent, fExtent, fExtent);

       
        glTexCoord3f(1.0f, -1.0f, -1.0f);
        glVertex3f(fExtent, -fExtent, -fExtent);
        
        glTexCoord3f(1.0f, -1.0f, 1.0f);
        glVertex3f(fExtent, -fExtent, fExtent);
        
        glTexCoord3f(1.0f, 1.0f, 1.0f);
        glVertex3f(fExtent, fExtent, fExtent);
        
        glTexCoord3f(1.0f, 1.0f, -1.0f);
        glVertex3f(fExtent, fExtent, -fExtent);
 
       
        glTexCoord3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-fExtent, -fExtent, -fExtent);
        
        glTexCoord3f(1.0f, -1.0f, -1.0f);
        glVertex3f(fExtent, -fExtent, -fExtent);
        
        glTexCoord3f(1.0f, 1.0f, -1.0f);
        glVertex3f(fExtent, fExtent, -fExtent);
        
        glTexCoord3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-fExtent, fExtent, -fExtent);

        
        glTexCoord3f(1.0f, -1.0f, 1.0f);
        glVertex3f(fExtent, -fExtent, fExtent);
        
        glTexCoord3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-fExtent, -fExtent, fExtent);
        
        glTexCoord3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-fExtent, fExtent, fExtent);
        
        glTexCoord3f(1.0f, 1.0f, 1.0f);
        glVertex3f(fExtent, fExtent, fExtent);

       
        glTexCoord3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-fExtent, fExtent, fExtent);
        
        glTexCoord3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-fExtent, fExtent, -fExtent);
        
        glTexCoord3f(1.0f, 1.0f, -1.0f);
        glVertex3f(fExtent, fExtent, -fExtent);
        
        glTexCoord3f(1.0f, 1.0f, 1.0f);
        glVertex3f(fExtent, fExtent, fExtent);
  
      
        glTexCoord3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-fExtent, -fExtent, -fExtent);
        
        glTexCoord3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-fExtent, -fExtent, fExtent);
        
        glTexCoord3f(1.0f, -1.0f, 1.0f);
        glVertex3f(fExtent, -fExtent, fExtent);
        
        glTexCoord3f(1.0f, -1.0f, -1.0f);
        glVertex3f(fExtent, -fExtent, -fExtent);
    glEnd();
}

///////////////////////////////////////////////////////////////////////////////
// Window has changed size, or has just been created. In either case, we need
// to use the window dimensions to set the viewport and the projection matrix.
void ChangeSize(int w, int h) 
{
    glViewport(0, 0, w, h);

    m3dMakePerspectiveMatrix(matProjection, (float)M3D_PI*0.25f, (float)w/(float)h, 0.1f, 1000.0f);
}

void PressNormalKeys(unsigned char key, int x, int y) 
{

}

void ReleaseNormalKeys(unsigned char key, int x, int y) 
{
	if (key == 27) 
		exit(0);
}

void PressSpecialKey(int key, int x, int y) 
{

		if (key == GLUT_KEY_UP)
			camVelTemp = -0.08f; 
		
		else if (key == GLUT_KEY_DOWN)
			camVelTemp = 0.08f; 

		else if (key == GLUT_KEY_RIGHT)
			camRotTemp = -0.01f; 
		
		else if (key == GLUT_KEY_LEFT)
			camRotTemp = 0.01f;	
}


void ReleaseSpecialKey(int key, int x, int y) 
{

		if (key == GLUT_KEY_UP)
			camVelTemp = 0.0;
		else if (key == GLUT_KEY_DOWN)
			camVelTemp = 0.0;
		else if (key == GLUT_KEY_RIGHT)
			camRotTemp = 0.0f;
		else if (key == GLUT_KEY_LEFT)
			camRotTemp = 0.0f;
}

void SetupRC() 
{
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	reflectionShader = gltLoadShaderPairWithAttributes("shaders_new/Reflection.vp", "shaders_new/Reflection.fp", 2, 
                                                GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                GLT_ATTRIBUTE_NORMAL, "vNormal");
	skyBoxShader = gltLoadShaderPairWithAttributes("shaders_new/SkyBox.vp", "shaders_new/SkyBox.fp", 2, 
                                                GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                GLT_ATTRIBUTE_NORMAL, "vNormal");


    glUseProgram(reflectionShader);

	locMVPReflect = glGetUniformLocation(reflectionShader, "mvpMatrix");
	if (locMVPReflect == -1)
        printf("cannot find \"mvpMatrix\" in reflectionShader!\n");

    locMVReflect = glGetUniformLocation(reflectionShader, "mvMatrix");
    if (locMVReflect == -1)
        printf("cannot find \"mvMatrix\" in reflectionShader!\n");

    locNormalReflect = glGetUniformLocation(reflectionShader, "normalMatrix");
    if (locNormalReflect == -1)
        printf("cannot find \"normalMatrix\" in reflectionShader!\n");

	locInvertedCamera = glGetUniformLocation(reflectionShader, "mInverseCamera");
    if (locInvertedCamera == -1)
        printf("cannot find \"mInverseCamera\" in reflectionShader!\n");



	glUseProgram(skyBoxShader);

	locMVPSkyBox = glGetUniformLocation(skyBoxShader, "mvpMatrix");
	if (locMVPSkyBox == -1)
        printf("cannot find \"mvpMatrix\" in skyBoxShader!\n");


    
    GLbyte *pBytes;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    int i;
       
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
        
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
            
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);       
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
  
    for(i = 0; i < 6; i++)
    {        
        pBytes = gltReadTGABits(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
        glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
	gltMakeSphere(sphereBatch, 1.0f, 52, 26);
}

void RenderScene(void) 
{
	M3DVector3f vec = { sinf(camRot) * camVelTemp, 0, cosf(camRot) * camVelTemp };
		camRot += camRotTemp;

      m3dAddVectors3(camPos, camPos, vec);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glLoadIdentity();

	gluLookAt(             camPos[0], camPos[1],              camPos[2], 
			  camPos[0]-sinf(camRot),         0, camPos[2]-cosf(camRot),
		                         0.0,       1.0,                   0.0);

    glGetFloatv(GL_MODELVIEW_MATRIX, matModelview);

	m3dMatrixMultiply44(matMVP, matProjection, matModelview);

	glUseProgram(skyBoxShader);
	glUniformMatrix4fv(locMVPSkyBox, 1, GL_FALSE, matMVP);

	DrawSkyBox();


	m3dExtractRotationMatrix33(matNormal, matModelview); 

	M3DMatrix44f mv, invMV;
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	m3dInvertMatrix44(invMV, mv);		

	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf(invMV);
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
        glUseProgram(reflectionShader);
		glUniformMatrix4fv(locMVPReflect, 1, GL_FALSE, matMVP);
		glUniformMatrix4fv(locMVReflect, 1, GL_FALSE, matModelview);
        glUniformMatrix3fv(locNormalReflect, 1, GL_FALSE, matNormal);
		glUniformMatrix4fv(locInvertedCamera, 1, GL_FALSE, invMV);

        sphereBatch.Draw();
	
	glPopMatrix();


    glUseProgram(0);

    glutSwapBuffers();
}


int main(int argc, char* argv[]) 
{
   
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutInitWindowSize(800, 600);

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

    return 0;
}
