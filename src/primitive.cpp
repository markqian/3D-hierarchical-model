#include "primitive.hpp"

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
  gluDeleteQuadric(m_Sphere);
}

Sphere::Sphere() {
  m_Sphere = gluNewQuadric();
}

void Sphere::walk_gl(bool picking) const
{
  gluSphere(m_Sphere,1,150,150); 
}
