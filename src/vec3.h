#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <immintrin.h>
#include <vector>
#include "utils.h"

using std::sqrt;

class vec3{

    public:
        vec3() {
            e = _mm256_set_pd(0, 0, 0, 0);
        }

        vec3(double e0, double e1, double e2) {
            e = _mm256_set_pd(0, e2, e1, e0);
            
        }

        vec3(const __m256d& o) {
            e = o;
        }

        vec3 operator-() const {
            return vec3(e);
        }

        vec3& operator+=(const vec3& v) {
            e = _mm256_add_pd(e, v.e);
            return *this;
        }

        vec3& operator*=(const double t) {
            e = _mm256_mul_pd(e, _mm256_set1_pd(t));
            return *this;
        }

        vec3& operator/=(const double t) {
            e = _mm256_div_pd(e, _mm256_set1_pd(t));
            return *this;
        }

        double length() {
            return sqrt(length_squared());
        }

        double length_squared() {
            __m256d e_pow2 = _mm256_mul_pd(e, e);
            __m128d vlow  = _mm256_castpd256_pd128(e_pow2);
            __m128d vhigh = _mm256_extractf128_pd(e_pow2, 1); // high 128
            vlow  = _mm_add_pd(vlow, vhigh);     // reduce down to 128
            __m128d high64 = _mm_unpackhi_pd(vlow, vlow);  // Store top element in bottom
            return  _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
        }

        static vec3 random() {
            return vec3(random_double(), random_double(), random_double());
        } 

        static vec3 random(double min, double max) {
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
        }

        bool near_zero() {
            const auto s = 1e-8;
            static const __m256d sign_mask = _mm256_set1_pd(-0.); // -0. = 1 << 63
            __m256d e_abs = _mm256_andnot_pd(sign_mask, e);  // Take absolute values
            __m256d near_zero_mask = _mm256_cmp_pd(e_abs, _mm256_set1_pd(s), _CMP_LT_OQ);
            return _mm256_movemask_pd(near_zero_mask) == 15;  // All registers set
        }

        std::vector<double> v2vec() const {
            std::vector<double> v;
            v.resize(3);
            __m128d vlow  = _mm256_castpd256_pd128(e);
            __m128d vhigh = _mm256_extractf128_pd(e, 1); // high 128
            v.at(0) = _mm_cvtsd_f64(vlow);
            v.at(1) = _mm_cvtsd_f64(_mm_unpackhi_pd(vlow, vlow));
            v.at(2) = _mm_cvtsd_f64(vhigh);
            return v;
        }

        double y() const {
            __m128d tmp = _mm256_castpd256_pd128(e);
            return _mm_cvtsd_f64(_mm_unpackhi_pd(tmp, tmp));
        }

        double x() const{
            return _mm_cvtsd_f64(_mm256_castpd256_pd128(e));
        }

    public:
        alignas(alignof(__m256d)) __m256d e;
};

// Define type aliases to clarify use
using point3 = vec3;  // 3D point
using color = vec3;   // RGB tuple

// vec3 Utility Functions
inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    auto v_tmp = v.v2vec();
    return out << v_tmp[0] << ' ' << v_tmp[1] << ' ' << v_tmp[2];
}

inline vec3 operator+(const vec3& u, const vec3& v){
    return vec3(_mm256_add_pd(u.e, v.e));
}

inline vec3 operator+(const vec3& v, const double t){
    return vec3(_mm256_add_pd(v.e, _mm256_set1_pd(t)));
}

inline vec3 operator-(const vec3& u, const vec3& v){
    return vec3(_mm256_sub_pd(u.e, v.e));
}

inline vec3 operator*(const vec3& u, const vec3& v){
    return vec3(_mm256_mul_pd(u.e, v.e));
}

inline vec3 operator*(const double t, const vec3& v){
    return vec3(_mm256_mul_pd(_mm256_set1_pd(t), v.e));
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, const double t) {
    return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v){
    __m256d x = _mm256_mul_pd(u.e, v.e);
    __m128d vlow  = _mm256_castpd256_pd128(x);
    __m128d vhigh = _mm256_extractf128_pd(x, 1); // high 128
    vlow  = _mm_add_pd(vlow, vhigh);     // reduce down to 128
    __m128d high64 = _mm_unpackhi_pd(vlow, vlow);  // Store top element in bottom
    return  _mm_cvtsd_f64(_mm_add_sd(vlow, high64));  // reduce to scalar
}

inline vec3 cross(const vec3& u, const vec3& v){
    __m256d tmp0 = _mm256_mul_pd(u.e, _mm256_permute4x64_pd(v.e, _MM_SHUFFLE(3, 0, 2, 1)));
    __m256d tmp1 = _mm256_mul_pd(v.e, _mm256_permute4x64_pd(u.e, _MM_SHUFFLE(3, 0, 2, 1)));
    return vec3(_mm256_permute4x64_pd(_mm256_sub_pd(tmp0, tmp1), _MM_SHUFFLE(3, 0, 2, 1)));
}

inline vec3 unit_vector(vec3 v){
    return v / v.length();
}

vec3 random_in_unit_sphere() {
    // https://math.stackexchange.com/questions/87230/picking-random-points-in-the-volume-of-sphere-with-uniform-probability
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() < 1) return p;
    }
}

vec3 random_unit_vector() {
    auto x = randn_double();
    auto y = randn_double();
    auto z = randn_double();
    // Just in case all numbers are 0, should almost never happen
    if (x == 0 && y == 0 && z == 0) {
        x = 1.0;
    }
    return unit_vector(vec3(x, y, z));
}

vec3 random_in_unit_disk() {
    auto theta = random_double() * 2 * pi;
    return vec3(cos(theta), sin(theta), 0);
}

vec3 reflect(const vec3& v, const vec3& n) {
    auto tmp = 2 * dot(v, n);
    return vec3(-_mm256_fmsub_pd(_mm256_set1_pd(tmp), n.e, v.e));
}

vec3 refract(const vec3& uv, const vec3& n, double eta_i_over_eta_t) {
    auto cos_theta = fmin(dot(-uv, n), 1.);
    vec3 r_out_perp = eta_i_over_eta_t * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

#endif