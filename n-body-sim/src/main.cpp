#include "particle_manager.hpp"
#include "xorshift.hpp"
#include "basic_particle_force.hpp"

int main() {
	using namespace n_body;
	Xorshift32 rand{255};
	Basic_Particle_Force<2> force{ -10.0, 1.0, 10.0, 2 };
	float x_range = 10.0;
	float y_range = 10.0;
	Particle_Manager<2> pm{force, {{x_range, y_range}} };
	for (size_t i = 0; i < 10; i++) {
		pm.create_particle(1.0, 1.0,
			{{
				static_cast<float>(rand.rand_float() * (x_range - 2.0) - x_range + 1.0),
				static_cast<float>(rand.rand_float() * (y_range - 2.0) - y_range + 1.0)
			}},
			{{
				static_cast<float>(rand.rand_float() * 4.0 - 2.0),
				static_cast<float>(rand.rand_float() * 4.0 - 2.0)
			}}
		);
	}
	for (size_t i = 0; i < 240 * 3; i++) {
		pm.iterate(1.0 / 60.0);
		pm.print_particles();
	}
	return 0;
}
