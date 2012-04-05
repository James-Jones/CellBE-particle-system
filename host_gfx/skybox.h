//include OGL Headers Here

#ifndef SKY_BOX_H
#define SKY_BOX_H

#include <GL/glpng.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

unsigned int skybox_texture;
unsigned int skybox_texture_array[6];
const float SKYBOX_SIZE = 20.0f;

void render_skybox()
{
		glLoadIdentity();
		glTranslatef(0,0,-5.0f);
		glScalef(SKYBOX_SIZE,SKYBOX_SIZE,SKYBOX_SIZE);


		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[0] );

	glBegin(GL_QUADS);
		// Front Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
	glEnd();		

		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[1] );

	glBegin(GL_QUADS);	
		// Back Face
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
	glEnd();		

		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[2] );

		// Top Face
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
	glEnd();		

		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[3] );

	glBegin(GL_QUADS);
		// Bottom Face
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
	glEnd();		

		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[4] );

	glBegin(GL_QUADS);
		// Right face
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
	glEnd();		

		glBindTexture( GL_TEXTURE_2D, skybox_texture_array[5] );
		
	glBegin(GL_QUADS);
		// Left Face
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
}


bool init_skybox()
{
  glGenTextures(6, skybox_texture_array);
  
  pngInfo info[6];

  skybox_texture_array[0] = pngBind("front.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[0] == 0)
  {
	  printf("Could not load front skybox texture\n");
	  return false;
  }
  
  skybox_texture_array[1] = pngBind("back.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[1] == 0)
  {
	  printf("Could not load back skybox texture\n");
	  return false;
  }

  skybox_texture_array[2] = pngBind("top.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[2] == 0)
  {
	  printf("Could not load top skybox texture\n");
	  return false;
  }

  skybox_texture_array[3] = pngBind("bottom.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[3] == 0)
  {
	  printf("Could not load bottom skybox texture\n");
	  return false;
  }

  skybox_texture_array[4] = pngBind("right.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[4] == 0)
  {
	  printf("Could not load right skybox texture\n");
	  return false;
  }

  skybox_texture_array[5] = pngBind("left.png", PNG_BUILDMIPMAPS, PNG_SOLID, &info[0], GL_CLAMP,
	  GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST);

  if( skybox_texture_array[5] == 0)
  {
	  printf("Could not load left skybox texture\n");
	  return false;
  }

  return true;
}

#endif
