#include "engine.hpp"
#include "stdio.h"

float angle(Vec3D v0, Vec3D v1)
{
    return (float)SDL_acos((double)v0.dot(v1));
}

int main(int argc, char **argv)
{
    Engine3D engine;
    if (!engine.init())
    {
        printf("Error initializing engine");
        exit(-1);
    }

    engine.run();


    return 0;
}