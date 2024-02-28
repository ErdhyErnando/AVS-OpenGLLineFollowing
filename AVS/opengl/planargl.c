/////////////////////////////////////////////////////////////
/* Template OpengGL sengaja dibuat untuk kuliah robotik 
*  di Departemen Teknik Elektro
*  Bagi yang ingin memodifikasi untuk keperluan yang lain,
*  dipersilahkan dengan menuliskan acknowledgement pada
*    Dr. Abdul Muis, MEng.
*    Autonomous Control Electronics (ACONICS) Research Group
*    http://www.ee.ui.ac.id/aconics
*////////////////////////////////////////////////////////////

#include <stdio.h> 
#include <stdlib.h> 
#include <GL/glut.h> // Header File For The GLUT Library
#include <GL/gl.h> // Header File For The OpenGL32 Library
#include <GL/glu.h> // Header File For The GLu32 Library
#include <unistd.h> // Header file for sleeping.
#include <math.h> 
#include <fcntl.h>			/* File control definitions */
#include <errno.h>			/* Error number definitions */
#include <termios.h>		/* POSIX terminal control definitions */
#include <sys/time.h>
#include "planar.c"

/* ascii code for the escape key */
#define ESCkey	27

/* The number/handle of our GLUT window */
int window, wcam, sensorwindow, backtopwindow, irwindow;


// visualisasi kamera
#define img_height 100
#define img_width 500
#define floor_height 2000
#define floor_width 20000

#define panjang 0.145

//image array
unsigned char image_raw[img_height+1][img_width+1];

//visual hasil kamera
void camera_sensor(void);
FILE *fileimage;
unsigned char* data = NULL;
unsigned int textureNumber;


/* To draw a quadric model */
GLUquadricObj *obj;

// ROBOT MODEL PARAMATER
#define Xoffset	0.0	
#define Yoffset	0.0
#define Zoffset	0.3

#define Link1 L1
#define Link2 L2

float *tetha1=&q1;
float *tetha2=&q2;
float tetha3 = 0;

float *x=&objx;
float *y=&objy;

char debug=0;


void Sim_main(void); // Deklarasi lebih awal agar bisa diakses oleh fungsi sebelumnya
void display_main(void);
void display_backtop(void);
void display_sensor(void);
void display_ir(void);

void display(void); // fungsi untuk menampilkan gambar robot / tampilan camera awal

/* define color */  
GLfloat green1[4]  ={0.8, 1.0, 0.8, 1.0};
GLfloat blue1[4]  ={0.1, 0.1, 1.0, 1.0};
GLfloat blue2[4]  ={0.2, 0.2, 1.0, 1.0};
GLfloat blue3[4]  ={0.3, 0.3, 1.0, 1.0};
GLfloat yellow1[4]={0.1, 0.1, 0.0, 1.0};
GLfloat yellow2[4]={0.2, 0.2, 0.0, 1.0};
GLfloat pink6[4] ={0.8, 0.55, 0.6, 1.0};
GLfloat yellow5[4]={0.8, 0.8, 0.0, 1.0};
GLfloat abu2[4]={0.5,0.5,0.5,1.0};
GLfloat gray1[4]  ={0.1, 0.1, 0.1, 1.0};
GLfloat gray2[4]  ={0.2, 0.2, 0.2, 1.0};
GLfloat gray3[4]  ={0.3, 0.3, 0.3, 1.0};
GLfloat gray4[4]  ={0.4, 0.4, 0.4, 1.0};
GLfloat gray5[4]  ={0.5, 0.5, 0.5, 1.0};
GLfloat gray6[4]  ={0.6, 0.6, 0.6, 1.0};
GLfloat gray7[4]  ={0.7, 0.7, 0.7, 1.0};
GLfloat gray8[4]  ={0.8, 0.8, 0.7, 1.0};
GLfloat gray9[4]  ={0.9, 0.9, 0.7, 1.0};


void  drawOneLine(double x1, double y1, double x2, double y2) 
   {glBegin(GL_LINES); glVertex3f((x1),(y1),0.0); glVertex3f((x2),(y2),0.0); glEnd();}
   
void  model_cylinder(GLUquadricObj * object, GLdouble lowerRadius,
  GLdouble upperRadius, GLdouble length, GLint res, GLfloat *color1, GLfloat *color2)
{
  glPushMatrix();
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
    glTranslatef(0,0,-length/2);
	  gluCylinder(object, lowerRadius, upperRadius, length, 20, res);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color2);
    gluDisk(object, 0.01, lowerRadius, 20, res); 
    glTranslatef(0, 0, length);
    gluDisk(object, 0.01, upperRadius, 20, res); 
  glPopMatrix();
}

