#ifndef CS488_A2_HPP
#define CS488_A2_HPP

#include "algebra.hpp"

// You should implement these functions, and use them from viewer.cpp

// Return a matrix to represent a counterclockwise rotation of "angle"
// degrees around the axis "axis", where "axis" is one of the
// characters 'x', 'y', or 'z'.
Matrix4x4 Rotation(double angle, char axis);

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 Translation(const Vector3D& displacement);

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 Scaling(const Vector3D& scale);

#endif
