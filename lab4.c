/*
*	lab4.c
*   Greg Bender - gjb6676@cs.rit.edu
*   Computer Graphics I - Project 4
*   This generates a 3D world that you can move around in.
*/

#include <GL/glut.h>		// include glut library
#include<stdio.h>			// needed for printf function for testing
#include<math.h>			// needed for cos and sin functions
#include<malloc.h>			// functions needed for dynamic arrays
#include<stdlib.h>			// needed for absolute value function
#include<errno.h>
#include <string.h>
#include "chair.c"		    // load in the chair object that was generated
                            // using the NuGraf software

/* DEFAULT SYSTEM VARIABLES */
int screenHeight = 500;		// default screen height in pixels
int screenWidth = 700;		// default screen width in pixels
int keys[256];				// array used to hold if keys were pressed
int shift_key = 0;			// variable to see if shift key pressed (to straif)

int left_down = 0;			// 0 if left button is not down, 1 if it is
int middle_down = 0;		// 0 if middle button is not down, 1 if it is
int right_down = 0;			// 0 if right button is not down, 1 if it is

GLfloat oldxpos = 0.0f;		// 'last good' position, used if a collision with
GLfloat oldzpos = 0.0f;     // a wall is detected

int total_x_trans = 0;		// total amount translated on each axis, used to
int total_z_trans = 0;      // detect collisions

GLfloat look_up_down = 0.0f;	// used to change angle of view, pageup/pagedown key

GLfloat scale = 1.0f;     // these keep track of the scaling values

GLfloat trans_x = 0.0f;		// these keep track of translation values
GLfloat trans_y = 0.0f;     // as inputted by the user's mouse
GLfloat trans_z = 0.0f;

GLfloat rot = 1.0f;			// degree of the rotation (speed)
GLfloat cur_rot = 0.0f;		// current rotation

GLuint	front;		// holds display lists, used to speed up display
GLuint  back;
GLuint  left;
GLuint  right;
GLuint  tfloor;
GLuint  ceiling;
GLuint  wood;
/* END DEFAULT SYSTEM VARIABLES */


/* DIRECTION VARIABLES */
GLfloat facing = 0.0f;		// the angle at which the 'person' is facing
GLfloat xpos = 100.0f;		// the x position of the 'person'
GLfloat yrot = 0.0f;		// amount the 'person' is rotated (related to facing)
GLfloat zpos = -100.0f;		// the z position of the 'person'
/* END DIRECTION VARIABLES */

/* MOUSE TRACKING VARIABLES */
int lastPosition[3]	= {0, 0};		// keeps track of last mouse position x,y
/* END MOUSE TRACKING VARIABLES */

/* LIGHT VARIABLES */
// positions of lights are the same, because they are translated into place
// by draw_wall instead


// type one  
GLfloat ambient[]= { 0.5f, 0.5f, 0.5f, 1.0f };	// ambient light values
GLfloat diffuse[]= { 2.0f, 2.0f, 2.0f, 1.0f };	// difuse light values

// type two
GLfloat ambient_2[]= { 0.5f, 0.5f, 0.5f, 1.0f };	// ambient light values
// blue difuse light
GLfloat diffuse_2[]= { 0.0f, 0.0f, 7.0f, 1.0f };	// difuse light values
GLfloat lightposition[]= { 0.0f, 22.0f, 0.0f, 1.0f };	 // position of the light

GLuint light_on = 0;		// is the light on or off
/* END LIGHT VARIABLES */

/* MATERIAL PROPERITIES */
GLfloat material_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// a red material
GLfloat material_red_diffuse[] = { 1.0f, 0.0f, 0.0f, 1.0f };

