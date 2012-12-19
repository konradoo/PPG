#include <vector>
#include <GLTools.h>
#include <GLFrame.h>
#include <GLFrustum.h>
#include <StopWatch.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#ifdef __APPLE__
#include <glut/glut.h>
#else
#define FREEGLUT_STATIC
#include <glut.h>
#endif

CStopWatch timer;

int n_faces, n_vertices;

GLint shader;
GLint projectionMatrixLocation;
GLint modelViewMatrixLocation;
GLint normalMatrixLocation;

GLFrame frame;
GLFrustum frustum;

void setUpFrame(GLFrame &frame,const M3DVector3f origin, const M3DVector3f forward, const M3DVector3f up)
{
  frame.SetOrigin(origin);
  frame.SetForwardVector(forward);
  M3DVector3f side, oUp;
  m3dCrossProduct3(side, forward, up);
  m3dCrossProduct3(oUp, side, forward);
  frame.SetUpVector(oUp);
  frame.Normalize();
};

void lookAt(GLFrame &frame, const M3DVector3f eye, const M3DVector3f at, const M3DVector3f up)
{
  M3DVector3f forward;
  m3dSubtractVectors3(forward, at, eye);
  setUpFrame(frame, eye, forward, up);
}

void updateCamera()
{
  M3DVector3f at = {0, 0, 0};
  M3DVector3f up = {0, 0, -1};
  M3DVector3f eye;
  float angle = timer.GetElapsedSeconds() * 2;
  
  eye[0] = 6.8f * cos(angle);
  eye[1] = 6.0f * sin(angle);
  eye[2] = 5.0f;

  lookAt(frame, eye, at, up);
}

void loadVerticesAndFaces()
{
  FILE *fvertices = fopen("geode_vertices.dat","r");
  if (fvertices == NULL)
  {
    fprintf(stderr, "cannot open vertices file for reading\n");
    exit(-1);
  }
  char line[120];
  
  std::vector<float> vertices;
  while (fgets(line, 120, fvertices) != NULL)
  {
    float x, y, z;
    double norm;
    sscanf(line, "%f %f %f", &x, &y, &z);
    
    norm = x * x + y * y + z * z;
    norm = sqrt(norm);
    n_vertices ++;
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(1.0f);
    vertices.push_back(x / norm);
    vertices.push_back(y / norm);
    vertices.push_back(z / norm);
  }
  fprintf(stderr, "nv = %u %lu\n", n_vertices, vertices.size());
  
  FILE *ffaces = fopen("geode_faces.dat", "r");
  if (ffaces == NULL)
  {
    fprintf(stderr, "cannot open faces file for reading\n");
    exit(-1);
  }
  
  std::vector<GLuint> faces;
  while (fgets(line, 120, ffaces) != NULL)
  {
    GLuint i, j, k;
    
    if (3 != sscanf(line, "%u %u %u", &i, &j, &k))
    {
      fprintf(stderr, "error reading faces\n");
      exit(-1);
    }
    n_faces ++;
    faces.push_back(i - 1);
    faces.push_back(j - 1);
    faces.push_back(k - 1);
  }
  fprintf(stderr, "nf = %u\n", n_faces);
  
  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  
  glBufferData(GL_ARRAY_BUFFER, n_vertices * sizeof(float) * 7, &vertices[0], GL_STATIC_DRAW);
  if (glGetError() != GL_NO_ERROR) fprintf(stderr, "error copying vertices\n");
  
  glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const GLvoid *) 0);
  glVertexAttribPointer(GLT_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (const GLvoid *) (4 * sizeof(float)));
  glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
  glEnableVertexAttribArray(GLT_ATTRIBUTE_NORMAL);
  
  GLuint faces_buffer;
  glGenBuffers(1, &faces_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces_buffer);
  if (glGetError() != GL_NO_ERROR) fprintf(stderr, "faces_buffer invalid\n");

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_faces * sizeof(GLuint) * 3, &faces[0], GL_STATIC_DRAW);
  if (glGetError() != GL_NO_ERROR) fprintf(stderr, "error copying faces\n");
}

void resize(int w, int h)
{
  glViewport(0, 0, w, h);
  frustum.SetPerspective(90.0, w/h, 0.1, 200.0);
}

void init()
{
  loadVerticesAndFaces();
  
  shader = gltLoadShaderPairWithAttributes("shader.vp", "shader.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_NORMAL, "vNormal");
  
  projectionMatrixLocation = glGetUniformLocation(shader, "projectionMatrix");
  modelViewMatrixLocation = glGetUniformLocation(shader, "modelViewMatrix");
  normalMatrixLocation = glGetUniformLocation(shader, "normalMatrix");

  if (projectionMatrixLocation == -1) 
	  fprintf(stderr, "projectionMatrix could not be found\n");

  if (modelViewMatrixLocation == -1) 
	  fprintf(stderr, "modelViewMatrixLocation could not be found\n");

  if (normalMatrixLocation == -1) 
	  fprintf(stderr, "normalMatrixLocation could not be found\n");
}

void render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glUseProgram(shader);
  
  updateCamera();
  
  GLGeometryTransform geometryPipeline;
  GLMatrixStack modelViewStack;
  GLMatrixStack projectionStack;
  geometryPipeline.SetMatrixStacks(modelViewStack, projectionStack);
  projectionStack.LoadMatrix(frustum.GetProjectionMatrix());
  M3DMatrix44f cameraMatrix;
  frame.GetCameraMatrix(cameraMatrix);
  modelViewStack.PushMatrix(cameraMatrix);
  
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, geometryPipeline.GetProjectionMatrix());
  glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, geometryPipeline.GetModelViewMatrix());
  glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, geometryPipeline.GetNormalMatrix());
  
  modelViewStack.PushMatrix();
  modelViewStack.Translate(0.0, 1.0, 0.0);
  glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, geometryPipeline.GetProjectionMatrix());
  glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, geometryPipeline.GetModelViewMatrix());
  glUniformMatrix4fv(normalMatrixLocation, 1, GL_FALSE, geometryPipeline.GetNormalMatrix());
  glDrawElements(GL_TRIANGLES, 3 * n_faces, GL_UNSIGNED_INT, 0);
  modelViewStack.PopMatrix();
  
  glutSwapBuffers();
  glutPostRedisplay();
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(600, 600);
  glutCreateWindow("Proj 4");
  glutReshapeFunc(resize);
  glutDisplayFunc(render);
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0, 1.0, 1.0, 1.0);

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
    return 1;
  }

  init();

  glutMainLoop();
  return 0;
}