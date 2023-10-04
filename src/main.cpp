#include <engine.hpp>
#include <stdio.h>


int main(int argc, char **argv)
{
    Engine3D engine;
    if (!engine.init())
    {
        printf("Error initializing engine");
        exit(-1);
    }

    engine.run();

    // HSL hsl;
    // hsl.FromRGB(SDL_Color{255, 0, 0, 255});
    // std::cout << "HSL{" << hsl.H << ", " << hsl.S << ", " << hsl.L << "}\n";

    // Multiply by light brightness
    // hsl.L *= d * lights[0].brightness;
    // triProjected.color = hsl.ToRGB();


    return 0;
}