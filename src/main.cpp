#include "engine.hpp"

#include <random>

int main(int argc, char **argv)
{
    Engine3D engine;
    if (!engine.init())
    {
        exit(-1);
    }

    engine.start();


    return 0;
}