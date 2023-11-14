#include "rtweekend.h"      // Inclui um arquivo de cabeçalho com funções e constantes úteis.
#include "color.h"          // Inclui um arquivo de cabeçalho para lidar com cores.
#include "hittable_list.h"  // Inclui um arquivo de cabeçalho para a lista de objetos atingíveis.
#include "sphere.h"         // Inclui um arquivo de cabeçalho para lidar com esferas.
#include "camera.h"
#include "material.h"
#include <iostream>         // Inclui a biblioteca de entrada/saída padrão do C++.
#include <thread>

color ray_color(const ray& r, const hittable_list* world, int depth) {
    hit_record rec;
    // Se excedemos o limite de reflexões do raio, não há mais luz coletada.
    if (depth <= 0)
        return color(0, 0, 0);
    if (world->hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

hittable_list random_scene() {
    hittable_list world;
    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;
                if (choose_mat < 0.8) {
                    // difuso
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // vidro
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

void trace_lines(int thread_id, int start_line, int end_line, int image_width, int image_height, camera* cam, const hittable_list* world, image* img) {
    const int samples_per_pixel = 200;
    const int max_depth = 50;

    cout << "starting thread: " << thread_id << " - lines " << start_line << " -> " << end_line << endl;

    for (int j = start_line; j < end_line; j++) {
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam->get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            img->set(i, image_height - j, pixel_color, samples_per_pixel);
        }
        cout << "\rLine[" << setw(2) << thread_id << "]: " << setw(4) << j << flush;
    }
    cout << endl << "Thread " << thread_id << " ended." << endl << flush;
}


int main() {
    // Imagem
    const auto aspect_ratio = 4.0 / 3.0;  // Updated aspect ratio for 800x600
    const int image_width = 800;          // Updated width
    const int image_height = 600;         // Updated height
    const int samples_per_pixel = 100;
    const int max_depth = 50;
    const int threads = 12;
    const int lines_threads = image_height / threads;
    int start = 0;
    thread* threads_vec = new thread[threads];

    image img = image(image_width, image_height);


    // Mundo
    auto world = random_scene();

    // Câmera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;
    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    //Threads
    for (int i = 0; i < threads; ++i) {
        if (i + 1 == threads)
            threads_vec[i] = thread(trace_lines, i, start, image_height, image_width, image_height, &cam, &world, &img);
        else
            threads_vec[i] = thread(trace_lines, i, start, start + lines_threads, image_width, image_height, &cam, &world, &img);
        start += lines_threads;
    }
    cout << "waiting threads to finish..." << endl;

    for (int i = 0; i < threads; ++i) {
        threads_vec[i].join();
    }

    //Render
    ofstream outfile;
    outfile.open("C:\\Users\\roger\\OneDrive\\Documentos\\Faculdade\\8periodo\\topicosEspeciais2\\imagemGerada\\test1.bmp", ios::binary | ios::out);
    img.writeBitmapFile(outfile);
    outfile.close();



    std::cerr << "\nDone.\n";
}