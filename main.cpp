#include <iostream>
#include <fstream>
#include "vec3.h"
#include "ray.h"

using namespace std;

bool hit_sphere(const point3& center, double radius, const ray& r) {
 vec3 oc = r.origin() - center;
 auto a = dot(r.direction(), r.direction());
 auto b = 2.0 * dot(oc, r.direction());
 auto c = dot(oc, oc) - radius*radius;
 auto discriminant = b*b - 4*a*c;
 return (discriminant > 0);
}
color ray_color(const ray& r) {
 if (hit_sphere(point3(0,0,-1), 0.5, r))
 return color(1, 0, 0);
 vec3 unit_direction = unit_vector(r.direction());
 auto t = 0.5*(unit_direction.y() + 1.0);
 return (1.0-t)*color(1.0, 1.0, 1.0) + t*color(0.5, 0.7, 1.0);
}

void write_color(ofstream& file, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    file << static_cast<int>(255.999 * pixel_color.x()) << ' '
         << static_cast<int>(255.999 * pixel_color.y()) << ' '
         << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

int main() {
    const int image_width = 800;
    const int image_height = 400;

    ofstream file("output.ppm");
    file << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    point3 origin(0.0, 0.0, 0.0);
    double viewport_height = 2.0;
    double viewport_width = 4.0;
    double focal_length = 1.0;

    vec3 horizontal(viewport_width, 0.0, 0.0);
    vec3 vertical(0.0, viewport_height, 0.0);
    vec3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width-1);
            auto v = double(j) / (image_height-1);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            color pixel_color = ray_color(r);
            write_color(file, pixel_color);
        }
    }

    file.close();
    return 0;
}
