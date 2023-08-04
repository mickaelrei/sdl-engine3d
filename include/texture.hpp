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
        printf("%d, %d\n", surface->w, surface->h);
        width = surface->w; height = surface->h;

        // Load color data
        // printf("Tex size: %ld\n", colors.size());
        // int i = 0;
        // colors = std::vector<SDL_Color>(width * height);
        // for (int y = 0; y < height; y++)
        // {
        //     for (int x = 0; x < width; x++)
        //     {
        //         Uint8 r, g, b, a;
        //         SDL_Color color = GetColorAt(x, y);
        //         colors[y * width + x] = color;
        //     }
        // }

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
            // printf("[GetPixelColor]: Coordinate (%d, %d) out of texture bounds (%d, %d)\n", sx, sy, width, height);
            return {0};
        }

        // Return color
        // return colors[sy * width + sx];
    }

    // Copy-and-Swap idiom by GManNickG
    // https://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
    friend void swap(Texture& first, Texture& second)
    {
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.loaded, second.loaded);
        swap(first.width, second.width);
        swap(first.height, second.height);
        swap(first.surface, second.surface);
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
    //     // std::copy(other.surface, other.surface, surface);
    // }

    // If texture has been loaded
    bool loaded;

    // Surface size
    int width = 0, height = 0;

    // Get color at given coordinate
    // Made by Daniel1985
    // https://discourse.libsdl.org/t/how-do-i-get-the-rgb-values-of-a-pixel-from-a-given-surface-and-x-and-y-coordinates-in-sdl2/26915
    SDL_Color ColorAt(int x, int y)
    {
        // Check if not loaded
        if (!loaded || !surface)
        {
            printf("Not loaded\n");
            return {0};
        }

        // Check if out of bounds
        if (x < 0 || x >= width || y < 0 || y >= height)
        {
            printf("[ColorAt]: Coordinate (%d, %d) out of texture bounds (%d, %d)\n", x, y, width, height);
            return {0};
        }

        // Bytes per pixel
        Uint8 Bpp = surface->format->BytesPerPixel;

        /*
        Retrieve the address to a specific pixel
        surface->pixels	= an array containing the SDL_Surface' pixels
        surface->pitch		= the length of a row of pixels (in bytes)
        X and Y				= the offset on where on the image to retrieve the pixel, (0, 0) is in the upper left corner of the image
        */
        Uint8* pPixel = (Uint8*)surface->pixels + y * surface->pitch + x * Bpp;

        Uint32 PixelData = *(Uint32*)pPixel;

        SDL_Color Color = {0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE};

        // Retrieve the RGB values of the specific pixel
        SDL_GetRGB(PixelData, surface->format, &Color.r, &Color.g, &Color.b);

        return Color;
    }

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