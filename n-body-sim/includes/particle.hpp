#pragma once
#include "vector.hpp"

namespace n_body {

template<size_t N>
class Particle {
public:
	Particle(real r, real inv_m);
	// semi-implicit Euler
	void iterate(real delta_t);
	// Adds a force to the accumlated_force
	void add_force(const Vector<N>& f);
	// Handles impulse from bouncing off another particle
	void add_elastic_impulse(Particle<N>& p);
	// Handles impulse from bouncing off a wall
	void add_boundary(Vector<N>& v);
	// Used for finding bounding boxes
	real get_greatest_point(size_t dim) const;
	real get_smallest_point(size_t dim) const;
	// Used for full collision
	real get_radius() const;
public:
	Vector<N> x;
	Vector<N> v;
private:
	real radius = 1.0;
	Vector<N> accumulated_force;
	real inverse_mass = 1.0;
};

template<size_t N>
Particle<N>::Particle(real r, real inv_m) : radius(r), inverse_mass(inv_m) {}

template<size_t N>
void Particle<N>::iterate(real delta_t) {
	auto acceleration = accumulated_force * inverse_mass;
	v += acceleration * delta_t;
	x += v * delta_t;
	accumulated_force *= 0.0;
}

template<size_t N>
void Particle<N>::add_force(const Vector<N>& f) {
	accumulated_force += f;
}

template<size_t N>
void Particle<N>::add_elastic_impulse(Particle<N>& o) {
	real double_reduced_mass = 2.0 / (inverse_mass + o.inverse_mass);
	Vector<N> normal = x - o.x;
	normal.normalize();
	Vector<N> vel_normal = normal * ((o.v - v) * normal) * double_reduced_mass;
	v   += vel_normal * inverse_mass;
	o.v -= vel_normal * o.inverse_mass;
}

template<size_t N>
void Particle<N>::add_boundary(Vector<N>& v) {
	
}

template<size_t N>
real Particle<N>::get_greatest_point(size_t dim) const {
	return x[dim] + radius;
}

template<size_t N>
real Particle<N>::get_smallest_point(size_t dim) const {
	return x[dim] - radius;
}

template<size_t N>
real Particle<N>::get_radius() const {
	return radius;
}

}

#ifndef N_BODY_PARTICLE_IMPL
namespace n_body {
	extern template class Particle<2>;
	extern template class Particle<3>;
}
#else
namespace n_body {
	template class Particle<2>;
	template class Particle<3>;
}
#endif
