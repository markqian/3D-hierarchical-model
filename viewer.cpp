#include "viewer.hpp"
#include "algebra.hpp"
#include <iostream>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

Viewer::Viewer()
{
  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Ask for an OpenGL Setup with
  //  - red, green and blue component colour
  //  - a depth buffer to avoid things overlapping wrongly
  //  - double-buffered rendering to avoid tearing/flickering
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
  if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
  }

  // Accept the configuration
  set_gl_capability(glconfig);

  // Register the fact that we want to receive these events
  add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);
  
  position = false;
  back_face = false;
  front_face = false;
  z_buffer = false;
  buttonpressed[0] = false;
  buttonpressed[1] = false;
  buttonpressed[2] = false;

}

Viewer::~Viewer()
{
  // Nothing to do here right now.
}

void Viewer::redo() {
  root->redo_transformation(redo_stack,redo_ids);
  invalidate();
}

void Viewer::undo() {
  root->pop_transformation(trans_stack, id_stack,redo_stack,redo_ids);
  invalidate();
}

void Viewer::set_position() {
  position = true;
}

void Viewer::set_joint() {
  position = false;
}

void Viewer::reset_position() {
  root->reset_origin();
  glLoadIdentity();
  invalidate();
}

void Viewer::reset_orientation() {
  root->reset_trans();
  glLoadIdentity();
  invalidate();
}

void Viewer::set_z_buffer() {
  z_buffer = 1 - z_buffer;
  invalidate();
}

void Viewer::set_front_cull() {
  front_face = 1 - front_face;
  invalidate();
}

void Viewer::set_back_cull() {
  back_face = 1 - back_face;
  invalidate();
}

void Viewer::invalidate()
{
  // Force a rerender
  Gtk::Allocation allocation = get_allocation();
  get_window()->invalidate_rect( allocation, false);
}

void Viewer::set_scene_node(SceneNode *rootnode) {
  root = rootnode;
}


void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  glShadeModel(GL_SMOOTH);
  glClearColor( 0, 0, 0, 0.0 );
  glEnable(GL_DEPTH_TEST);

  gldrawable->gl_end();
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;


  // Set up for perspective drawing 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, get_width(), get_height());
  gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);
  //  glLoadIdentity();
  // glRotatef( rot_angle, rotAxis[0], rotAxis[1], rotAxis[2] );

  if (z_buffer) {
    glEnable(GL_DEPTH_TEST);
  }

  if (back_face || front_face) {
    glEnable(GL_CULL_FACE);
    if (back_face && front_face)
      glCullFace(GL_FRONT_AND_BACK);
    else if (back_face) 
      glCullFace(GL_BACK);
    else
      glCullFace(GL_FRONT);
  }

  // Clear framebuffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Set up lighting
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  GLfloat ambientLight[] = {0.0, 0.0, 0.0};
  GLfloat diffuseLight[] =  {1.0, 1.0, 1.0}; 
  GLfloat specularLight[] = {1.0, 1.0, 1.0}; 
  GLfloat position[] = { 0.0f, 0.0f, 10.0f, 0.0f };
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  // Draw stuff
  root->walk_gl(false);

  // Swap the contents of the front and back buffers so we see what we
  // just drew. This should only be done if double buffering is enabled.
  gldrawable->swap_buffers();

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

int Viewer::processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr, picked;
   float min_z, z1;

   min_z = 1000;
   printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;
   for (i = 0; i < hits; i++) { /*  for each hit  */
      names = *ptr;
      printf (" number of names for hit = %d\n", names); ptr++;

      z1 = (float) *ptr/0x7fffffff;     
      if ( z1 < min_z) {
       	min_z = z1;
      }
      else {
	continue;
      }

      printf("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
      printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
      printf ("   the name is ");

      if (std::abs(names) > 1000) {
	continue;
      }

      for (j = 0; j < names; j++) {     /*  for each name */
	picked = *ptr;
	printf ("%d ", *ptr); ptr++;
      }
      printf ("\n");
   }
   return picked;
}

