#include "particle_manager.hpp"
#include "xorshift.hpp"
#include "basic_particle_force.hpp"
#include "olcPixelGameEngine.h"
#include <cctype>
#include <string>

n_body::real round_to_int(n_body::real a) {
	return static_cast<n_body::real>(static_cast<int>(a));
}

class Real_Gas : public olc::PixelGameEngine {
public:
	Real_Gas() :
		rand{255},
		force{ -000.0, 1.0, 1.0, 3 },
		x_range{10},
		y_range{10},
		pm{force, {{x_range, y_range}} }
	{
		sAppName = "Real Gas";
		for (size_t i = 0; i < 100; i++) {
			pm.create_particle(0.2, 1.0,
				{{
					static_cast<n_body::real>(rand.rand_float() * (x_range - 2.0) * 2.0 - x_range + 1.0),
					static_cast<n_body::real>(rand.rand_float() * (y_range - 2.0) * 2.0 - y_range + 1.0)
				}},
				{{
					static_cast<n_body::real>((rand.rand_float() * 2.0 - 1.0) * 5.0),
					static_cast<n_body::real>((rand.rand_float() * 2.0 - 1.0) * 5.0)
				}}
			);
			colors.emplace_back(
				(rand.get_rand() & 0b01111111) + 127,
				(rand.get_rand() & 0b01111111) + 127,
				(rand.get_rand() & 0b01111111) + 127
			);
			std::cout << "\x1b[48;2;" << std::to_string(colors.back().r) << ";" << std::to_string(colors.back().g) << ";" << std::to_string(colors.back().b) << "mParticle " << std::to_string(i) << "\x1b[m\n";
		}
	}

	bool OnUserCreate() override {
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		for (size_t i = 0; i < pm.particles.size(); i++) {
			const auto& particle = pm.particles[i];
			int x = ( (particle.x[0] / pm.boundary_rectangle[0]) + 1.0 ) / 2.0 * ScreenWidth();
			int y = ( (particle.x[1] / pm.boundary_rectangle[1]) + 1.0 ) / 2.0 * ScreenHeight();
			int radius = (particle.get_radius() / pm.boundary_rectangle[0] / 2.0) * ScreenWidth();
			FillCircle(x, y, radius, olc::BLACK);
		}
		pm.iterate(fElapsedTime);
		//Clear(olc::BLACK);
		for (size_t i = 0; i < pm.particles.size(); i++) {
			const auto& particle = pm.particles[i];
			int x = ( (particle.x[0] / pm.boundary_rectangle[0]) + 1.0 ) / 2.0 * ScreenWidth();
			int y = ( (particle.x[1] / pm.boundary_rectangle[1]) + 1.0 ) / 2.0 * ScreenHeight();
			int radius = (particle.get_radius() / pm.boundary_rectangle[0] / 2.0) * ScreenWidth();
			FillCircle(x, y, radius, colors[i]);
			FillCircle(x, y, radius * 0.75, olc::BLACK);
		}
		return true;
	}
private:
	n_body::Xorshift32 rand;
	n_body::Basic_Particle_Force<2> force;
	float x_range;
	float y_range;
	n_body::Particle_Manager<2> pm;
	std::vector<olc::Pixel> colors;
};

int main() {
	using namespace n_body;
	Real_Gas demo;
	if (demo.Construct(1024, 1024, 1, 1)) {
		demo.Start();
	}
	return 0;
}
