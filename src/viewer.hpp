#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include "scene.hpp"
// The "main" OpenGL widget
class Viewer : public Gtk::GL::DrawingArea {
public:
  Viewer();
  virtual ~Viewer();

  // A useful function that forces this widget to rerender. If you
  // want to render a new frame, do not call on_expose_event
  // directly. Instead call this, which will cause an on_expose_event
  // call when the time is right.
  void invalidate();
  void set_scene_node(SceneNode *rootnode);

  void redo();
  void undo();
  void set_position();
  void set_joint();
  void reset_orientation();
  void reset_position();
  void set_z_buffer();
  void set_front_cull();
  void set_back_cull();
  Vector3D trackBallMapping(double x, double y);

  SceneNode *root;
  bool position;
  std::vector<Matrix4x4> trans_stack;
  std::vector<Matrix4x4> redo_stack;
  std::vector<int> id_stack;
  std::vector<int> redo_ids;
  bool front_face, back_face, z_buffer;
  Vector3D curPoint, lastPoint, rotAxis;
  bool buttonpressed[3];
  GLfloat objectXform;
  double rot_angle;

protected:

  // Events we implement
  // Note that we could use gtkmm's "signals and slots" mechanism
  // instead, but for many classes there's a convenient member
  // function one just needs to define that'll be called with the
  // event.

  // Called when GL is first initialized
  virtual void on_realize();
  // Called when our window needs to be redrawn
  virtual bool on_expose_event(GdkEventExpose* event);
  // Called when the window is resized
  virtual bool on_configure_event(GdkEventConfigure* event);
  // Called when a mouse button is pressed
  virtual bool on_button_press_event(GdkEventButton* event);
  // Called when a mouse button is released
  virtual bool on_button_release_event(GdkEventButton* event);
  // Called when the mouse moves
  virtual bool on_motion_notify_event(GdkEventMotion* event);

  // Draw a circle for the trackball, with OpenGL commands.
  // Assumes the context for the viewer is active.
  void draw_trackball_circle();

  int processHits(GLint hits, GLuint buffer[]);

  int pick_id;
  double x1,y1,dx,dy;
private:
};

#endif
