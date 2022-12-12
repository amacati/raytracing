#include <vector>
#include "vec3.h"
#include "../lib/pngwriter/src/pngwriter.h"


void write_img(std::vector<vec3>& pixel_colors, const uint32_t samples_per_pixel, pngwriter& png) {
    auto img_height = png.getheight();
    auto img_width = png.getwidth();
    for (int j = img_height - 1; j >= 0; --j) {
        for (int i = 0; i < img_width; ++i){
            auto& pixel_color = pixel_colors[j * img_width + i];
            auto r = pixel_color.x();
            auto g = pixel_color.y();
            auto b = pixel_color.z();
            auto scale = 1. / samples_per_pixel;
            r = sqrt(scale * r);
            g = sqrt(scale * g);
            b = sqrt(scale * b);
            png.plot(i, j, r, g, b);
        }
    }
    png.close();
}