// a green material
GLfloat material_green_diffuse[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat material_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat material_shininess[] = { 100.0f };
/* END MATERIAL PROPERITIES */

/* FOG VARIABLES */
GLfloat fog_color[4] = {0.5, 0.5, 0.5, 1.0};  // make for a grey color
GLuint fog_on = 0;			// is the fog on or off
/* END FOG VARIABLES */

/* TEXTURE STUFF */
// these hold the texture files
GLint brick_1;
GLint brick_2;
GLint brick_3;
GLint brass_panel;
GLint fireplace;
GLint tile;
GLint thefloor;
GLint topwindowon;
GLint topwindowoff;
/* END TEXTURE STUFF */

/* ROOM VARIABLES */
int collision[390][4];		// array that holds collision information for each
                            // part of the maze

// array that defines what the room looks like, see readme for more info
// 0's are empty space, 1's are a 'block', 2 is a start tile, 3 is an
// end tile, and 4 is where the object to rotate is located
int room[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,1,3,1,
	1,0,0,1,1,1,0,0,0,1,0,0,1,0,1,
	1,0,0,1,0,1,1,1,1,1,0,0,1,0,1,
	1,0,1,1,0,0,0,0,0,0,0,1,1,0,1,
	1,0,0,0,0,0,1,1,1,1,1,1,0,0,1,
	1,0,0,0,0,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,1,1,1,1,0,0,0,1,1,1,1,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,1,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,4,5,4,0,0,0,0,0,1,
	1,0,0,0,1,0,0,6,0,0,1,0,0,0,1,
	1,0,1,1,1,0,0,0,0,0,1,1,1,0,1,
	1,0,1,0,0,0,0,1,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,
	1,0,1,0,0,1,1,1,1,1,0,0,1,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,1,1,1,1,0,0,1,0,1,0,0,1,
	1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,
	1,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};
int rows = 26;		// number of 'rows' in the room array
int cols = 15;		// number of 'columns' in the room array

int total_rooms = 0;	// used to keep track of translations
GLfloat wallHeight = 20.0f;		// pixel height of a wall
GLfloat wallWidth = 20.0f;		// pixel width of a wall
/* END ROOM VARIABLES */

/*
* this sets glColor to be a random color
*/
void random_color() {
	// make random color
	glColor3f((rand() % 256)/256.0f,(rand() % 256)/256.0f,(rand() % 256)/256.0f);
} // end random_color

/* LoadTextureRAW
* load a 256x256 RGB .RAW file as a texture
* I did not write this function myself.  I found it online
* at http://www.nullterminator.net/gltexture.html
*/
GLuint LoadTextureRAW( const char * filename, int wrap ) {
    GLuint texture;
    int width, height;
    void * data;
    FILE * file;

    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;

    // allocate buffer
    width = 256;
    height = 256;
    data = malloc( width * height * 3 );

    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );

    // allocate a texture name
    glGenTextures( 1, &texture );

    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     wrap ? GL_REPEAT : GL_CLAMP );

    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                       GL_RGB, GL_UNSIGNED_BYTE, data );

    // free buffer
    free( data );

    return texture;
} // end LoadTextureRAW

/*
*	These are instructions to draw a front wall
*/
void front_wall() {

	// make this wall a display list to speed things up
	glNewList(front,GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, brick_2); // get right texture for this wall

		glBegin(GL_QUADS);
		// get the right texture for this wall
		glNormal3f( 0.0f, 0.0f, 1.0f); // normal pointing towards viewer
        glTexCoord2f(0.0,0.0);
		glVertex3f(0.0f, 0.0f, wallHeight);
        glTexCoord2f(0.0,1.0);
		glVertex3f(wallWidth, 0.0f, wallHeight);
        glTexCoord2f(1.0,1.0);
		glVertex3f(wallWidth, wallHeight, wallHeight);
        glTexCoord2f(1.0,0.0);
		glVertex3f(0.0f, wallHeight, wallHeight);
		glEnd();
	glEndList();
} // end front wall

/*
*	These are instructions to draw a back wall
*/
void back_wall() {

	// make this wall a display list to speed things up
	glNewList(back,GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, brick_3); // grab right texture
		glBegin(GL_QUADS);
     	glNormal3f( 0.0f, 0.0f,-1.0f); // normal points away from viewer
        glTexCoord2f(0.0,0.0);
		glVertex3f(0.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0,1.0);
		glVertex3f(wallWidth, 0.0f, 0.0f);
        glTexCoord2f(1.0,1.0);
		glVertex3f(wallWidth, wallHeight, 0.0f);
        glTexCoord2f(1.0,0.0);
		glVertex3f(0.0f, wallHeight, 0.0f);
		glEnd();
	glEndList();
} // end back wall

/*
*	These are instructions to draw a right wall
*/
void right_wall() {
	// make this wall a display list to speed things up
	glNewList(right,GL_COMPILE);
		glBindTexture(GL_TEXTURE_2D, brick_3); // grab right texture
		glBegin(GL_QUADS);
		glNormal3f( 1.0f, 0.0f, 0.0f); // normal points right
        glTexCoord2f(0.0,0.0);
		glVertex3f(wallWidth, 0.0f, 0.0f);
        glTexCoord2f(0.0,1.0);
		glVertex3f(wallWidth, 0.0f, wallWidth);
        glTexCoord2f(1.0,1.0);
		glVertex3f(wallWidth, wallHeight, wallWidth);
        glTexCoord2f(1.0,0.0);
		glVertex3f(wallWidth, wallHeight, 0.0f);
		glEnd();
	glEndList();
} // end right_wall

