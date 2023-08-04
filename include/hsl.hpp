#pragma once

#include <SDL2/SDL.h>

struct HSL
{
	int H;
	float S;
	float L;

    void FromRGB(SDL_Color color)
    {
        HSL hsl;

        float r = (color.r / 255.0f);
        float g = (color.g / 255.0f);
        float b = (color.b / 255.0f);

        float min = std::min(std::min(r, g), b);
        float max = std::max(std::max(r, g), b);
        float delta = max - min;

        L = (max + min) / 2;

        if (delta == 0)
        {
            H = 0;
            S = 0.0f;
        }
        else
        {
            S = (hsl.L <= 0.5) ? (delta / (max + min)) : (delta / (2 - max - min));

            float hue;

            if (r == max)
            {
                hue = ((g - b) / 6) / delta;
            }
            else if (g == max)
            {
                hue = (1.0f / 3) + ((b - r) / 6) / delta;
            }
            else
            {
                hue = (2.0f / 3) + ((r - g) / 6) / delta;
            }

            if (hue < 0)
                hue += 1;
            if (hue > 1)
                hue -= 1;

            H = (int)(hue * 360);
        }
    }

    float HueToRGB(float v1, float v2, float vH) {
        if (vH < 0)
            vH += 1;

        if (vH > 1)
            vH -= 1;

        if ((6 * vH) < 1)
            return (v1 + (v2 - v1) * 6 * vH);

        if ((2 * vH) < 1)
            return v2;

        if ((3 * vH) < 2)
            return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

        return v1;
    }

    SDL_Color ToRGB()
    {
        unsigned char r = 0;
        unsigned char g = 0;
        unsigned char b = 0;

        if (S == 0)
        {
            r = g = b = (unsigned char)(L * 255);
        }
        else
        {
            float v1, v2;
            float hue = (float)H / 360;

            v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
            v1 = 2 * L - v2;

            r = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
            g = (unsigned char)(255 * HueToRGB(v1, v2, hue));
            b = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
        }

        return {r, g, b, SDL_ALPHA_OPAQUE};
    }

    // Printing
    friend std::ostream& operator << (std::ostream &os, const HSL &c) {
        return (os << "(" << c.H << ", " << c.S << ", " << c.L << ")");
    }
};
