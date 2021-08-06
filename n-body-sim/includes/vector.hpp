#pragma once
#include "real.hpp"
#include <cstddef>
#include <cmath>
#include <array>

namespace n_body {

template<size_t N>
class Vector {
public:
	Vector();
	Vector(std::array<real, N> v);
	real operator[](size_t i) const;
	real& operator[](size_t i);
	// inverse
	void invert();
	Vector<N> operator-() const;
	// vector addition
	Vector<N>  operator+ (const Vector<N>& v) const;
	Vector<N>& operator+=(const Vector<N>& v);
	// vector subtraction
	Vector<N>  operator- (const Vector<N>& v) const;
	Vector<N>& operator-=(const Vector<N>& v);
	// scalar multiplication
	Vector<N>  operator* (const real& s) const;
	Vector<N>& operator*=(const real& s);
	// dot product
	real    operator* (const Vector<N>& s) const;
	// magnitude
	real    sqr_magnitude() const;
	real    magnitude() const;
	// normalizes the vector
	void normalize();
	// returns a normalized copy of the vector
	Vector<N> normalized() const;
public:
	std::array<real, N> x;
};

template<size_t N>
Vector<N>::Vector() {
	std::fill(x.begin(), x.end(), 0);
}

template<size_t N>
Vector<N>::Vector(std::array<real, N> v) : x{v} {}

template<size_t N>
real Vector<N>::operator[](size_t i) const {
	return x[i];
}

template<size_t N>
real& Vector<N>::operator[](size_t i) {
	return x[i];
}

template<size_t N>
void Vector<N>::invert() {
	for (real& i : x) {
		i = -i;
	}
}

template<size_t N>
Vector<N> Vector<N>::operator-() const {
	Vector<N> out = *this;
	out.invert();
	return out;
}

template<size_t N>
Vector<N> Vector<N>::operator+(const Vector<N>& v) const {
	Vector<N> out;
	for (size_t i = 0; i < N; i++) {
		out[i] = x[i] + v[i];
	}
	return out;
}

template<size_t N>
Vector<N>& Vector<N>::operator+=(const Vector<N>& v) {
	for (size_t i = 0; i < N; i++) {
		x[i] += v[i];
	}
	return *this;
}

template<size_t N>
Vector<N> Vector<N>::operator-(const Vector<N>& v) const {
	Vector<N> out;
	for (size_t i = 0; i < N; i++) {
		out[i] = x[i] - v[i];
	}
	return out;
}

template<size_t N>
Vector<N>& Vector<N>::operator-=(const Vector<N>& v) {
	for (size_t i = 0; i < N; i++) {
		x[i] -= v[i];
	}
	return *this;
}

template<size_t N>
Vector<N> Vector<N>::operator*(const real& s) const {
	Vector<N> out;
	for (size_t i = 0; i < N; i++) {
		out[i] = x[i] * s;
	}
	return out;
}

template<size_t N>
Vector<N>& Vector<N>::operator*=(const real& s) {
	for (size_t i = 0; i < N; i++) {
		x[i] *= s;
	}
	return *this;
}

template<size_t N>
real Vector<N>::operator*(const Vector<N>& v) const {
	real out = 0.0;
	for (size_t i = 0; i < N; i++) {
		out += x[i] * v[i];
	}
	return out;
}

template<size_t N>
real Vector<N>::sqr_magnitude() const {
	return (*this) * (*this);
}

template<size_t N>
real Vector<N>::magnitude() const {
	return sqrt(sqr_magnitude());
}

template<size_t N>
void Vector<N>::normalize() {
	(*this) *= 1.0 / magnitude();
}

template<size_t N>
Vector<N> Vector<N>::normalized() const {
	return (*this) * (1.0 / magnitude());
}

}

#ifndef N_BODY_VECTOR_IMPL
namespace n_body {
	extern template class Vector<2>;
	extern template class Vector<3>;
}
#else
namespace n_body {
	template class Vector<2>;
	template class Vector<3>;
}
#endif