void  model_box(GLfloat width, GLfloat depth, GLfloat height, GLfloat *color1, GLfloat *color2, GLfloat *color3, int color)
{
   width=width/2.0;depth=depth/2.0;height=height/2.0;
   glBegin(GL_QUADS);
// top
    if (color==1) 
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
    glVertex3f(-width,-depth, height);
    glVertex3f( width,-depth, height);
    glVertex3f( width, depth, height);
    glVertex3f(-width, depth, height);
   glEnd();
   glBegin(GL_QUADS);
// bottom
    if (color==1) 
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color1);
    glVertex3f(-width,-depth,-height);
    glVertex3f( width,-depth,-height);
    glVertex3f( width, depth,-height);
    glVertex3f(-width, depth,-height);
   glEnd();
   glBegin(GL_QUAD_STRIP);
// sides
    if (color==1) 
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color2);
    glVertex3f(-width,-depth,height);
    glVertex3f(-width,-depth,-height);
    glVertex3f(width,-depth,height);
    glVertex3f(width,-depth,-height);
    glVertex3f(width,depth,height);
    glVertex3f(width,depth,-height);
    glVertex3f(-width,depth,height);
    glVertex3f(-width,depth,-height);
    glVertex3f(-width,-depth,height);
   glEnd();
}



void disp_floor(void)
{
  int i,j,flagc=1;

  if (grid) {
   glPushMatrix();
   GLfloat dx=4.5, dy=4.5;
   GLint amount=15;
   GLfloat x_min=dx/2.0, x_max=dx/2.0, x_sp=(GLfloat) dx/amount;
   GLfloat y_min=-dy/2.0, y_max=dy/2.0, y_sp=(GLfloat) dy/amount;

   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green1);
   for(i=0; i<48; i++) {
      drawOneLine(-2.4+0.1*i, -2.4,       -2.4+0.11*i, 2.4);
      drawOneLine(-2.4,       -2.4+0.11*i, 2.4        -2.4+0.1*i);
   }
   glPopMatrix();
  }

  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, textureNumber);
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_POLYGON);
  glTexCoord2f(0,1); glVertex3f(-1.0f, -1.0f, 0);
  glTexCoord2f(0,1); glVertex3f(-1.0f, 1.0f, 0);
  glTexCoord2f(0,1); glVertex3f(1.0f, 1.0f, 0);
  glTexCoord2f(0,1); glVertex3f(1.0f, -1.0f, 0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

void  lighting(void)
{

	GLfloat light_ambient[] =  {0.2, 0.2, 0.2, 1.0};
	GLfloat light_diffuse[] =  {0.4, 0.4, 0.4, 1.0};
	GLfloat light_specular[] = {0.3, 0.3, 0.3, 1.0};
	GLfloat light_position[] = {2, 0.1, 7,1.0};
	GLfloat spot_direction[] = {0.0, -0.1, -1.0, 1.0};

	glClearColor(0.0, 0.0, 0.0, 0.0);     
  
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 40.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 4);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void disp_robot(void)
{
	
      glPushMatrix();
      glTranslatef(Xoffset,Yoffset, 0.02);
      glRotatef(tetha3, 0, 0, 1);
      model_box(0.005, 0.025, 0.002, pink6, pink6, pink6, 1);
      glPushMatrix();
      glTranslatef(0,0.07125, 0);
      model_box(0.02, 0.1175, 0.002, pink6, pink6, pink6, 1);
      glPopMatrix();
      glPushMatrix();
      glTranslatef(-0.025, 0, 0.001);
      glRotatef(90, 0, 1, 0);
      model_cylinder(obj, 0.025, 0.025, 0.005, 2, pink6, pink6);
      glPopMatrix();
      glPushMatrix();
      glTranslatef(0.025, 0, 0.001);
      glRotatef(-90, 0, 1, 0);
      model_cylinder(obj, 0.025, 0.025, 0.005, 2, pink6, pink6);
      glPopMatrix();
      glPopMatrix();
   

   
   






    //glTranslatef(Xoffset, Yoffset, Zoffset/2);
    //// Draw base
    //model_cylinder(obj, 0.1, 0.1, Zoffset, 2, blue1, yellow2);
    //// Menuju joint-1
    //glTranslatef(0, 0, Zoffset/2);
    //glRotatef(*tetha1*RTD,0,0,1);
    //glPushMatrix();
    //  // Gambar link1-1
    //  glRotatef(90,0,1,0);
    //  glTranslatef(0,0,Link1/2);
    //  model_cylinder(obj, 0.03, 0.03, Link1, 2, pink6, yellow2);
    //glPopMatrix();
    //// Menuju joint-2
    //glTranslatef(Link1,0,0);
    //glRotatef(*tetha2*RTD,0,0,1);
    //glPushMatrix();
    //  // Gambar link1-1
    //  glRotatef(90,0,1,0);
    //  glTranslatef(0,0,Link2/2);
    //  model_cylinder(obj, 0.03, 0.03, Link2, 2, yellow5, yellow2);
    //glPopMatrix();
  //PopMatrix();


}

// Draw Object
void display(void)
{
//   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT) ; // Clear The Screen and The Depth Buffer 
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT) ; // Clear The Screen and The Depth Buffer 
   //glLoadIdentity();  // Reset View
   disp_floor();
   
   disp_robot();

   /* since window is double buffered, 
      Swap the front and back buffers (used in double buffering). */
   glutSwapBuffers() ; 
   

}

