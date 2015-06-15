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
  void reset_origin() {
      m_trans = m_init;
  }

  virtual void reset_trans() {
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {      
      m_trans = m_init;
      (*it)->reset_trans();
    }
  }

  virtual ~SceneNode();
  
  virtual void walk_gl(bool picking = false);
  
  virtual void set_picked(int id, double x, double y) {
    m_current_id = id;

    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {  
      (*it)->set_picked(m_current_id, x, y);
    }
  }

  virtual void push_transformation(std::vector<Matrix4x4> &trans_stack, 
                                   std::vector<int> &id_stack) {
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      (*it)->push_transformation(trans_stack,id_stack);
    }
  }


  virtual bool pop_transformation(std::vector<Matrix4x4> &trans_stack, 
                                  std::vector<int> &id_stack,
                                  std::vector<Matrix4x4> &redo_stack,
                                  std::vector<int> &redo_ids) {
    bool done = false;
    if (!id_stack.empty()) {
      for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
        done = (*it)->pop_transformation(trans_stack, id_stack, redo_stack, redo_ids);
        if (done)
          break;
      }
    }
  }

  virtual bool redo_transformation(std::vector<Matrix4x4> &redo_stack,
                                   std::vector<int> &redo_ids) {
    bool done = false;
    if (!redo_ids.empty()) {
      for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
        done = (*it)->redo_transformation(redo_stack, redo_ids);
        if (done)
          break;
      }
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

  void set_new_transform(const Matrix4x4& m) {
    current_trans = m;
  }

  void reset_transform() {
    Matrix4x4 I;
    current_trans = I;
  }

  // Callbacks to be implemented.
  // These will be called from Lua.
  void rotate(char axis, double angle);
  void scale(const Vector3D& amount);
  void translate(const Vector3D& amount);
  void mytranslate(const Vector3D& amount);
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
  Matrix4x4 current_trans;
  Matrix4x4 m_init;

  // Hierarchy
  typedef std::list<SceneNode*> ChildList;
  ChildList m_children;
};

class JointNode : public SceneNode {
public:
  JointNode(const std::string& name);
  virtual ~JointNode();

  virtual void walk_gl(bool picking = false);

  virtual void reset_trans() {
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {      
      m_trans = m_init;
      (*it)->reset_trans();
    }
  }


  virtual void set_picked(int id, double x, double y) {
    m_current_id = id;
   
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {  

      if ((*it)->is_selected()) {
	//apply transformation  
	if ((x/60 + m_joint_x.change) < m_joint_x.max && 
            (x/60 + m_joint_x.change) > m_joint_x.min) {
	  m_joint_x.change += x/60;
          m_trans = Rotation(x/60, 'x' ) * m_trans;
          set_new_transform(Rotation(x/60, 'x' ) * current_trans);
	}
	if ((y/60 + m_joint_y.change) < m_joint_y.max && 
            (y/60 + m_joint_y.change) > m_joint_y.min) {
	  m_joint_y.change += y/60;
          m_trans = Rotation(y/60, 'z') * m_trans;
          set_new_transform(Rotation(y/60, 'z' ) * current_trans);
	}
	if (x != 0 && y != 0)
	  (*it)->set_moved();

      }

      (*it)->set_picked(m_current_id, x, y);
    }
  }
  
  virtual void push_transformation(std::vector<Matrix4x4> &trans_stack, 
                                   std::vector<int> &id_stack) {

    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {      
      if ((*it)->is_selected()) {
        trans_stack.push_back(current_trans);
        id_stack.push_back((*it)->get_id());
	reset_transform();
      }
      (*it)->push_transformation(trans_stack, id_stack);
    }
  }
  

  virtual bool pop_transformation(std::vector<Matrix4x4> &trans_stack, 
                                  std::vector<int> &id_stack,
                                  std::vector<Matrix4x4> &redo_stack,
                                  std::vector<int> &redo_ids) {
    bool done = false;
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      if (!id_stack.empty()) {
        if ((*it)->get_id() == id_stack.back()) {
          m_trans = trans_stack.back().invert() * m_trans;
          redo_stack.push_back(trans_stack.back());
          redo_ids.push_back(id_stack.back());
          trans_stack.pop_back();
          id_stack.pop_back();
          m_joint_x.change = m_joint_x.init;
          m_joint_y.change = m_joint_y.init;
          return true;
        }
      }
      done = (*it)->pop_transformation(trans_stack, id_stack, redo_stack, redo_ids);
      if (done)
        return true;
    }
  }

  virtual bool redo_transformation(std::vector<Matrix4x4> &redo_stack,
                                   std::vector<int> &redo_ids) {
    bool done = false;
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      if (!redo_ids.empty()) {
        if ((*it)->get_id() == redo_ids.back()) {
          m_trans = redo_stack.back() * m_trans;
          redo_stack.pop_back();
          redo_ids.pop_back();
          return true;
        }
      }
      done = (*it)->redo_transformation(redo_stack, redo_ids);
      if (done)
        return true;
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
    double min, init, max, change;
  };

  std::vector<double> x_stack,y_stack;

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

  virtual void reset_trans() {
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {      
      m_trans = m_init;
      (*it)->reset_trans();
    }
  }

  virtual void push_transformation(std::vector<Matrix4x4> &trans_stack, 
                                   std::vector<int> &id_stack) {
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      (*it)->push_transformation(trans_stack, id_stack);
    }
  }

  virtual bool pop_transformation(std::vector<Matrix4x4> &trans_stack, 
                                  std::vector<int> &id_stack,
                                  std::vector<Matrix4x4> &redo_stack,
                                  std::vector<int> &redo_ids) {
    Matrix4x4 tmp;
    bool done = false;
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      done = (*it)->pop_transformation(trans_stack, id_stack, redo_stack, redo_ids);
      if (done)
        return true;
    }
  }

  virtual bool redo_transformation(std::vector<Matrix4x4> &redo_stack,
                                   std::vector<int> &redo_ids) {
    Matrix4x4 tmp;
    bool done = false;
    for (ChildList::iterator it = m_children.begin(); it != m_children.end(); it++) {
      done = (*it)->redo_transformation(redo_stack, redo_ids);
      if (done)
        return true;
    }    
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
