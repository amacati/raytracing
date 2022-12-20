#include <vector>
#include "immintrin.h"
#include "vec3.h"
#include "../lib/pngwriter/src/pngwriter.h"


void write_img(std::vector<vec3>& pixel_colors, const uint32_t samples_per_pixel, pngwriter& png) {
    auto img_height = png.getheight();
    auto img_width = png.getwidth();
    for (int j = img_height - 1; j >= 0; --j) {
        for (int i = 0; i < img_width; ++i){
            auto& pixel_color = pixel_colors[j * img_width + i];
            auto scale = 1. / samples_per_pixel;
            auto rgb = vec3(_mm256_sqrt_pd(_mm256_mul_pd(pixel_color.e, _mm256_set1_pd(scale))));
            std::vector<double> rgb_vec = rgb.v2vec();
            png.plot(i, j, rgb_vec[0], rgb_vec[1], rgb_vec[2]);
        }
    }
    png.close();
}