void Sim_main(void)
{
	unsigned long Xr=0,Yr=0, Xg=0,Yg=0, Xb=0,Yb=0; // titik untuk menghitung sum
	int Nr=0, Ng=0, Nb=0;
	static unsigned int Rx,Ry, Gx,Gy, Bx,By; // untuk menyimpan hasil titik berat
	unsigned int i,j,k;
  static int count=0;
  glutSetWindow(window);
  animate(count);
  display();
  // Retrieve_serial();
  usleep(2000);
  	
	
}
void keyboard(unsigned char key, int i, int j)
{
	 switch(key){
      case ESCkey: exit(1); break;
      case '1': *tetha1+=5*DTR; break;
      case '2': *tetha2+=10*DTR; break;
      
   }
}



void init(void) 
{ 
   obj = gluNewQuadric(); 
   /* Clear background to (Red, Green, Blue, Alpha) */
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f) ;
   glEnable(GL_DEPTH_TEST); // Enables Depth Testing
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, 2, 0.2, 8);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0.2, -1.0, 0.5,  0.0, 0.2, 0.2,  0.0, 0.0, 1.0); 
	 lighting();
	 
   /* When the shading model is GL_FLAT only one colour per polygon is used, 
      whereas when the shading model is set to GL_SMOOTH the colour of 
      a polygon is interpolated among the colours of its vertices.  */
   glShadeModel(GL_SMOOTH) ; 

   glutDisplayFunc (&display) ;
   glutKeyboardFunc(&keyboard);

}

