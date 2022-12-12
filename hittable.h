#ifndef HITTABLE_H_
#define HITTABLE_H_

#include "ray.h"
#include "vec3.h"
#include "utils.h"

class material;
class metal;

struct hit_record{
    point3 p;
    vec3 normal;
    double t;
    bool front_face;
    shared_ptr<material> mat_ptr;

    inline void set_face_normal(const ray& r, const vec3& outward_normal){
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable{

    public:
        virtual bool hit(const ray& r, const double t_min, const double t_max, hit_record& rec) const = 0;

    public:
        shared_ptr<material> mat_ptr;
};

#endif