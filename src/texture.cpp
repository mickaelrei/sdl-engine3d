#include <texture.hpp>

Texture::Texture()
{
    loaded = false;
    surface = NULL;
}

Texture::~Texture()
{
    // printf("Freeing texture\n");
    // if (surface)
    // {
    //     SDL_FreeSurface(surface);
    //     surface = NULL;
    // }
}

bool Texture::init(std::string filePath)
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

SDL_Color Texture::GetColorAt(int x, int y)
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

// Get pixel data
Uint32 Texture::GetPixel(int x, int y)
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

void Texture::swap(Texture& first, Texture& second)
{
    // by swapping the members of two objects,
    // the two objects are effectively swapped
    std::swap(first.loaded, second.loaded);
    std::swap(first.width, second.width);
    std::swap(first.height, second.height);
    std::swap(first.surface, second.surface);
}

Texture& Texture::operator=(Texture other)
{
    swap(*this, other);

    return *this;
}

// Texture::Texture(const Texture& other)
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