#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include <iostream>
#include "camera.h"
#include "material.h"
#include "box.h"

//return (1.0 - t) * color(255, 212, 23) + t * color(135, 23, 255); background 

//Before Emissive Materials
/*
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 0.83, 0.09) + t * color(0.52, 0.09, 1.0);
}
*/

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}

int main() {

    // Image
    //const auto aspect_ratio = 4.0 / 3.0;
    //const int image_width = 800;
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1920;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    //Blackout
    //color background(0, 0, 0);
    
    //Navy Room
    color background(0.0, 0.0, 0.4);

    // World
    hittable_list world;

    // Materials
    auto material_ground = make_shared<lambertian>(color(0.0, 0.0, 0.6));

    //Metal
    auto metal_gold = make_shared<metal>(color(1.0, 0.95, 0.0),0.15);
    auto metal_green = make_shared<metal>(color(0.37, 1.0, 0.37),0.075);

    //Lambert
    auto material_orange = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_aqua = make_shared<lambertian>(color(0.2, 1.0, 1.0));
    auto material_pink = make_shared<lambertian>(color(1.0, 0.6, 1.0));
    auto material_lambert = make_shared<lambertian>(color(1.0, 1.0, 0.4));

    //Glass
    auto material_glass = make_shared<dielectric>(2.5);

    //Diffuse Light
    auto light_green = make_shared<diffuse_light>(color(0.4, 0.9, 0.1));
    auto light_moon = make_shared<diffuse_light>(color(1.0, 1.0, 0.4));
    auto light_orange = make_shared<diffuse_light>(color(0.7, 0.3, 0.3));
    auto light_pink = make_shared<diffuse_light>(color(1.0, 0.6, 1.0));


    /*
    MAIN OBJECTS

    //Ground
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    
    //1
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.175, material_orange));
    
    //Glass 1
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.175, material_glass));
    
    //Hollow Glass 1
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), -0.175, material_glass));

    //Orange Light 1
    //world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.175, light_orange));
    
    //2
    world.add(make_shared<sphere>(point3(-0.6, 0.2, -1.0), 0.05, metal_gold));
    
    //3
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.4, metal_green));

    //Green Light 3
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.4, light_green));
    
    //4
    world.add(make_shared<sphere>(point3(0.5, 0.4, -1.0), 0.1, metal_gold));
    
    //5
    //world.add(make_shared<sphere>(point3(0.95, 0.1, -1.0), 0.25, material_lambert));

    //Orange Light 5
    world.add(make_shared<sphere>(point3(0.95, 0.1, -1.0), 0.25, light_orange));

    //Blue Cube Lambert
    //world.add(make_shared<box>(point3(0, 0, 0), point3(0.2, 0.2, 0.2), material_aqua));
    
    //Glass Cube
    //world.add(make_shared<box>(point3(0, 0, 0), point3(0.2, 0.2, 0.2), material_glass));

    //Yellow Light Cube
    world.add(make_shared<box>(point3(0, 0, 0), point3(0.2, 0.2, 0.2), light_moon));
    
    //Pink Rectangle Prism Lambert
    //world.add(make_shared<box>(point3(-0.15, -0.45, -0.15), point3(0.0, 0.0, 0.0), material_pink));

    //Pink Light Rectangle Prism
    world.add(make_shared<box>(point3(-0.15, -0.45, -0.15), point3(0.0, 0.0, 0.0), light_pink));

    //Metal Green
    world.add(make_shared<sphere>(point3(-0.4, -0.16, 0.18), 0.15, metal_green));
    */

    //LET'S GET CREATIVE - MICKEY MOUSE
    
    //Ground
    world.add(make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
    
    //Head
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.4, light_moon));

    //Right Ear
    world.add(make_shared<sphere>(point3(0.5, 0.4, -1.0), 0.2, metal_gold));

    //Left Ear
    world.add(make_shared<sphere>(point3(-0.5, 0.4, -1.0), 0.2, metal_gold));

    //Nose
    world.add(make_shared<sphere>(point3(0.02, -0.125, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.01, -0.125, -0.6), 0.0425, metal_gold));
    world.add(make_shared<sphere>(point3(0.00, -0.125, -0.6), 0.045, metal_gold));
    world.add(make_shared<sphere>(point3(-0.01, -0.125, -0.6), 0.0425, metal_gold));
    world.add(make_shared<sphere>(point3(-0.02, -0.125, -0.6), 0.04, metal_gold));

    //Left Eye
    world.add(make_shared<sphere>(point3(-0.125, 0.1, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.09, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.08, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.07, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.06, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.05, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.04, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.03, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.02, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.01, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, 0.00, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, -0.01, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, -0.02, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, -0.03, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(-0.125, -0.04, -0.6), 0.04, metal_gold));

    //Right Eye
    world.add(make_shared<sphere>(point3(0.125, 0.1, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.09, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.08, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.07, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.06, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.05, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.04, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.03, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.02, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.01, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, 0.00, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, -0.01, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, -0.02, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, -0.03, -0.6), 0.04, metal_gold));
    world.add(make_shared<sphere>(point3(0.125, -0.04, -0.6), 0.04, metal_gold));

    //Front Camera
    //camera cam(point3(0, 0, 2), point3(0, 0.0, -1), vec3(0, 1, 0), 45, aspect_ratio);
    
    //Angle #1
    //camera cam(point3(-1, 0, 2), point3(0, 0.5, -1), vec3(0, 1, 0), 40, aspect_ratio);

    //Angle #2
    camera cam(point3(-4.85, -0.45, 1.5), point3(1.25, 1.25, -1.5), vec3(0, 1, 0), 35, aspect_ratio);

    // Camera
    //Task1 Angle1
    //camera cam(point3(-1, 0, 2), point3(0, 0.5, -1), vec3(0, 1, 0), 60, aspect_ratio);
    
    //Task1 Angle2
    //camera cam(point3(-4.85, -0.45, 1.5), point3(1.25, 1.25, -1.5), vec3(0, 1, 0), 30, aspect_ratio);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    //NO ANTI-ALIASING
    /*
    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical);
            color pixel_color = ray_color(r, world);
            write_color(std::cout, pixel_color);
        }
    }
    */

    //RANDOM SUPERSAMPLING ANTI-ALIASING
    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                //pixel_color += ray_color(r, world, max_depth);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    //GRID SUPERSAMPLING ANTI-ALIASING
    /*
    int grid = 3;

    for (int j = image_height - 1; j >= 0; --j) { //Y-Coordinate
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;

        for (int i = 0; i < image_width; ++i) { //X-Coordinate
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                color color_temp(0, 0, 0);
                for (int p = 0; p < grid; p++) {
                    for (int q = 0; q < grid; q++) {
                        auto u = ((double(i) + ((p * 1.0) / grid)) / (image_width - 1));
                        auto v = ((double(j) + ((q * 1.0) / grid)) / (image_height - 1));

                        ray r = cam.get_ray(u, v);
                        color_temp += ray_color(r, world);
                    }
                }
                pixel_color += color_temp / (grid * grid);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }
    */

    std::cerr << "\nDone.\n";
}