// Main Program
int main(int argc, char** argv)
{
 // Initialize GLUT
   /* Initialize GLUT state - glut will take any command line arguments 
      see summary on OpenGL Summary */  
   glutInit (&argc, argv);
   
   // Berikut jika ingin menggunakan serial port
   //fd = open_port();
   //init_port(fd);

   /* Select type of Display mode:   
      Double buffer 
      RGBA color
      Alpha components supported 
      Depth buffer */  

   //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );
   /* set a 400 (width) x 400 (height) window and its position */


   //glutInitWindowSize(800,400);	
   //glutInitWindowPosition (40, 100);

   /* Open a window */  
   //window = glutCreateWindow ("Simple Window");

   /* Initialize our window. */
   // init() ;
   obj = gluNewQuadric();

   init_robot();
   main_window();
   textureNumber = loadGLTexture("track.ppm", 500, 500);
   camera_backtopwindow();
   textureNumber = loadGLTexture("track.ppm",500,500);
   camera_window();
   textureNumber = loadGLTexture("track.ppm",500,500);
   ir_window();

   void main_window(void) {
      glutInitWindowSize(800,400);
      glutInitWindowPosition(40,100);

      //opening a window
      window = glutCreateWindow ("Sample");
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glEnable(GL_DEPTH_TEST);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(60.0, 2, 0.2, 8);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(0.2, -1.0, 1.5, 0.0, 0.2, 0.2, 0.0, 0.0, 1.0);
      lighting();

      glShadeModel(GL_SMOOTH);

      glutDisplayFunc(&display_main);
      glutKeyboardFunc(&keyboard);
   }

   int loadGLTexture(const char *filename, int width, int height) {
      free(data);

      fileimage = fopen(filename, "r");
      if (fileimage == NULL) return 0;

      data = (unsigned char*) malloc(width * height * 3);

      fread(data, width * height * 3, 1, fileimage);
      fclose(fileimage);

      unsigned int textureID;
      int border=0;
      int depth=width * height * 3;
      glGenTextures(1, &textureID);

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTextEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glTextParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTextParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTextParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTextParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

      }

   void camera_window(void) {
      glutInitWindowSize(img_width, img_height);
      glutInitWindowPosition (500, 100);
      sensorwindow = glutCreateWindow("Camera is Processing wakwaw");
      printf("sensorwindow id : %d\n", sensorwindow);
      glutDisplayFunc(&display_sensor);
      glutKeyboardFunc(&keyboard);
   }

   void ir_window(void) {
      glutInitWindowSize(img_width, img_height);
      glutInitWindowPosition (500, 100);
      ir_window = glutCreateWindow("Sensor Terlihat");
      printf("irwindow id : %d\n", irwindow);
      glutDisplayFunc(&display_ir);
      glutKeyboardFunc(&keyboard);
   }

   void display_sensor(void) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      float floor_x=0.145+0.005, floor_y=0, floor_z=0;
      float sense_x=0.145, sense_y=0, sense_z=0.2;

      float floor_x_ = newx(floor_x, floor_y);
      float floor_y_ = newy(floor_x, floor_y);
      float floor_x_ = newy(sense_x, sense_y);
      float floor_y_ = newy(sense_x, sense_y);

      glFrustum(-0.05, 0.05, 0.01, -0.01, 0.19, 1);
      gluLookAt(sense_x_, sense_y_, sense_z, floor_x_, floor_y_, floor_z, 0.0, 0.0, 1.0);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      disp_floor(false);
      disp_robot();
      lighting();
      glShadeModel(GL_SMOOTH);
      glutSwapBuffers();

      glPixelTransferf(GL_RED_SCALE, 0.3333*0.2);
      glPixelTransferf(GL_GREEN_SCALE, 0.3333*0.2);
      glPixelTransferf(GL_BLUE_SCALE, 0.3333*0.2);
      
      glReadPixels(0,0, img_width, img_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_raw);

      ir8 = (image_raw[50][sen8]<50) ? 1:0;
      ir7 = (image_raw[50][sen7]<50) ? 1:0;
      ir6 = (image_raw[50][sen6]<50) ? 1:0;
      ir5 = (image_raw[50][sen5]<50) ? 1:0;
      ir4 = (image_raw[50][sen4]<50) ? 1:0;
      ir3 = (image_raw[50][sen3]<50) ? 1:0;
      ir2 = (image_raw[50][sen2]<50) ? 1:0;
      ir1 = (image_raw[50][sen1]<50) ? 1:0;

      image_raw[50][sen8]=ir8*255;
      image_raw[50][sen7]=ir7*255;
      image_raw[50][sen6]=ir6*255;
      image_raw[50][sen5]=ir5*255;
      image_raw[50][sen4]=ir4*255;
      image_raw[50][sen3]=ir3*255;
      image_raw[50][sen2]=ir2*255;
      image_raw[50][sen1]=ir1*255;

      void display_ir(void){
         glClear(GL_COLOR_BUFFER_BIT);
         glDrawPixels(img_width, img_height, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_raw);
         glutSwapBuffers();
      }

      void jacobian(float &dx, float &dy, float &dshi, float dq2, float dq1, float shi) {
         dx=0.025/2.0*cos(shi)*(dq2+dq1);
         dy=0.025/2.0*sin(shi)*(dq2+dq1);
         dshi=0.025/0.18*(dq2-dq1);
      }

      void animate(int k) {

         static int oldsensor=0;
         int sensor = ((ir1)?-4.:0)+((ir2)?-3:0)+((ir3)?-2:0)+((ir4)?-1:0)+((ir5)?2:0)+((ir6)?2:0)+((ir7)?3:0)+((ir8)?4:0);
         int adasensor = ir1+ir2+ir3+ir4+ir5+ir6+ir7+ir8+ir8;
         if (adasensor) {

         }

         jacobian(dx, dy, dshi, dq2, dq1, shi);
         dq2=0; dq1=0;
         shi=0.025/0.18*(q2-q1)+0;

         rx=rx+rv*cos((shi+shi_old)/2.0);
         ry=ry+rv*sin((shi+shi_old)/2.0);
         shi_old=shi;
         usleep(100000);


      }


   }

   /* Register the function to do all our OpenGL drawing. */
   glutIdleFunc(&Sim_main); // fungsi untuk simulasi utama

   /* Start Event Processing Engine */ 
   glutMainLoop () ;
   return 0 ;
}           
