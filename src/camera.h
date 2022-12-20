#ifndef CAMERA_H_
#define CAMERA_H_

#include "vec3.h"
#include "ray.h"

class camera {
    public:
        // Vertical FoV in degrees
        camera(point3 lookfrom,
               point3 lookat,
               vec3 vup,
               double vfov,
               double aspect_ratio,
               double aperture,
               double focus_dist) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta/2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

            lens_radius = aperture / 2;
        }

        ray get_ray(double s, double t) const {
            vec3 rd = lens_radius * random_in_unit_disk();
            //static const __mmask8 x_mask = 0x01 << 3;
            //static const __mmask8 y_mask = 0x01 << 2;
            //auto u_offset = _mm256_mask_mul_pd(u.e, x_mask, u.e, rd.e);
            //vec3 offset = _mm256_add_pd(u_offset, _mm256_mask_mul_pd(v.e, y_mask, v.e, rd.e));
            vec3 offset = u * rd.x() + v * rd.y();
            return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset);
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};

#endif