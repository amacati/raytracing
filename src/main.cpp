#include <iostream>
#include <functional>
#include <chrono>
#include <vector>

#include "vec3.h"
#include "ray.h"
#include "utils.h"
#include "sphere.h"
#include "hittable_list.h"
#include "camera.h"
#include "material.h"
#include "write_img.h"
#include "threading/threadpool.h"
#include "../lib/pngwriter/src/pngwriter.h"

color ray_color(const ray& r, const hittable& world, int depth) {
    thread_local hit_record rec;

    if (depth <= 0) return color(0,0,0);

    if (world.hit(r, 0.001, infinity, rec)) {
        thread_local ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            if (rec.mat_ptr->is_light()){
                return attenuation;
            }
            return attenuation * ray_color(scattered, world, depth-1);
        }
        return color(0, 0, 0);
    }
    //return color(0, 0, 0);
    thread_local auto unit_direction = unit_vector(r.direction());
    thread_local auto t = 0.5 * (unit_direction.y() + 1.);
    return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

hittable_list small_scene(){
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.05, 0.05, 0.35));
    world.add(make_shared<sphere>(point3(-4, 1, 1), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    // Add a light source
    //auto material4 = make_shared<light>(color(1, 1, 1), 4.0);
    //world.add(make_shared<sphere>(point3(0, 1, 3), 1, material4));

    // auto material5 = make_shared<light>(color(1, 1, 0.9), 1.0);
    // world.add(make_shared<sphere>(point3(0, 10000, 3), 9500, material5));

    return world;
}

int main(){
    // Image settings
    const auto aspect_ratio = 16./9.;
    const int img_width = 400;
    const int img_height = static_cast<int>(img_width / aspect_ratio);
    int samples_per_pixel = 100;
    int max_depth = 50;

    // World
    auto world = small_scene();

    // Camera
    point3 lookfrom(13,2,3);
    point3 lookat(0,0,0);
    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    // Create threadpool for multiprocessing
    uint32_t num_threads = 12;
    ThreadPool threadpool(num_threads);
    threadpool.start();

    // Render
    std::vector<vec3> pixel_colors;
    pixel_colors.resize(img_height * img_width);
    pixel_colors[0] = vec3(0, 0, 0);
    for (int j = img_height - 1; j >= 0; --j) {
        std::function<void()> fun = [&pixel_colors, j, samples_per_pixel, &cam, &world, max_depth] {
            for (int i = 0; i < img_width; ++i){
                vec3 pixel_color(0, 0, 0);
                for (int s = 0; s != samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (img_width - 1);
                    auto v = (j + random_double()) / (img_height - 1);
                    pixel_color += ray_color(cam.get_ray(u, v), world, max_depth);
                }   
                pixel_colors[j * img_width + i] = pixel_color;
            }
        };
        threadpool.add_job(fun);
    }

    while (threadpool.busy()){
        std::cout << "\rScanlines remaining: " << threadpool.num_jobs() << " " << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    threadpool.stop();
    std::cout << "\rScanlines remaining: 0 " << std::flush;  // Set counter to 0 after finishing
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Processing time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;

    pngwriter png(img_width, img_height, 0., "rendering.png");
    write_img(pixel_colors, samples_per_pixel, png);
    std::cout << "Done.\n";
}