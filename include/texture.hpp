#include <SDL2/SDL.h>

class Texture
{
public:
    // Constructors and destructor
    Texture()
    {
        loaded = false;
        surface = NULL;
        colors = NULL;
    }
    ~Texture()
    {
        if (surface)
        {
            SDL_FreeSurface(surface);
            surface = NULL;
        }
        if (colors)
        {
            free(colors);
            colors = NULL;
        }
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
        printf("Loaded! Size: %d, %d\n", surface->w, surface->h);
        loaded = true;
        width = surface->w; height = surface->h;
        
        // Load color data
        int i = 0;
        colors = new SDL_Color[width * height];
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
                colors[i++] = ColorAt(x, y);
        }
        printf("Set colors\n");

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
        return colors[sy * width + sx];
    }

    // If texture has been loaded
    bool loaded;

private:
    // Get color at given coordinate
    // Made by Daniel1985
    // https://discourse.libsdl.org/t/how-do-i-get-the-rgb-values-of-a-pixel-from-a-given-surface-and-x-and-y-coordinates-in-sdl2/26915
    SDL_Color ColorAt(const int x, const int y)
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
        const Uint8 Bpp = surface->format->BytesPerPixel;

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

    // Surface loaded from file
    SDL_Surface *surface;

    // Surface size
    int width = 0, height = 0;

    // Array containing color value for each pixel
    SDL_Color *colors;
};