#include "particle_manager.hpp"
#include "basic_particle_force.hpp"

int main() {
	using namespace n_body;
	Basic_Particle_Force<2> force{ -10.0, 1.0, 10.0, 2 };
	Particle_Manager<2> pm{force};
	pm.create_particle(1.0, 1.0, std::array<real, 2>{ 0, 0 });
	pm.create_particle(1.0, 1.0, std::array<real, 2>{ 3, 0 });
	for (size_t i = 0; i < 240; i++) {
		pm.iterate(1.0 / 60.0);
		pm.print_particles();
	}
	return 0;
}
