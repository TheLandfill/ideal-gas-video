#pragma once
#include "particle.hpp"
#include <cmath>

namespace n_body {

template<size_t N>
class Basic_Particle_Force {
public:
	Basic_Particle_Force(real strength, real offset, real cut_off_dist, size_t power);
	void operator()(Particle<N>& a, Particle<N>& b);
private:
	real strength;
	real offset;
	real cut_off_dist;
	size_t power;
};

template<size_t N>
Basic_Particle_Force<N>::Basic_Particle_Force(real s, real o, real cod, size_t p) : strength(s), offset(fabs(o)), cut_off_dist(cod), power(p) {}

template<size_t N>
void Basic_Particle_Force<N>::operator()(Particle<N>& a, Particle<N>& b) {
	Vector<N> direction = a.x - b.x;
	real dist = direction.magnitude();
	direction *= (1.0 / dist);
	if (dist >= cut_off_dist) {
		return;
	}
	dist += offset;
	real denom = 1.0;
	real dist_pow = dist;
	size_t cur_power = power;
	while (cur_power > 0) {
		if (cur_power & 1) {
			denom *= dist_pow;
		}
		cur_power >>= 1;
		dist_pow *= dist_pow;
	}
	Vector<N> force = direction * (strength / denom);
	a.add_force(force);
	b.add_force(-force);
}

}

#ifndef N_BODY_BASIC_PARTICLE_FORCE_IMPL
namespace n_body {
	extern template class Basic_Particle_Force<2>;
	extern template class Basic_Particle_Force<3>;
}
#else
namespace n_body {
	template class Basic_Particle_Force<2>;
	template class Basic_Particle_Force<3>;
}
#endif
