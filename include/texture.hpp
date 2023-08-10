#pragma once

#include <SDL2/SDL.h>
#include <bits/stdc++.h>

class Texture
{
public:
    // Constructors and destructor
    Texture()
    {
        loaded = false;
        surface = NULL;
    }
    ~Texture()
    {
        // printf("Freeing texture\n");
        // if (surface)
        // {
        //     SDL_FreeSurface(surface);
        //     surface = NULL;
        // }
    }

    // Initialize texture
    bool init(std::string filePath)
    {
        // Try to load image
        surface = SDL_LoadBMP(filePath.c_str());
        if (surface == NULL) {
            printf("Unable to load BMP at path %s! SDL Error: %s\n", filePath.c_str(), SDL_GetError());
            loaded = false;
            return loaded;
        }

        // Set size
        loaded = true;
        width = surface->w; height = surface->h;

        return loaded;
    }

    // Get color at coordinate
    SDL_Color GetPixelColor(float x, float y)
    {
        // Loaded check
        if (!loaded || !surface)
        {
            printf("Texture not loaded\n");
            return {0};
        }

        // Bounds check
        int sx = (int)(x * (float)width);
        int sy = (int)(y * (float)height);
        if (sx < 0 || sx >= width || sy < 0 || sy >= height)
        {
            printf("[GetPixelColor]: Coordinate (%d, %d) out of texture bounds (%d, %d)\n", sx, sy, width, height);
            return {0};
        }

        // Return color
        // return colors[sy * width + sx];
    }

    // Copy-and-Swap idiom by GManNickG
    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    friend void swap(Texture& first, Texture& second)
    {
        // by swapping the members of two objects,
        // the two objects are effectively swapped
        std::swap(first.loaded, second.loaded);
        std::swap(first.width, second.width);
        std::swap(first.height, second.height);
        std::swap(first.surface, second.surface);
    }

    Texture& operator=(Texture other)
    {
        printf("copy assignment\n");
        swap(*this, other);

        return *this;
    }

    // copy-constructor
    // Texture(const Texture& other)
    //     : loaded(other.loaded),
    //       width(other.width),
    //       height(other.height)
    //     //   mArray(mSize ? new int[mSize] : nullptr)
    // {
    //     printf("Copy ctor\n");
    //     // note that this is non-throwing, because of the data
    //     // types being used; more attention to detail with regards
    //     // to exceptions must be given in a more general case, however
    //     std::copy(other.surface, other.surface, surface);
    // }

    // If texture has been loaded
    bool loaded;

    // Surface size
    int width = 0, height = 0;

    // Get color at given coordinate
    SDL_Color GetColorAt(int x, int y)
    {
        // Check if not loaded
        if (!loaded || !surface)
        {
            printf("Not loaded\n");
            return {0, 0, 0, 0};
        }

        // Check if out of bounds
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            // printf("[ColorAt]: Coordinate (%d, %d) out of texture bounds (%d, %d)\n", x, y, width, height);
            return {0, 0, 0, 0};
        }

        // Declare color
        SDL_Color rgb;

        // Get pixel data
        Uint32 data = GetPixel(x, y);

        // Get RGB
        SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);

        // Set alpha to max
        rgb.a = SDL_ALPHA_OPAQUE;
        return rgb;
    }
private:
    // Get pixel data
    Uint32 GetPixel(int x, int y)
    {
        int bpp = surface->format->BytesPerPixel;
        /* Here p is the address to the pixel we want to retrieve */
        Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

        switch (bpp)
        {
            case 1:
                return *p;
                break;

            case 2:
                return *(Uint16 *)p;
                break;

            case 3:
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                    return p[0] << 16 | p[1] << 8 | p[2];
                else
                    return p[0] | p[1] << 8 | p[2] << 16;
                break;

            case 4:
                return *(Uint32 *)p;
                break;

            default:
                return 0;       /* shouldn't happen, but avoids warnings */
        }
    }

    // Surface loaded from file
    SDL_Surface *surface;

    // Array containing color value for each pixel
    // std::vector<SDL_Color> colors;
};