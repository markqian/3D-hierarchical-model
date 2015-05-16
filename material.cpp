#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  GLfloat materialColor[] = {(float)m_kd.R(), (float)m_kd.G(), (float)m_kd.B(), 1.0};
  GLfloat materialSpecular[] = {(float)m_ks.R(), (float)m_ks.G(), (float)m_ks.B(), 1.0};

  //The color emitted by the material
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
  glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, m_shininess);
  // Perform OpenGL calls necessary to set up this material.
}






