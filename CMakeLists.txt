cmake_minimum_required(VERSION 3.21)
project(RayTracer)

set(CMAKE_CXX_STANDARD 14 -pthread)

add_executable(RayTracer main.cpp vec3.h color.h ray.h hittable.h sphere.h hittable_list.h ray_tracer.h camera.h material.h triangle.h setupScene.h aabb.h bvh_node.h texture.h ray_tracer_stb_image.h external/stb_image.h rectangle.h background.h Animator/frameMaker.h Animator/movingCamera.h constant_medium.h box.h SuperSpectrum.h RGBSpectrum.h SampledSpectrum.h CIE_Values.h)
