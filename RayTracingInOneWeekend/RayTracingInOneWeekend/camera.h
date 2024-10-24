#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"

class camera 
{
public:
    double aspect_ratio = 1.0;
    int image_width = 100;
    int samples_per_pixel = 10;   
    int max_depth = 10;   

    void render(const hittable& world) 
    {
        initialize();

        std::clog << "\rStart rendering.           \n";

        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int h = 0; h < image_height; h++)
        {
            std::clog << "\rScanlines done: " << (float)h / (float)image_height << ' ' << std::flush;
            for (int w = 0; w < image_width; w++)
            {
                colour pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++) 
                {
                    ray r = get_ray(w, h);
                    pixel_color += ray_colour(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }

        std::clog << "\rFinish rendering.           \n";
    }

private:
    int    image_height;
    point3 camera_center;
    point3 pixel00_loc;
    vec3   pixel_delta_u;
    vec3   pixel_delta_v;
    double pixel_samples_scale;  

    void initialize() 
    {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        camera_center = point3(0, 0, 0);

        auto focal_length = 1.0;
        auto viewport_height = 2.0;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        auto viewport_u = vec3(viewport_width, 0, 0);
        auto viewport_v = vec3(0, -viewport_height, 0);
        
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        pixel_samples_scale = 1.0 / samples_per_pixel;
    }

    ray get_ray(int i, int j) const 
    {
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
            + ((i + offset.x()) * pixel_delta_u)
            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const 
    {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    colour ray_colour(const ray& r, int depth, const hittable& world) const 
    {
        hit_record rec;

        if (depth <= 0)
        {
            return colour(0, 0, 0);
        }

        if (world.hit(r, interval(0.001, infinity), rec)) 
        {
            vec3 direction = random_on_hemisphere(rec.normal);
            return 0.5 * ray_colour(ray(rec.p, direction), depth - 1, world);
        }

        vec3 unit_direction = unit_vector(r.direction());
        auto a = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - a) * colour(1.0, 1.0, 1.0) + a * colour(0.5, 0.7, 1.0);
    }
};

#endif
