#include <array>
#include <algorithm>

#include "engine.hpp"

// Drawing
void Engine3D::Fill(SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}

void Engine3D::RenderPoint(Vec2D p, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPointF(renderer, p.x, p.y);
}

void Engine3D::RenderTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderDrawLine(renderer, (int)v0.x, (int)v0.y, (int)v1.x, (int)v1.y);
    SDL_RenderDrawLine(renderer, (int)v1.x, (int)v1.y, (int)v2.x, (int)v2.y);
    SDL_RenderDrawLine(renderer, (int)v2.x, (int)v2.y, (int)v0.x, (int)v0.y);
}

/* My attempt (not perfect), reading from:
http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
*/
void Engine3D::FillTriangleOld(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Create array
    std::array<Vec2D, 3> vecList = {v0, v1, v2};

    // Sort by Y
    std::sort(vecList.begin(), vecList.end(), [](const Vec2D& first, const Vec2D& second) -> bool {
        return first.y < second.y;
    });

    // TOP PART
    int diffY = vecList[1].y - vecList[0].y;

    // Increment on X towards x1
    float incX0 = (vecList[1].x - vecList[0].x) / (float)diffY;

    // Get X coordinate on Y-level of v1 on line from v0 to v2
    float x3 = vecList[0].x + (vecList[1].y - vecList[0].y) / (vecList[2].y - vecList[0].y) * (vecList[2].x - vecList[0].x);

    // Increment on X towards x3
    float incX1 = (x3 - vecList[0].x) / (float)diffY;

    // Scan lines for top part
    for (int i = 0; i <= diffY; i++)
    {
        int y = vecList[0].y + i;
        float x0 = vecList[0].x + incX0 * i;
        float x1 = vecList[0].x + incX1 * i;

        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }

    // BOTTOM PART
    diffY = vecList[2].y - vecList[1].y;

    // Increment on X from v1 towards x2
    incX0 = (vecList[2].x - vecList[1].x) / (float)diffY;

    // Increment on X towards x2
    incX1 = (vecList[2].x - x3) / (float)diffY;

    // Scan lines for bottom part
    for (int i = 0; i <= diffY; i++)
    {
        int y = vecList[1].y + i;
        float x0 = vecList[1].x + incX0 * i;
        float x1 = x3 + incX1 * i;

        SDL_RenderDrawLine(renderer, x0, y, x1, y);
    }
}

/* Taken from:
https://github.com/OneLoneCoder/Javidx9/blob/master/ConsoleGameEngine/olcConsoleGameEngine.h
on line 537
*/
void Engine3D::FillTriangle(Vec2D v0, Vec2D v1, Vec2D v2, SDL_Color color)
{
    int x1 = (int)v0.x;
    int x2 = (int)v1.x;
    int x3 = (int)v2.x;
    int y1 = (int)v0.y;
    int y2 = (int)v1.y;
    int y3 = (int)v2.y;
    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
    auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) RenderPoint({(float)i, (float)ny}, color); };

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                        // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        SWAP(dy1, dx1);
        changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i<dx1) i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next4:

        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        drawline(minx, maxx, y);   										
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y>y3) return;
    }
}

void Engine3D::RenderRect(Vec2D pos, Vec2D size, int thickness, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect rect{(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
    if (thickness < 0)
        return;
    else if (thickness == 0)
        SDL_RenderFillRect(renderer, &rect);
    else if (thickness == 1)
        SDL_RenderDrawRect(renderer, &rect);
    else
    {
        // Thickness can't be over half the size of the bigger axis
        thickness = std::min({thickness, (int)std::min({size.x, size.y})});

        // Left
        int i;
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x + i, pos.y, pos.x + i, pos.y + size.y);
        }

        // Right
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x + size.x - i, pos.y, pos.x + size.x - i, pos.y + size.y);
        }

        // Top
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x, pos.y + i, pos.x + size.x, pos.y + i);
        }

        // Bottom
        for (i = 0; i <= thickness; i++)
        {
            SDL_RenderDrawLine(renderer, pos.x, pos.y + size.y - i, pos.x + size.x, pos.y + size.y - i);
        }
    }
}