#define BUFSIZE 2048

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  if (position) {
    if (event->button == 1) {
      //translate along x,y axis
      lastPoint[0] = event->x;
      lastPoint[1] = event->y;
      buttonpressed[0] = true;
    }
    else if (event->button == 2) {
      //translate along z axis
      lastPoint[0] = event->x;
      lastPoint[1] = event->y;
      buttonpressed[1] = true;
    }
    else if (event->button == 3) {
      //translate along virtual track ball
      lastPoint = trackBallMapping(event->x, event->y);
      buttonpressed[2] = true;
    }
  }
  else {
    GLuint selectBuf[BUFSIZE];
    GLint hits, picked;
    GLint viewport[4]; 
    glGetIntegerv (GL_VIEWPORT, viewport);
    
    glSelectBuffer (BUFSIZE, selectBuf);
    (void) glRenderMode (GL_SELECT);
    
    glInitNames();
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix ((GLdouble) event->x, 
		   (GLdouble) (viewport[3] - event->y), 
                   5.0, 5.0, viewport);
    gluPerspective(40.0, (GLfloat)get_width()/(GLfloat)get_height(), 0.1, 1000.0);
    
    // Draw stuff
    root->walk_gl(true);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    hits = glRenderMode (GL_RENDER);
    picked = processHits (hits, selectBuf);
    
    root->set_picked(picked,0,0);
    
    x1 = event->x;
    y1 = event->y;
    
    pick_id = picked;    
  }
  invalidate();

  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  if (position) {
    if (event->button == 1)
      buttonpressed[0] = false;
    else if (event->button == 2)
      buttonpressed[1] = false;
    else if (event->button == 3)
      buttonpressed[2] = false;
  }
  else {
    
    root->push_transformation(trans_stack, id_stack);
  }

  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  if (position) {
    if (buttonpressed[0] == true) {
      //translate along x,y axis
      double dx = event->x - lastPoint[0];
      double dy = event->y - lastPoint[1];
      root->mytranslate(Vector3D(dx/10,-dy/10,0));
      invalidate();
      lastPoint[0] = event->x;
      lastPoint[1] = event->y;
    }
    else if (buttonpressed[1] == true ) {
      //translate along z axis
      double dx = event->x - lastPoint[0];
      double dy = event->y - lastPoint[1];
      root->mytranslate(Vector3D(0,0,dy/10));
      invalidate();
      lastPoint[0] = event->x;
      lastPoint[1] = event->y;
    }
    else if (buttonpressed[2] == true) {
      //translate along virtual track ball
      curPoint = trackBallMapping(event->x, event->y);      
      Vector3D direction = curPoint - lastPoint;
      double velocity = direction.length();
      if (velocity > 0.0001) {
        rotAxis = curPoint.cross(lastPoint);
        rot_angle = 90 * velocity;
        lastPoint = curPoint;
        GLfloat matrix[16];
        glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
        glLoadIdentity();
        glRotatef( rot_angle, rotAxis[0], rotAxis[1], rotAxis[2] );
        glMultMatrixf( (GLfloat *) matrix );
        invalidate();
      }
    }
  }
  else {

    dx = event->x - x1;
    dy = event->y - y1;
    
    root->set_picked(pick_id,dx,dy);
    
    invalidate();
  }
  return true;
}

void Viewer::draw_trackball_circle()
{
  int current_width = get_width();
  int current_height = get_height();
  
  // Set up for orthogonal drawing to draw a circle on screen.
  // You'll want to make the rest of the function conditional on
  // whether or not we want to draw the circle this time around.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, current_width, current_height);
  glOrtho(0.0, (float)current_width, 
           0.0, (float)current_height, -0.1, 0.1);

  // change to model view for drawing
  glMatrixMode(GL_MODELVIEW);

  // Reset modelview matrix
  glLoadIdentity();

  // draw a circle for use with the trackball 
  glDisable(GL_LIGHTING);
  glEnable(GL_LINE_SMOOTH);
  glColor3f(1.0, 1.0, 1.0);
  double radius = current_width < current_height ? 
    (float)current_width * 0.25 : (float)current_height * 0.25;
  glTranslated((float)current_width * 0.5, (float)current_height * 0.5, 0);
  glBegin(GL_LINE_LOOP);
  for(size_t i=0; i<40; ++i) {
    double cosine = radius * cos(i*2*M_PI/40);
    double sine = radius * sin(i*2*M_PI/40);
    glVertex2f(cosine, sine);
  }
  glEnd();
  glColor3f(0.0, 0.0, 0.0);
  glDisable(GL_LINE_SMOOTH);
}

Vector3D Viewer::trackBallMapping(double x, double y) {
  Vector3D v;
  double d;
  double w = get_width();
  double h = get_height();

  v[0] = (2.0 * x - w)/w;
  v[1] = (h - 2.0 * y)/h;
  v[2] = 0;

  d = v.length();
  d = (d<1.0)?d:1.0;
  v[2] = cos(M_PI/2.0 *d);  
  v.normalize();
  return v;
}


