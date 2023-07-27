#include "engine.hpp"
#include "stdio.h"

#include <random>

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