/*
*	These are instructions to draw a left wall
*/
void left_wall() {

	// make this wall a display list to speed things up
	glNewList(left,GL_COMPILE);
        glBindTexture(GL_TEXTURE_2D, brass_panel);	
		glBegin(GL_QUADS);
			glNormal3f(-1.0f, 0.0f, 0.0f); // normal points left
	        glTexCoord2f(0.0,0.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
		    glTexCoord2f(0.0,1.0);
			glVertex3f(0.0f, 0.0f, wallWidth);
	        glTexCoord2f(1.0,1.0);
			glVertex3f(0.0f, wallHeight, wallWidth);
			glTexCoord2f(1.0,0.0);
			glVertex3f(0.0f, wallHeight, 0.0f);
		glEnd();

	glEndList();

} // end left_wall

/*
*	These are instructions to draw a ceiling
*/
void ceiling_wall() {

   GLfloat ceil_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

	// make this wall a display list to speed things up
	glNewList(ceiling,GL_COMPILE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ceil_ambient);
		glBindTexture(GL_TEXTURE_2D, brick_2); // grab right texture
		glBegin(GL_QUADS);
			glNormal3f( 0.0f,1.0f, 0.0f); // normal points up
			glTexCoord2f(0.0,0.0);
			glVertex3f(0.0f, wallHeight, 0.0f);
	        glTexCoord2f(0.0,1.0);
			glVertex3f(0.0f, wallHeight, wallWidth);
		    glTexCoord2f(1.0,1.0);
			glVertex3f(wallWidth, wallHeight, wallWidth);
			glTexCoord2f(1.0,0.0);
			glVertex3f(wallWidth, wallHeight, 0.0f);
		glEnd();
	glEndList();
} // end ceiling_wall

/*
*	These are instructions to draw a floor
*/
void floor_wall() {
	
	// make this wall a display list to speed things up
	glNewList(tfloor,GL_COMPILE);

		glBindTexture(GL_TEXTURE_2D, brick_1); // grab right texture
	glBegin(GL_QUADS);
			glNormal3f( 0.0f, -1.0f, 0.0f); // normal points down
		    glTexCoord2f(0.0,0.0);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glTexCoord2f(0.0,1.0);
			glVertex3f(0.0f, 0.0f, wallWidth);
	        glTexCoord2f(1.0,1.0);
			glVertex3f(wallWidth, 0.0f, wallWidth);
		    glTexCoord2f(1.0,0.0);
			glVertex3f(wallWidth, 0.0f, 0.0f);
		glEnd();
	glEndList();
} // end floor_wall

/*
*	These are instructions to draw a floor with an 'ending' tile
*/
void end_floor_wall() {
	glBegin(GL_QUADS);
		glVertex3f(0.0f, wallHeight, 0.0f);
		glVertex3f(0.0f, wallHeight, wallWidth);
		glVertex3f(wallWidth, wallHeight, wallWidth);
		glVertex3f(wallWidth, wallHeight, 0.0f);
	glEnd();

} // end floor_wall

/*
*	These are instructions to draw a floor with a 'starting' tile
*/
void start_floor_wall() {
	glBegin(GL_QUADS);
		glVertex3f(0.0f, wallHeight, 0.0f);
		glVertex3f(0.0f, wallHeight, wallWidth);
		glVertex3f(wallWidth, wallHeight, wallWidth);
		glVertex3f(wallWidth, wallHeight, 0.0f);
	glEnd();
} // end floor_wall

/*
* draw chair
* I did not create the chair object, it was generated using the NuGraf software
* see chair.c for more information.
*/
void draw_chair() {

	glPushMatrix();

	glRotatef(cur_rot, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 20.0f, 0.0f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
	glScalef(0.40f, 0.40f, 0.40f);


		glBindTexture(GL_TEXTURE_2D, wood);
		OGL_Set_Current_Material_Parameters(2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f);
		rectangle__1();
		
		glBindTexture(GL_TEXTURE_2D, tile);		
		CHRFLWBR_CUSHIN();
		
		glBindTexture(GL_TEXTURE_2D, wood);
		CHRFLWBR_FRAME__T_100__TEAK_GIF();
		instance();
		instance_2();
	glPopMatrix();
} // end draw chair

/*
*	These are instructions to draw a ceilinglight
*/
void ceiling_light() {

	if ( light_on == 0 ) {
		glBindTexture(GL_TEXTURE_2D, topwindowon); // grab right texture
	}
	else {
		glBindTexture(GL_TEXTURE_2D, topwindowoff); // grab right texture
	}

	glBegin(GL_QUADS);

		    glTexCoord2f(0.0,0.0);
		glVertex3f(0.0f, 0.0f, 0.0f);
		    glTexCoord2f(0.0,1.0);
		glVertex3f(0.0f, 0.0f, wallWidth);
		    glTexCoord2f(1.0,1.0);
		glVertex3f(wallWidth, 0.0f, wallWidth);
		    glTexCoord2f(1.0,0.0);
		glVertex3f(wallWidth, 0.0f, 0.0f);
	glEnd();
} // end floor_wall

/*
* put the wall drawing routines into a display lists, this is
* 'suppose' to speed things up, but i can't tell a difference
*/
void build_display_lists() {

	front=glGenLists(1);
	back=glGenLists(1);
	left=glGenLists(1);
	right=glGenLists(1);
	ceiling=glGenLists(1);
	tfloor=glGenLists(1);
	front_wall();
	back_wall();
	left_wall();
	right_wall();
	ceiling_wall();
	floor_wall();
} // end build display lists

/*
* this performs the scale, translate and rotate functions on the
* scene as inticated by user mouses input (indicated by 4's on
* the room array)
*/
void doTransformations() {
	glScalef(scale, scale, scale);
	glTranslatef(trans_x, trans_y, trans_z);
	cur_rot += rot;
	glRotatef(-(360.0 - cur_rot), 1.0f, 1.0f, 1.0f);
} // end do Transformations

/*
* this draws a scene somewhere within the maze that can be controlled
* with the mouse
*/
void drawCubeScene() {

	GLfloat cubeHeight = 1.0f;
	GLfloat cubeWidth = 1.0f;
	GLfloat cubeDepth = 1.0f;

	// keep these transformations seperate from the
	// rest of the maze
	glPushMatrix();

	// put cube in the center of the viewers position
	glTranslatef(0.0f, wallHeight / 2, 0.0f);
	
	// perform user defined transformations
	doTransformations();

	// draw the triangles attatched to the cubes
	glPushMatrix();
	glRotatef(180.0f, 1.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 2.0f, 0.0f);

    // setup triangle material
	    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_red_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f( 1.0f,-1.0f, -1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f( 1.0f,-1.0f, -1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
	glEnd();

	glPopMatrix();

	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0f, 2.0f, 0.0f);

	    glMaterialfv(GL_FRONT, GL_DIFFUSE, material_green_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	// draw second triangle attatched to the cube
	glBegin(GL_TRIANGLES);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f( 1.0f,-1.0f, 1.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f( 1.0f,-1.0f, -1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f( 1.0f,-1.0f, -1.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,-1.0f, -1.0f);
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f);
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(-1.0f,-1.0f, 1.0f);
	glEnd();

	glPopMatrix();

		// setup materials
        glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

    // draw the cube that the triangle is attatched to
	glBindTexture(GL_TEXTURE_2D, fireplace); // grab right texture
	glBegin(GL_QUADS);
		glColor3f(1.0f,1.0f,1.0f);
		
		// face one
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0,0.0);
		glVertex3f( cubeWidth, cubeHeight,-cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-cubeWidth, cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-cubeWidth, cubeHeight, cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f( cubeWidth, cubeHeight, cubeDepth);


		glNormal3f( 0.0f,-1.0f, 0.0f);	
		glTexCoord2f(0.0,0.0);
		glVertex3f( cubeWidth,-cubeHeight, cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-cubeWidth,-cubeHeight, cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-cubeWidth,-cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f( cubeWidth,-cubeHeight,-cubeDepth);

		glNormal3f( 0.0f, 0.0f, 1.0f);	
		glTexCoord2f(0.0,0.0);
		glVertex3f( cubeWidth, cubeHeight, cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-cubeWidth, cubeHeight, cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-cubeWidth,-cubeHeight, cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f( cubeWidth,-cubeHeight, cubeDepth);
		

		glNormal3f( 0.0f, 0.0f,-1.0f);
		glTexCoord2f(0.0,0.0);
		glVertex3f( cubeWidth,-cubeHeight,-cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-cubeWidth,-cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-cubeWidth, cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f( cubeWidth, cubeHeight,-cubeDepth);
		
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0,0.0);
		glVertex3f(-cubeWidth, cubeHeight, cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f(-cubeWidth, cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f(-cubeWidth,-cubeHeight,-cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f(-cubeWidth,-cubeHeight, cubeDepth);
		
		glNormal3f( 1.0f, 0.0f, 0.0f);	
		glTexCoord2f(0.0,0.0);
		glVertex3f( cubeWidth, cubeHeight,-cubeDepth);
		glTexCoord2f(0.0,1.0);
		glVertex3f( cubeWidth, cubeHeight, cubeDepth);
		glTexCoord2f(1.0,1.0);
		glVertex3f( cubeWidth,-cubeHeight, cubeDepth);
		glTexCoord2f(1.0,0.0);
		glVertex3f( cubeWidth,-cubeHeight,-cubeDepth);
	glEnd();

	glPopMatrix();

	// put color back to normal so it doesn't interfere
	glColor3f(1.0f,1.0f,1.0f);

} // end drawCubeScene


/*
* These are called to draw each type of wall.  See readme file for
* more information on defining walls
*/
void draw_wall(int type, int position) {


	if ( type == 0 ) {
		// empty room has just a floor and ceiling
		glCallList(ceiling);
		glCallList(tfloor);
		// xmin
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;
	}
	// wall type 1
	// total block room |=|
	else if ( type == 1 ) {
		glCallList(ceiling);
		glCallList(tfloor);
		glCallList(back);
		glCallList(left);
		glCallList(right);
		glCallList(front);

		// xmin
		collision[position][0] = total_x_trans - 2;
		// xmax
		collision[position][1] = total_x_trans + wallWidth + 2; 
		//zmin
		collision[position][2] = total_z_trans - 3;
		//zmax
		collision[position][3] = total_z_trans + wallWidth + 3;
	}
	else if ( type == 2 ) {
		// empty room has just a floor and ceiling
		// and a starting tile
		floor_wall();
		start_floor_wall();
		// xmin
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;
  }
  else if ( type == 3 ) {
		// empty room has just a floor and ceiling
		// and an ending tile
		floor_wall();
		end_floor_wall();
		// xmin
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;
  }
	// wall type 4, this is the
	// one with the scene to rotate in it
	else if ( type == 4 ) {
		glCallList(tfloor);
		glCallList(ceiling);

		// xmin collision
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;

		// this draws an empty room, but includes
		// the scene that the user can rotate
		drawCubeScene();
	}
	else if ( type == 5 ) {
		// LIGHT ROOM
   		
		if ( light_on == 1 ) {
			// place the light in the correct spot
			glLightfv(GL_LIGHT1, GL_POSITION, lightposition);

			// turn the light on
			glEnable(GL_LIGHT1);
		}

		// empty room has just a floor and ceiling
		glCallList(ceiling);
		ceiling_light();
		draw_chair();

		// xmin
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;
	}
	else if ( type == 6 ) {
		// LIGHT ROOM type two
   		
		if ( light_on == 1 ) {
			// place the light in the correct spot
			glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

			// turn the light on
			glEnable(GL_LIGHT0);
		}

		// empty room has just a floor and ceiling
		glCallList(ceiling);
		ceiling_light();

		// xmin
		collision[position][0] = 1;
		// xmax
		collision[position][1] = 1; 
		//zmin
		collision[position][2] = 1;
		//zmax
		collision[position][3] = 1;
	}
	else {
		// should never get here
	}
} // end draw_wall


/*
*	function that is called whenever a mouse button is pressed
*/
void mouse( int button, int state, int x, int y ) {

	// remember where mouse was clicked
	lastPosition[0] = x;
	lastPosition[1] = y;

	// see if mouse button is being pressed
	if ( state == GLUT_DOWN ) {

		// if it is the right mouse button
		if ( button == GLUT_RIGHT_BUTTON ) {
			right_down = 1;
		}
		if ( button == GLUT_MIDDLE_BUTTON ) {
			middle_down = 1;
		}
		if ( button == GLUT_LEFT_BUTTON ) {
			left_down = 1;
		}
	}

	// increment the viewNumber if a mouse button was pressed
	if ( state==GLUT_UP ) {

		// if it is the right mouse button
		if ( button == GLUT_RIGHT_BUTTON ) {
			right_down = 0;
		}
		if ( button == GLUT_MIDDLE_BUTTON ) {
			middle_down = 0;
		}
		if ( button == GLUT_LEFT_BUTTON ) {
			left_down = 0;
		}

		// do something on mouse press
	   	glutPostRedisplay();  // marks the window as needing to be redisplayed
	}
} // end mouse

int check_collision() {

	int flag = 0;
	int i = 0;

	// if you look at each part of the collision array, and multiply the
	// the two related values together (xmin/max, and zmin/max) you will get
	// a negative number only when you are 'within' the block.  If it is positive
	// then you are ok
	for ( i = 0; i < rows * cols; i++ ) {
		if( (collision[i][0] * collision[i][1]) < 0 ) {
			if( (collision[i][2] * collision[i][3]) < 0 ) {
				flag = 1;
			}
		}
	} // end collision for loop

	return flag;
} // end check_collision


/*
* draw lights
* this sets up the lights
*/
void draw_lights() {
  /// glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  /// glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
} // end draw_lights

/*
* draw fog
* this function displays the fog on the screen
*/
void draw_fog() {

   glFogi(GL_FOG_MODE, GL_EXP); // choose type of fog
   glFogfv(GL_FOG_COLOR, fog_color); // set the color of the fog

   glFogf(GL_FOG_DENSITY, 0.05);	// how dense the fog is

   glHint(GL_FOG_HINT, GL_NICEST); // make the fog look the best

   glFogf(GL_FOG_START, 1.0);  // the beginning of the fog
   glFogf(GL_FOG_END, 15.0); // the end of the fog
} // end draw_fog

/*
* This is our main drawing function
* it moves to the correction loction, parses the rooms array
* and generates each wall
*/
void drawWorld( void ) {
	
	// make counter variables
	int i = 0;
	int j = 0;
	
	// keep track of total amount translated so far, this is used
	// to fill the collision array
	total_x_trans = 0;
	total_z_trans = 0;

	// flip like a mirror, so that a drawn map will match the level drawn
	glRotatef(180, 1.0f,0.0f, 0.0f);

	// move up a little bit so wall is where we want it
	glTranslatef(0.0f, -(wallHeight/2), 0.0f);

	// move to where the person is
	glRotatef(-(360.0 - yrot),0.0f,1.0f, 0.0f);
	glTranslatef(-xpos, 0.0f, -zpos);  

	total_x_trans += (-xpos);
	total_z_trans += (-zpos);

	// move starting position into the 'main room'
	glTranslatef(17.0f, 0.0f, 240.0f);	

	total_x_trans += 17.0f;
	total_z_trans += 240.0f;

	// loop through each wall to draw
	for ( i = 0; i < total_rooms; i++ ) {
		draw_wall(room[i], i);

		// these translate right and down, as it reads the wall data
		// like a book, left to right, top to bottom
		j++;
		if ( j < cols ) {	
			// next column over
			glTranslatef(wallWidth, 0.0f, 0.0f);

			total_x_trans += wallWidth;
		}
		// go to next row
		else {
			// go to 'beginning' of columns
			glTranslatef(-((cols - 1)*wallWidth), 0.0f, 0.0f);
			j = 0;
			glTranslatef(0.0f, 0.0f, -wallWidth);

			total_x_trans += (-((cols - 1)*wallWidth));
			total_z_trans += -wallWidth;
		}
	} // end for

} // end drawWorld

void animate(void) {
	glutPostRedisplay();		// tell screen it needs to be redrawn
} // end animation


/*
* this is called every time the screen is displayed
* and it checks the keys array to see if things have changed
*/
void process_keys_array() {

	int flag_moved = 0;

	// only check for a collision if one of the keys is
	// down
	if ( (keys[GLUT_KEY_UP] == 1) || (keys[GLUT_KEY_DOWN] == 1) ||
		shift_key == 1 ) {

		// if there is a collision, then
		// stop the motion, and move back to the last
		// good value
		if ( check_collision() == 1 ) {

			keys[GLUT_KEY_UP] = 0;
			keys[GLUT_KEY_DOWN] = 0;
			keys[GLUT_KEY_LEFT] = 0;
			keys[GLUT_KEY_RIGHT] = 0;
		
			xpos = oldxpos;
			zpos = oldzpos;
		}
		else {
			oldxpos = xpos;
			oldzpos = zpos;
		}
	} // end if check

	/// look left
	if ( keys[GLUT_KEY_LEFT] == 1 ) {

		// see if shift key is being pressed (to straif)
		if ( shift_key == 1 ) {
			zpos -= (float)sin( facing * (3.14/180.0)) * 1.05f;
			xpos -= (float)cos( facing * (3.14/180.0)) * 1.05f;
		}
		else {
			facing += 2.0f;
			yrot = facing;
		}
		flag_moved = 1;
	}
	// look right
	if ( keys[GLUT_KEY_RIGHT] == 1 ) {

		// see if shift key is being pressed (to straif)
		if ( shift_key == 1 ) {
			zpos += (float)sin( facing * (3.14/180.0)) * 1.05f;
			xpos += (float)cos( facing * (3.14/180.0)) * 1.05f;
		}
		else {
			facing -= 2.0f;
			yrot = facing;
		}
		flag_moved = 1;
	}
	// walk forwards
	if ( keys[GLUT_KEY_UP] == 1 ) {
		xpos += (float)sin( -facing * (3.14/180.0)) * 1.05f;
		zpos += (float)cos( -facing * (3.14/180.0)) * 1.05f;
		flag_moved = 1;
	}
	// walk backwards
	if ( keys[GLUT_KEY_DOWN] == 1 ) {
		xpos -= (float)sin( -facing * (3.14/180.0)) * 1.05f;
		zpos -= (float)cos( -facing * (3.14/180.0)) * 1.05f;
		flag_moved = 1;
	}
	// check view angle up view
	if ( keys[GLUT_KEY_PAGE_UP] == 1 ) {
		look_up_down += 0.05f;
	}

	// check view angle down view
	// check view angle up view
	if ( keys[GLUT_KEY_PAGE_DOWN] == 1 ) {
		look_up_down -= 0.05f;
	}
} // end process_keys_array

/*
* This sets up the world
*/
void display( void ) {

	// see if any keys were pressed
	process_keys_array();

	// set clear color to black
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

	// clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// use perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f,(GLfloat)screenWidth/(GLfloat)screenHeight, 0.1f, 600.0f);

	// set camera lookat
	gluLookAt(0.0f, 0.0f, 2.0f,
				0.0f, look_up_down, 0.0f, 
					0.0f, 1.0f, 0.0f);


	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	/* Set viewport - In this case, the whole
	 * screen, although window resizing has not
	 * been handled
	 */
	glViewport( 0, 0, screenWidth, screenHeight );

	// draw rest of world
	drawWorld();

	// flush buffers
	glFlush();

	// swap buffers for animation
	glutSwapBuffers();
} // end display

/*
* mouse motion checks for mouse motion
*/
void mouseMotion( int x, int y ) {

    int 	currentPosition[2];
    int 	dx = 0;
    int 	dy = 0;

	currentPosition[0] = x;
	currentPosition[1] = y;

    dx = currentPosition[0] - lastPosition[0];
    dy = currentPosition[1] - lastPosition[1];

	// check to see if button being held is right button
	// do scaling
	if ( right_down == 1 ) {
		scale = ((dx/10));
	}

	// do translation
	if ( middle_down == 1 ) {
		trans_x = (dx/10.0f);
		trans_y = (dy/10.0f);

	}
	// do rotation
	if ( left_down == 1 ) {
		rot = (dx / 100);
	}

	// redisplay the screen
	glutPostRedisplay();
} // end mouseMotion

/*
* callback function to handle 'regular' keyboard keys
*/
void keyboard ( unsigned char key, int x, int y ) {
	switch ( key ) {
		// reset mouse view
		case 'r':
		case 'R':
			scale = 1.0f;
			trans_x = 0.0f;
			trans_y = 0.0f;
			rot = 0.0f;
			facing = 0.0f;
			xpos = 100.0f;
			yrot = 0.0f;
			zpos = -100.0f;
			break;
		case 'l':
		case 'L':
			// turn lighting on and off
			if ( light_on == 1) {
				glDisable(GL_LIGHTING);
				light_on = 0;
			}
			else {
				glEnable(GL_LIGHTING);
				draw_lights();
				light_on = 1;

			}
			break;
		case 'f':
		case 'F':
			// toggle fog
			if ( fog_on == 1 ) {
				glDisable(GL_FOG);
				fog_on = 0;
			}
			else {
				glEnable(GL_FOG);
				fog_on = 1;

				draw_fog();
				glutPostRedisplay();
			}

			break;
       	default:
           	break;
   	}
} // end keyboard


/*
*   Deal with special key strokes (arrow keys)
*   and make changes in where looking or walking
*/
void HandleSpecialKeyboard(int key,int x, int y) {
	switch (key) {
		case GLUT_KEY_LEFT:
			// see if shift key is being pressed (to straif)			
			if ( glutGetModifiers() == GLUT_ACTIVE_SHIFT ) {
				shift_key = 1;
			}
			else {
				shift_key = 0;
			}
			keys[key] = 1;
			break;
		case GLUT_KEY_RIGHT:
			// see if shift key is being pressed (to straif)
			if ( glutGetModifiers() == GLUT_ACTIVE_SHIFT ) {
				shift_key = 1;
			}
			else {
				shift_key = 0;
			}
			keys[key] = 1;
			break;
		case GLUT_KEY_UP:
			keys[key] = 1;
			break;
		case GLUT_KEY_DOWN:
			keys[key] = 1;
			break;
		case GLUT_KEY_PAGE_UP:
			keys[key] = 1;
			break;
		case GLUT_KEY_PAGE_DOWN:
			keys[key] = 1;
			break;
	}
	// redisplay the screen
	glutPostRedisplay();
} // end HandleSpecialKeyboard

/*
* this function is called whenever a key is released
* this allows us to have two keys down at the same time
* (allows the person to turn and walk at same time)
*/
void HandleSpecialKeyboardUP(int key,int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT:
			// see if shift key is being pressed (to straif)			
			if ( glutGetModifiers() == GLUT_ACTIVE_SHIFT ) {
				shift_key = 1;
			}
			else {
				shift_key = 0;
			}
			keys[key] = 0;
			break;
		case GLUT_KEY_RIGHT:
			// see if shift key is being pressed (to straif)			
			if ( glutGetModifiers() == GLUT_ACTIVE_SHIFT ) {
				shift_key = 1;
			}
			else {
				shift_key = 0;
			}
			keys[key] = 0;
			break;
		case GLUT_KEY_UP:
			keys[key] = 0;
			break;
		case GLUT_KEY_DOWN:
			keys[key] = 0;
			break;
		case GLUT_KEY_PAGE_UP:
			keys[key] = 0;
			break;
		case GLUT_KEY_PAGE_DOWN:
			keys[key] = 0;
			break;
	}

	// redisplay the screen
	glutPostRedisplay();
} // end HandleSpecialKeyboardUp

/*
* displays controlls to the screen
*/
void display_controls() {
	// print movement instructions to the screen
	printf("Project 4 Controls\n");
	printf("-----------------------\n");
	printf("UP ARROW    - Walk Forward\n");
	printf("DOWN ARROW  - Walk Backwards\n");
	printf("LEFT ARROW  - Look Left\n");
	printf("RIGHT ARROW - Look Right\n");
	printf("PAGE UP     - Tilt View Up\n");
	printf("PAGE DOWN   - Tilt View Down\n");
	printf("\n");
	printf("HOLD SHIFT KEY - Straife Left/Right\n");
	printf("\n");
	printf("HOLD LEFT MOUSE BUTTON - Rotate\n");
	printf("HOLD MIDDLE MOUSE BUTTON - Translate\n");
	printf("HOLD RIGHT MOUSE BUTTON - Scaling\n");
	printf("\n");
	printf("F - Toggle Fog\n");
	printf("L - Toggle Lights\n");
	printf("\n");
	printf("r - reset to original position\n");
	printf("\n");
} // end display_controls

/*
* init function to setup texture mapping
* i got this from your website
*/
void texture_init() {

	 glEnable( GL_TEXTURE_2D );
	 
     glGenTextures(1, &brick_1);
     glBindTexture(GL_TEXTURE_2D, brick_1);
     brick_1 = LoadTextureRAW("brick1.raw", 1);

     glGenTextures(1, &brick_2);
     glBindTexture(GL_TEXTURE_2D, brick_2);
     brick_2 = LoadTextureRAW("brick2.raw", 1);

     glGenTextures(1, &brick_3);
     glBindTexture(GL_TEXTURE_2D, brick_3);
     brick_3 = LoadTextureRAW("brick3.raw", 1);

	 glGenTextures(1, &brass_panel);
     glBindTexture(GL_TEXTURE_2D, brass_panel);
     brass_panel = LoadTextureRAW("brasspanel.raw", 1);

     glGenTextures(1, &fireplace);
     glBindTexture(GL_TEXTURE_2D, fireplace);
     fireplace = LoadTextureRAW("fireplace.raw", 1);

     glGenTextures(1, &tile);
     glBindTexture(GL_TEXTURE_2D, tile);
     tile = LoadTextureRAW("tile.raw", 1);

     glGenTextures(1, &thefloor);
     glBindTexture(GL_TEXTURE_2D, thefloor);
     thefloor = LoadTextureRAW("floor.raw", 1);

     glGenTextures(1, &topwindowon);
     glBindTexture(GL_TEXTURE_2D, topwindowon);
     topwindowon = LoadTextureRAW("topwindowon.raw", 1);

     glGenTextures(1, &topwindowoff);
     glBindTexture(GL_TEXTURE_2D, topwindowoff);
     topwindowoff = LoadTextureRAW("topwindowoff.raw", 1);

     glGenTextures(1, &wood);
     glBindTexture(GL_TEXTURE_2D, wood);
     wood = LoadTextureRAW("wood.raw", 1);

} // end texture_init

/*
*	main function -- initilizes openGL, registers callback functions
*   and starts display function
*/
int main( int argc, char** argv ) {

	// use a different method for keeping track of keys, so that we can
	// press two down at a time
	int i = 0;
	int seed = 52;
	
	// set total number of rooms, used to keep
	// track of translations	
    total_rooms = rows * cols;

	// initilize the keys array
	for(i = 0; i < 256 ; i++ ) {
		keys[i] = 0;
	}

	// initilize random number generator
	srand( seed );

	// display game instructions to screen
	display_controls();

 	glutInit( &argc, argv );

	// use double buffering for animation
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

	glutInitWindowSize( screenWidth, screenHeight );
	glutInitWindowPosition( 0, 0 );
	glutCreateWindow( "Greg Bender - Project 4" );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LINE_STIPPLE );
	glShadeModel( GL_SMOOTH );  // Smooth Shading

	// make things look nice
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	// setup textures
	texture_init();

	build_display_lists();

	// register callback functions
	glutKeyboardFunc(keyboard);	// KEYBOARD - glut keyboard function
	glutSpecialFunc(HandleSpecialKeyboard); // KEYBOARD - handle arrow keys

	// handle when a key is 'let go', this along with the keys array
	// allows us to have two keys pressed at the same time
	glutSpecialUpFunc( HandleSpecialKeyboardUP );

	glutMouseFunc( mouse );			// check for mouse buttons
    glutMotionFunc( mouseMotion );  // check for mouse movement
	glutDisplayFunc( display );		// run function 'display'
	glutIdleFunc( animate );		// set idle callback function

	// setup light one (general white light)
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);	// setup ambient light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);	// setup difuse

	// setup light two (blue focus light)
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_2);	// setup ambient light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_2);	// setup difuse

	// start main loop
	glutMainLoop();

} // end main
