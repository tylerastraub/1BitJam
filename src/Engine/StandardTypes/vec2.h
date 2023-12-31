#ifndef VEC2_H
#define VEC2_H

#include <iostream>

namespace strb {
    struct vec2 {
        vec2() : x(0.f), y(0.f) {}
        vec2(float x, float y) : x(x), y(y) {}

        friend std::ostream& operator<<(std::ostream& os, const vec2& vec) {
            return os << "(" << vec.x << ", " << vec.y << ") ";
        }

        vec2 operator+(vec2 const& v) {
            return vec2(x + v.x, y + v.y);
        }

        vec2 operator+=(vec2 const& v) {
            x += v.x;
            y += v.y;
            return *this;
        }
        
        vec2 operator-(vec2 const& v) {
            return vec2(x - v.x, y - v.y);
        }
        
        vec2 operator-=(vec2 const& v) {
            x -= v.x;
            y -= v.y;
            return *this;
        }

        vec2 operator*(vec2 const& v) {
            return vec2(x * v.x, y * v.y);
        }

        vec2 operator*=(vec2 const& v) {
            x *= v.x;
            y *= v.y;
            return *this;
        }

        vec2 operator*(int const& i) {
            return vec2(x * i, y * i);
        }

        vec2 operator*=(int const& i) {
            x *= i;
            y *= i;
            return *this;
        }

        vec2 operator*(float const& f) {
            return vec2(x * f, y * f);
        }

        vec2 operator*=(float const& f) {
            x *= f;
            y *= f;
            return *this;
        }

        vec2 operator/(vec2 const& v) {
            return vec2(x / v.x, y / v.y);
        }

        vec2 operator/=(vec2 const& v) {
            x /= v.x;
            y /= v.y;
            return *this;
        }

        vec2 operator/(int const& i) {
            return vec2(x / i, y / i);
        }

        vec2 operator/=(int const& i) {
            x /= i;
            y /= i;
            return *this;
        }

        vec2 operator/(float const& f) {
            return vec2(x / f, y / f);
        }

        vec2 operator/=(float const& f) {
            x /= f;
            y /= f;
            return *this;
        }

        bool operator==(vec2 const& v) {
            return x == v.x && y == v.y;
        }

        bool operator!=(vec2 const& v) {
            return x != v.x || y != v.y;
        }

        float x;
        float y;
    };
};

#endif