#pragma once
#include "particle.hpp"
#include "xorshift.hpp"
#include <iostream>
#include <cstddef>
#include <vector>
#include <array>
#include <algorithm>

namespace n_body {

struct Possible_Collision {
	size_t a, b;
};

// For this system to work, all particles must be added onto the end of the list
template<size_t N>
class Collision {
public:
	Collision(size_t seed = 1);
	void handle_collisions(std::vector<Particle<N>>& particles);
	void remove_particles(std::vector<size_t>& indices);
private:
	void broad_collision_detection(const std::vector<Particle<N>>& particles);
	void sweep_and_prune(const std::vector<Particle<N>>& particles);
	void update_pos(const std::vector<Particle<N>>& particles);
	void select_random_index();
private:
	size_t cur_index = 0;
	std::array<std::vector<size_t>, N> sorted_pos;
	std::array<size_t, N> num_collisions_first_pass;
	std::vector<Possible_Collision> possible_collisions;
	std::vector<size_t> collisions_to_remove;
	Xorshift32 rand;
};

template<size_t N>
Collision<N>::Collision(size_t seed) : rand(seed) {
	std::fill(num_collisions_first_pass.begin(), num_collisions_first_pass.end(), 1);
}

template<size_t N>
void Collision<N>::handle_collisions(std::vector<Particle<N>>& particles) {
	possible_collisions.clear();
	broad_collision_detection(particles);
	for (const auto& pc : possible_collisions) {
		Particle<N>& a = particles[pc.a];
		Particle<N>& b = particles[pc.b];
		real sqr_dist = (a.x - b.x).sqr_magnitude();
		real radius_sum = a.get_radius() + b.get_radius();
		radius_sum *= radius_sum;
		if (radius_sum >= sqr_dist) {
			a.add_elastic_impulse(b);
			std::cout << "Collision happened!\n";
		}
	}
}

template<size_t N>
void Collision<N>::remove_particles(std::vector<size_t>& indices) {
	std::sort(indices.begin(), indices.end());
	std::vector<size_t> new_map(sorted_pos[0].size(), (size_t)-1);
	size_t offset = 0;
	for (size_t i = 0; i < new_map.size(); i++) {
		if (offset < indices.size() && i == indices[offset]) {
			offset++;
			continue;
		}
		new_map[i] = i + offset;
	}
	for (auto& dim : sorted_pos) {
		for (auto& i : dim) {
			i = new_map[i];
		}
		dim.erase(std::remove(dim.begin(), dim.end(), (size_t)-1), dim.end());
	}
}

template<size_t N>
void Collision<N>::broad_collision_detection(const std::vector<Particle<N>>& particles) {
	sweep_and_prune(particles);
	if (!possible_collisions.empty()) {
		std::cout << "Broad collision detected!\n";
	}
}

template<size_t N>
void Collision<N>::sweep_and_prune(const std::vector<Particle<N>>& particles) {
	possible_collisions.clear();
	possible_collisions.reserve(N * particles.size());
	update_pos(particles);
	auto & cur_dim = sorted_pos[cur_index];
	num_collisions_first_pass[cur_index] = 0;
	for (size_t i = 0; i < cur_dim.size(); i++) {
		real greatest_point = particles[i].get_greatest_point(cur_index);
		for (size_t j = i + 1; j < cur_dim.size(); j++) {
			real smallest_point = particles[j].get_smallest_point(cur_index);
			if (smallest_point <= greatest_point) {
				possible_collisions.push_back({i, j});
				num_collisions_first_pass[cur_index] += 1;
			}
		}
	}
	collisions_to_remove.clear();
	collisions_to_remove.reserve(possible_collisions.size());
	for (size_t i = 0; i < possible_collisions.size(); i++) {
		const auto& pc = possible_collisions[i];
		for (size_t dim = 1; dim < N; dim++) {
			size_t adj_dim = (dim + cur_index) % N;
			const Particle<N>& a = particles[pc.a];
			const Particle<N>& b = particles[pc.b];
			real middle_point_a = a.x[adj_dim];
			real middle_point_b = b.x[adj_dim];
			real smallest_point_a = a.get_smallest_point(adj_dim);
			real greatest_point_b = b.get_greatest_point(adj_dim);
			if (
				(middle_point_a > middle_point_b)
				&& (smallest_point_a > greatest_point_b)
			) {
				collisions_to_remove.push_back(i);
				break;
			}
			real greatest_point_a = a.get_greatest_point(adj_dim);
			real smallest_point_b = b.get_smallest_point(adj_dim);
			if (
				(middle_point_a <= middle_point_b)
				&& (greatest_point_a < smallest_point_b)
			) {
				collisions_to_remove.push_back(i);
				break;
			}
		}
	}
	size_t offset = 0;
	for (size_t i = 0; i < possible_collisions.size(); i++) {
		if (offset >= collisions_to_remove.size() || i == collisions_to_remove[offset]) {
			offset++;
			continue;
		}
		possible_collisions[i - offset] = possible_collisions[i];
	}
	possible_collisions.resize(possible_collisions.size() - collisions_to_remove.size());
	select_random_index();
}

template<size_t N>
void Collision<N>::update_pos(const std::vector<Particle<N>>& particles) {
	std::sort(
		sorted_pos[cur_index].begin(),
		sorted_pos[cur_index].end(),
		[&](const size_t& a, const size_t& b){
			if (a >= particles.size()) {
				return false;
			}
			if (b >= particles.size()) {
				return true;
			}
			const Particle<N>& pa = particles[a];
			const Particle<N>& pb = particles[b];
			return (pa.x[cur_index] - pa.get_radius()) < (pb.x[cur_index] - pb.get_radius());
		}
	);
	sorted_pos[cur_index].resize(particles.size());
}

template<size_t N>
void Collision<N>::select_random_index() {
	size_t max_element = num_collisions_first_pass[0];
	for (size_t i = 1; i < N; i++) {
		if (max_element < num_collisions_first_pass[i]) {
			max_element = num_collisions_first_pass[i];
		}
	}
	max_element *= 1.5;
	size_t max_value = 0;
	std::array<size_t, N> rand_array;
	size_t rand_offset = rand.get_rand() % N;
	for (size_t i = 0; i < N; i++) {
		rand_array[(i + rand_offset) % N] = max_element - num_collisions_first_pass[i];
		max_value += rand_array[(i + rand_offset) % N];
	}
	size_t index = rand.get_rand() % max_value;
	index += 1;
	size_t cur_step = 0;
	cur_index = 0;
	for (; cur_index < N && cur_step < index; cur_index++) {
		cur_step += rand_array[cur_index];
	}
	cur_index += N - rand_offset;
	cur_index %= N;
}

}

#ifndef N_BODY_COLLISION_IMPL
namespace n_body {
	extern template class Collision<2>;
	extern template class Collision<3>;
}
#else
namespace n_body {
	template class Collision<2>;
	template class Collision<3>;
}
#endif
