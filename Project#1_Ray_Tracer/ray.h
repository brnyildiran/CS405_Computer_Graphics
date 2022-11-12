#ifndef RAY_H
#define RAY_H

//==============================================================================================
// Originally written in 2020 by Peter Shirley <ptrshrl@gmail.com>
// “Ray Tracing in One Weekend.” raytracing.github.io/books/RayTracingInOneWeekend.html
//(accessed 11.06, 2022)
//==============================================================================================

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }

    point3 at(double t) const {
        return orig + t * dir;
    }

public:
    point3 orig;
    vec3 dir;
};

#endif