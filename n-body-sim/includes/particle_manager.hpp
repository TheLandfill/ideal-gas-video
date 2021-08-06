#pragma once
#include "collision.hpp"
#include "particle.hpp"
#include <cstddef>
#include <iostream>
#include <iomanip>
#include <functional>

namespace n_body {

template<size_t N>
class Particle_Manager {
public:
	Particle_Manager(std::function<void(Particle<N>&, Particle<N>&)> force_func);
	void create_particle(const real radius, const real inverse_mass, const Vector<N>& pos, const Vector<N>& vel = {});
	void iterate(real delta_t);
	void remove_particle(size_t index);
	void remove_particles(std::vector<size_t>& indices);
	void print_particles();
private:
	void apply_forces();
	void handle_collisions();
private:
	std::function<void(Particle<N>&, Particle<N>&)> force;
	std::vector<Particle<N>> particles;
	Collision<N> collision_handler;
};

template<size_t N>
Particle_Manager<N>::Particle_Manager(std::function<void(Particle<N>&, Particle<N>&)> force_func) : force(force_func) {}

template<size_t N>
void Particle_Manager<N>::create_particle(const real radius, const real inverse_mass, const Vector<N>& pos, const Vector<N>& vel) {
	particles.emplace_back(radius, inverse_mass);
	particles.back().x = pos;
	particles.back().v = vel;
}

template<size_t N>
void Particle_Manager<N>::iterate(real delta_t) {
	apply_forces();
	for (auto& particle : particles) {
		particle.iterate(delta_t);
	}
	handle_collisions();
}

template<size_t N>
void Particle_Manager<N>::remove_particle(size_t index) {
	std::vector<size_t> indices{};
	indices.push_back(index);
	remove_particles(indices);
}

template<size_t N>
void Particle_Manager<N>::remove_particles(std::vector<size_t>& indices) {
	std::sort(indices.begin(), indices.end());
	collision_handler.remove_particles(indices);
	int i = 0;
	for (size_t j = 0; j < particles.size(); j++) {
		if (j != indices[i]) {
			particles[j - i] = particles[j];
		} else {
			i++;
		}
	}
	for (size_t i = 0; i < indices.size(); i++) {
		particles.pop_back();
	}
}

template<size_t N>
void Particle_Manager<N>::print_particles() {
	for (const auto& particle : particles) {
		std::cout << "[" << &particle << "]: (";
		for (size_t i = 0; i < N - 1; i++) {
			std::cout << std::fixed << std::setprecision(6) << particle.x[i] << ", ";
		}
		std::cout << std::fixed << std::setprecision(6) << particle.x[N - 1] << ")\t\t\t(";
		for (size_t i = 0; i < N - 1; i++) {
			std::cout << std::fixed << std::setprecision(6) << particle.v[i] << ", ";
		}
		std::cout << std::fixed << std::setprecision(6) << particle.v[N - 1] << ")\n";
	}
	std::cout << "\n";
}

template<size_t N>
void Particle_Manager<N>::apply_forces() {
	for (size_t i = 0; i < particles.size(); i++) {
		for (size_t j = i + 1; j < particles.size(); j++) {
			force(particles[i], particles[j]);
		}
	}
}

template<size_t N>
void Particle_Manager<N>::handle_collisions() {
	collision_handler.handle_collisions(particles);
}

}

#ifndef N_BODY_PARTICLE_MANAGER_IMPL
namespace n_body {
	extern template class Particle_Manager<2>;
	extern template class Particle_Manager<3>;
}
#else
namespace n_body {
	template class Particle_Manager<2>;
	template class Particle_Manager<3>;
}
#endif
