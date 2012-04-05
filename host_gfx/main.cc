#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/time.h>
#include <time.h>

#include "skybox.h"

#include <unistd.h>
#include <math.h>
#include "client.h"

//#include "Text.h"

//bool SelectEffect;
//TextBox* text;

//#pragma comment(lib, "")

signed long g_sleep_time = 0;
Client g_client;

char input;

void Resize (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void Render() {

   glClearColor(0.3, 0.3, 0.3, 1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
   glLoadIdentity();
      
   gluLookAt(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -400.0f, 0.0f, 1.0f, 0.0f);
   
   printf("in render\n");


	render_skybox();
	
	glColor3f(1.0f, 1.0f, 1.0f);

float runtime = g_client.read_float();
	
	short n = g_client.read_int();
	printf("n batches is %d\n", n);
	Packet data[10];
	glPointSize(2.0f);
	
	glBegin(GL_POINTS);

	glVertex4f(0.0f, 0.0f, -5.0f, 1.0f);
	glVertex4f(-0.3f, 0.2f, -7.0f, 1.0f);
	while(n > 0)
	{
		g_client.read_packet_batch(data);
		glVertex4fv(data[0].pos);
		glVertex4fv(data[1].pos);
		glVertex4fv(data[2].pos);
		glVertex4fv(data[3].pos);
		glVertex4fv(data[4].pos);
		glVertex4fv(data[5].pos);
		glVertex4fv(data[6].pos);
		glVertex4fv(data[7].pos);
		glVertex4fv(data[8].pos);
		glVertex4fv(data[9].pos);
		--n;
	}
	glEnd();
	
	n = g_client.read_int();
	
	float length = 0.2f;
	
	glBegin(GL_LINES);
	
	while(n > 0)
	{
		g_client.read_packet_batch(data);

		glVertex4fv(data[0].pos);
		data[0].pos[1] -= length;
		glVertex4fv(data[0].pos);

		glVertex4fv(data[1].pos);
		data[1].pos[1] -= length;
		glVertex4fv(data[1].pos);

		glVertex4fv(data[2].pos);
		data[2].pos[1] -= length;
		glVertex4fv(data[2].pos);

		glVertex4fv(data[3].pos);
		data[3].pos[1] -= length;
		glVertex4fv(data[3].pos);
		
		glVertex4fv(data[4].pos);
		data[4].pos[1] -= length;
		glVertex4fv(data[4].pos);
		
		glVertex4fv(data[5].pos);
		data[5].pos[1] -= length;
		glVertex4fv(data[5].pos);
		
		glVertex4fv(data[6].pos);
		data[6].pos[1] -= length;
		glVertex4fv(data[6].pos);

		glVertex4fv(data[7].pos);
		data[7].pos[1] -= length;
		glVertex4fv(data[7].pos);
		
		glVertex4fv(data[8].pos);
		data[8].pos[1] -= length;
		glVertex4fv(data[8].pos);
		
		glVertex4fv(data[9].pos);
		data[9].pos[1] -= length;
		glVertex4fv(data[9].pos);
		--n;
	}
	glEnd();
	
   glFinish();
   glutSwapBuffers();

g_client.write_char(input);
if(input != 0)
	input = 0;

   if (g_sleep_time >= 0)
      usleep(g_sleep_time);
}

void Keyboard(unsigned char key, int x __attribute__ ((unused)),
				  int y __attribute__ ((unused)))
{
  switch (key)
  {
  	case 0x72: input = 'r';// r - increase rain
  	break;
  	case 0x73: input = 's';//stop rain
  	break;
  	case 0x65:input = 'e';//e - new explosion effect
  	break;
  case 0x1B://Esc
    g_client.shutdown();
    exit(0);
    break;
  }
}

int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   
   //PROCESS PROGRAM PARAMETERS
   
   //SLEEP TIME IS IN CONVERTED INTO MICROSECONDS (10e-6)
   if (argc == 2)
      g_sleep_time = (signed long)(1000000.0f * (float)atof(argv[1]));
  else
    if(argc == 1)
      g_sleep_time = 100000; //default 0.1 seconds
    else
    {
      fprintf(stderr, "usage: host_gfx [sleeptime(s)]\n");
      exit(0);
    }

input = 0;
   

  //GLUT SETUP

   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (640, 480);
   glutInitWindowPosition (0, 0);
   glutCreateWindow (argv[0]);
   glutDisplayFunc(Render);
   glutIdleFunc(Render);
   glutReshapeFunc(Resize);
   glutKeyboardFunc(Keyboard);
   
  //OPENGL STATE SETUP

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_CULL_FACE);//enable culling
	glCullFace(GL_BACK);//cull back faces
	
	glEnable(GL_TEXTURE_2D);
	
	init_skybox();

  g_client.create();
  g_client.connect_to_server();
  
    
  Resize(640, 480);
    
	
	//enter main loop
	
	printf("main loop\n");
	
  glutMainLoop();

printf("client disconnecting ...\n");

  g_client.shutdown();

printf("program returning ...\n");

   return 0;
}
