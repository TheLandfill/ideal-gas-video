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
		for (int i = 0; i < 5; i++) {
			DrawLine(i, i, ScreenWidth() - 1 - i, i, {220, 20, 60});
			DrawLine(i, i, i, ScreenHeight() - 1 - i, {220, 20, 60});
			DrawLine(ScreenWidth() - 1 - i, ScreenHeight() - 1 - i, i, ScreenHeight() - 1 - i, {220, 20, 60});
			DrawLine(ScreenWidth() - 1 - i, ScreenHeight() - 1 - i, ScreenWidth() - 1 - i, i, {220, 20, 60});
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		if (!move) {
			if (total_elapsed_time < 60.0) {
				for (size_t i = 0; i < pm.particles.size(); i++) {
					const auto& particle = pm.particles[i];
					int x = ( (particle.x[0] / pm.boundary_rectangle[0]) + 1.0 ) / 2.0 * (ScreenWidth() - 10);
					int y = ( (particle.x[1] / pm.boundary_rectangle[1]) + 1.0 ) / 2.0 * (ScreenHeight() - 10);
					int r = (particle.radius / pm.boundary_rectangle[0] / 2.0) * (ScreenWidth() - 10);
					x += 5;
					y += 5;
					FillCircle(x, y, r, colors[i]);
					FillCircle(x, y, r * 0.75, olc::BLACK);
				}
				total_elapsed_time += fElapsedTime;
				return true;
			} else {
				move = true;
				total_elapsed_time = 0.0;
			}
		}
		for (size_t i = 0; i < pm.particles.size(); i++) {
			auto& particle = pm.particles[i];
			int x = ( (particle.x[0] / pm.boundary_rectangle[0]) + 1.0 ) / 2.0 * (ScreenWidth() - 10);
			int y = ( (particle.x[1] / pm.boundary_rectangle[1]) + 1.0 ) / 2.0 * (ScreenHeight() - 10);
			int r = (particle.radius / pm.boundary_rectangle[0] / 2.0) * (ScreenWidth() - 10);
			x += 5;
			y += 5;
			FillCircle(x, y, r, olc::BLACK);
		}
		if (total_elapsed_time < 22.5) {
			;
		} else if (total_elapsed_time < 34.0) {
			for (auto& particle : pm.particles) {
				if (particle.radius <= 0.75) {
					particle.radius += fElapsedTime * 0.1 / (2.0 * sqrt(particle.radius));
				}
			}
		} else if (total_elapsed_time < 46.0) {
			for (auto& particle : pm.particles) {
				if (particle.radius >= 0.2) {
					particle.radius -= 0.5 * fElapsedTime / (2.0 * sqrt(particle.radius));
				}
				particle.v *= pow(0.125, fElapsedTime);
			}
			force.strength -= 15.0 * fElapsedTime;
			force.cut_off_dist += 8.0 * fElapsedTime / (2.0 * sqrt(force.cut_off_dist));
			pm.force = force;
		} else {
			if (total_elapsed_time < 46.01) {
				std::cout << "Final phase!\n";
			}
			if (force.strength < 0) {
				force.strength *= -0.125 / 4.0;
			}
			force.strength *= pow(0.125, fElapsedTime);
			force.cut_off_dist *= pow(0.5, fElapsedTime);
			pm.force = force;
		}
		pm.iterate(fElapsedTime);
		for (size_t i = 0; i < pm.particles.size(); i++) {
			const auto& particle = pm.particles[i];
			int x = ( (particle.x[0] / pm.boundary_rectangle[0]) + 1.0 ) / 2.0 * (ScreenWidth() - 10);
			int y = ( (particle.x[1] / pm.boundary_rectangle[1]) + 1.0 ) / 2.0 * (ScreenHeight() - 10);
			int r = (particle.radius / pm.boundary_rectangle[0] / 2.0) * (ScreenWidth() - 10);
			x += 5;
			y += 5;
			FillCircle(x, y, r, colors[i]);
			FillCircle(x, y, r * 0.75, olc::BLACK);
		}
		total_elapsed_time += fElapsedTime;
		return true;
	}
private:
	n_body::Xorshift32 rand;
	n_body::Basic_Particle_Force<2> force;
	float x_range;
	float y_range;
	n_body::Particle_Manager<2> pm;
	std::vector<olc::Pixel> colors;
	double total_elapsed_time = 0.0;
	bool move = false;
};

int main() {
	using namespace n_body;
	Real_Gas demo;
	if (demo.Construct(1024, 1024, 1, 1)) {
		demo.Start();
	}
	return 0;
}
