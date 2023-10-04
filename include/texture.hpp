#pragma once

#include <SDL2/SDL.h>
#include <bits/stdc++.h>

class Texture
{
public:
    // Constructors and destructor
    Texture();
    ~Texture();

    // Initialize texture from image
    bool init(std::string filePath);

    // Initialize texture with base color
    bool init(SDL_Color color);

    // Copy-and-Swap idiom by GManNickG
    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    void swap(Texture& first, Texture& second);

    Texture& operator=(Texture other);

    // copy-constructor
    // Texture(const Texture& other);

    // If texture has been loaded
    bool loaded;

    // Whether this texture is only a base color (not loaded from image file)
    bool isBaseColor;

    // Surface size
    int width = 0, height = 0;

    // Base color, in case texture was not loaded from image file
    SDL_Color baseColor = {0, 0, 0, SDL_ALPHA_OPAQUE};

    // Get color at given coordinate
    SDL_Color GetColorAt(int x, int y);
private:
    // Get pixel data
    Uint32 GetPixel(int x, int y);

    // Surface loaded from file
    SDL_Surface *surface;
};