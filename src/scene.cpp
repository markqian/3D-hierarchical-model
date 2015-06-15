#include "scene.hpp"
#include <iostream>

int id = 0;

SceneNode::SceneNode(const std::string& name)
  : m_name(name),
    m_id(id++),
    m_current_id(0),
    picked(true),
    moved(false),
    selected(false)
{
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(bool picking) 
{
  glMultMatrixd(m_trans.transpose().begin());

  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->walk_gl(picking);    
  }

  glMultMatrixd(m_trans.invert().transpose().begin());
}

void SceneNode::rotate(char axis, double angle)
{
  Matrix4x4 r;

  if (axis == 'x') {
    r[1][1] = cos(angle*M_PI/180);
    r[1][2] = -sin(angle*M_PI/180);
    r[2][1] = sin(angle*M_PI/180);
    r[2][2] = cos(angle*M_PI/180);
  }
  else if (axis == 'y') {
    r[0][0] = cos(angle*M_PI/180);
    r[2][2] = cos(angle*M_PI/180);
    r[0][2] = sin(angle*M_PI/180);
    r[2][0] = -sin(angle*M_PI/180);
  }
  else if (axis == 'z') {
    r[0][0] = cos(angle*M_PI/180);
    r[0][1] = -sin(angle*M_PI/180);
    r[1][0] = sin(angle*M_PI/180);
    r[1][1] = cos(angle*M_PI/180);
  }

  m_init = m_init * r;
  m_trans = m_trans * r ;  
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 r;

  r[0][0] = amount[0];
  r[1][1] = amount[1];
  r[2][2] = amount[2];

  m_init = m_init * r;
  m_trans = m_trans * r;
}

void SceneNode::translate(const Vector3D& amount)
{
  Matrix4x4 r;

  r[0][3] = amount[0];
  r[1][3] = amount[1];
  r[2][3] = amount[2];

  m_init = m_init * r;
  m_trans = m_trans * r;
}

void SceneNode::mytranslate(const Vector3D& amount)
{
  Matrix4x4 r;

  r[0][3] = amount[0];
  r[1][3] = amount[1];
  r[2][3] = amount[2];

  m_trans = m_trans * r;
}


bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
  m_joint_x.change = 0;
  m_joint_y.change = 0;
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(bool picking) 
{
  glMultMatrixd(m_trans.transpose().begin());
  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {  
    (*it)->walk_gl(picking);
  }

  glMultMatrixd(m_trans.invert().transpose().begin());
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  m_joint_x.min = min;
  m_joint_x.init = init;
  m_joint_x.max = max;
  m_joint_x.change = init;

}

void JointNode::set_joint_y(double min, double init, double max)
{
  m_joint_y.min = min;
  m_joint_y.init = init;
  m_joint_y.max = max;
  m_joint_y.change = init;

}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive),
    m_material(NULL)    
{
    

}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(bool picking) 
{
  glMultMatrixd(m_trans.transpose().begin());

  if (m_material != NULL && m_current_id != m_id  && picking == false) {
    if (picked) {
      selected = false;
      m_material->apply_gl();
    }
    else {
      selected = true;
      GLfloat materialColor[] = {1.0f, 1.0f, 1.0f, 1.0};
      GLfloat materialSpecular[] = {0.1f, 0.1f, 0.1f, 1.0};

      //The color emitted by the material
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
      glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10); 
    }    
  }
  else if (picking == false  && moved) {
      
      GLfloat materialColor[] = {1.0f, 1.0f, 1.0f, 1.0};
      GLfloat materialSpecular[] = {0.1f, 0.1f, 0.1f, 1.0};

      //The color emitted by the material
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
      glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10); 
      moved = false;
      picked = false;
      selected = true;
  }
  else if (picking == false) {
    if (picked) {
      picked = false;
      selected = true;
      GLfloat materialColor[] = {1.0f, 1.0f, 1.0f, 1.0};
      GLfloat materialSpecular[] = {0.1f, 0.1f, 0.1f, 1.0};

      //The color emitted by the material
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
      glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10); 
    }
    else {
      picked = true;
      selected = false;
      m_material->apply_gl();
    }
  }

  if (picking) {
    glPushName(m_id);
  }

  m_primitive->walk_gl(picking);

  if (picking) {
    glPopName();
  }

  for (ChildList::const_iterator it = m_children.begin(); it != m_children.end(); it++) {
    (*it)->walk_gl(picking);
  }

  glMultMatrixd(m_trans.invert().transpose().begin());
  
}
 
