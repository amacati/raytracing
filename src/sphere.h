#ifndef SPHERE_H_
#define SPHERE_H_

#include <cmath>
#include "hittable.h"
#include "material.h"

class sphere : public hittable{

    public:
        sphere() {}
        sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m){};

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    point3 ac = r.origin() - center;
    double half_b = dot(r.direction(), ac);
    double a = r.direction().length_squared();
    double c = ac.length_squared() - radius * radius;
    auto discriminant = half_b*half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }
    rec.t = root;
    rec.p = r.at(rec.t);
    rec.mat_ptr = mat_ptr;
    auto outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return true;
}
#endif