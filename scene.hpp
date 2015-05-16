#ifndef SCENE_HPP
#define SCENE_HPP

#include <list>
#include "algebra.hpp"
#include "primitive.hpp"
#include "material.hpp"
#include "a3.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

class SceneNode {
public:
  SceneNode(const std::string& name);
  virtual ~SceneNode();

  virtual void walk_gl(bool picking = false);
  
  virtual void set_picked(int id, double x, double y) {
    m_current_id = id;

    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {  
      (*it)->set_picked(m_current_id, x, y);
    }
  }

  const Matrix4x4& get_transform() const { return m_trans; }
  const Matrix4x4& get_inverse() const { return m_invtrans; }

  void set_scene_node(SceneNode *rootnode);

  void set_transform(const Matrix4x4& m)
  {
    m_trans = m;
    m_invtrans = m.invert();
  }

  void set_transform(const Matrix4x4& m, const Matrix4x4& i)
  {
    m_trans = m;
    m_invtrans = i;
  }

  void add_child(SceneNode* child)
  {
    m_children.push_back(child);
  }

  void remove_child(SceneNode* child)
  {
    m_children.remove(child);
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);

  // Returns true if and only if this node is a JointNode
  virtual bool is_joint() const;
  
  virtual int get_id() {
    return m_id;
  }

  virtual void set_moved() {
    moved = true;
  }

  virtual bool is_selected() {
    return selected;
  }


protected:
  
  // Useful for picking
  bool picked, moved, selected;
  int m_id, m_current_id;

  std::string m_name;

  // Transformations
  Matrix4x4 m_trans;
  Matrix4x4 m_invtrans;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(bool picking = false);

  virtual void set_picked(int id, double x, double y) {
    m_current_id = id;
   
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {  

      if ((*it)->is_selected()) {
	//apply transformation  
	if ((x/60 + m_joint_x.init) < m_joint_x.max && (x/60 + m_joint_x.init) > m_joint_x.min) {
	  m_joint_x.init += x/60;
	  set_transform(  Rotation(x/60, 'x' ) * m_trans  );
	}
	if ((y/60 + m_joint_y.init) < m_joint_y.max && (y/60 + m_joint_y.init) > m_joint_y.min) {
	  m_joint_y.init += y/60;
	  set_transform(  Rotation(y/60, 'z' ) * m_trans  );   
	}
	if (x != 0 && y != 0)
	  (*it)->set_moved();
      }
      
      (*it)->set_picked(m_current_id, x, y);
    }
  }

  virtual bool is_joint() const;

  virtual int get_id() {
    return m_id;
  }

  virtual void set_moved() {
    moved = true;
  }

  virtual bool is_selected() {
    return selected;
  }


  void set_joint_x(double min, double init, double max);
  void set_joint_y(double min, double init, double max);

  struct JointRange {
    double min, init, max;
  };

protected:

  JointRange m_joint_x, m_joint_y;
};

class GeometryNode : public SceneNode {
public:
  GeometryNode(const std::string& name,
               Primitive* primitive);
  virtual ~GeometryNode();

  virtual void walk_gl(bool picking = false) ;

  virtual void set_picked(int id, double x, double y) {
    m_current_id = id;

    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {  
      (*it)->set_picked(m_current_id, x, y);
    }
  }

  const Material* get_material() const;
  Material* get_material();

  void set_material(Material* material)
  {
    m_material = material;
  }

  virtual int get_id() {
    return m_id;
  }

  virtual void set_moved() {
    moved = true;
  }

  virtual bool is_selected() {
    return selected;
  }
  
protected:
  Material* m_material;
  Primitive* m_primitive;

};

#endif
