#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "vec3d.hpp"
#include "texture.hpp"
#include "triangle.hpp"

inline float map_num(float n, float start1, float stop1, float start2, float stop2)
{
    return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

class Mesh
{
public:
    Mesh() {}
    ~Mesh() {}

    std::vector<Triangle> tris;
    Vec3D position = {0.0f, 0.0f, 0.0f};
    Vec3D rotation = {0.0f, 0.0f, 0.0f};
    Vec3D size = {1.0f, 1.0f, 1.0f};
    Texture texture;

    void SetColor(SDL_Color color)
    {
        for (auto& tri : tris)
        {
            tri.color = color;
        }
    }

    bool LoadFromOBJFile(std::string fileName)
    {
        std::ifstream f(fileName);
		if (!f.is_open())
			return false;

        // Check if file has UVs and normals
        bool hasTexture = false;
        bool hasNormals = false;

		// Local cache of verts
		std::vector<Vec3D> verts;
		std::vector<TexUV> texs;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::stringstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				if (line[1] == 't')
				{
                    // File has texture
                    hasTexture = true;

					TexUV tex;
					s >> junk >> junk >> tex.u >> tex.v;

                    // Y axis starts at the bottom for UVs, so invert V
                    tex.v = 1.0f - tex.v;
                    tex.w = 1.0f;
					texs.push_back(tex);
				}
                else if (line[1] == 'n')
                {
                    // File has normals
                    hasNormals = true;

                    // Store in vector
                }
				else
				{
					Vec3D vert;
					s >> junk >> vert.x >> vert.y >> vert.z;
					verts.push_back(vert);
				}
			}

            if (line[0] == 'f')
            {
                if (!hasTexture)
                {
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                }
                else
                {
                    s >> junk;

                    std::string tokens[6];
                    int nTokenCount = -1;

                    while (!s.eof())
                    {
                        char c = s.get();
                        if (c == ' ' || c == '/')
                            nTokenCount++;
                        else
                            tokens[nTokenCount].append(1, c);
                    }
                    tokens[nTokenCount].pop_back();

                    tris.push_back({
                        verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],     // Verts
                        texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1]         // UVs
                    });
                }
            }
		}
		return true;
    }

    void ToCube(Vec3D size = {1.0f, 1.0f, 1.0f})
    {
        tris.clear();

        float halfX = size.x * .5f;
        float halfY = size.y * .5f;
        float halfZ = size.z * .5f;
        tris = {
            // FRONT
            { Vec3D(-halfX, -halfY, -halfZ),    Vec3D(-halfX,  halfY, -halfZ),   Vec3D( halfX,  halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D(-halfX, -halfY, -halfZ),    Vec3D( halfX,  halfY, -halfZ),   Vec3D( halfX, -halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },

            // RIGHT
            { Vec3D( halfX, -halfY, -halfZ),    Vec3D( halfX,  halfY, -halfZ),   Vec3D( halfX,  halfY,  halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D( halfX, -halfY, -halfZ),    Vec3D( halfX,  halfY,  halfZ),   Vec3D( halfX, -halfY,  halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },

            // BACK
            { Vec3D( halfX, -halfY,  halfZ),    Vec3D( halfX,  halfY,  halfZ),   Vec3D(-halfX,  halfY,  halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D( halfX, -halfY,  halfZ),    Vec3D(-halfX,  halfY,  halfZ),   Vec3D(-halfX, -halfY,  halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },

            // LEFT
            { Vec3D(-halfX, -halfY,  halfZ),    Vec3D(-halfX,  halfY,  halfZ),   Vec3D(-halfX,  halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D(-halfX, -halfY,  halfZ),    Vec3D(-halfX,  halfY, -halfZ),   Vec3D(-halfX, -halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },

            // TOP
            { Vec3D(-halfX,  halfY, -halfZ),    Vec3D(-halfX,  halfY,  halfZ),   Vec3D( halfX,  halfY,  halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D(-halfX,  halfY, -halfZ),    Vec3D( halfX,  halfY,  halfZ),   Vec3D( halfX,  halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },

            // BOTTOM
            { Vec3D( halfX, -halfY,  halfZ),    Vec3D(-halfX, -halfY,  halfZ),   Vec3D(-halfX, -halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(1.0f, 0.0f) },
            { Vec3D( halfX, -halfY,  halfZ),    Vec3D(-halfX, -halfY, -halfZ),   Vec3D( halfX, -halfY, -halfZ),    TexUV(0.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(1.0f, 1.0f) },
		};
    }

    void ToSphere(float radius, int resolution = 10)
    {
        // Clear tris
        tris.clear();

        // Cache of vertices
        std::vector<Vec3D> vertices;

        // Create vertices
        for (int i = 0; i < resolution; i++)
        {
            float lat = map_num(i, 0, resolution-1, 0, 2 * M_PI);
            for (int j = 0; j < resolution; j++)
            {
                float lon = map_num(j, 0, resolution-1, 0, M_PI);

                Vec3D v;
                v.x = SDL_sinf(lon) * SDL_cosf(lat) * radius * 0.5f;
                v.y = SDL_sinf(lon) * SDL_sinf(lat) * radius * 0.5f;
                v.z = SDL_cosf(lon) * radius * 0.5f;

                vertices.push_back(v);
            }
        }

        // Create faces
        for (int i = 0; i < resolution - 1; i++)
        {
            for (int j = 0; j < resolution - 1; j++)
            {
                // Get indices
                int i0 = i + j * resolution;
                int i1 = (i+1) + j * resolution;
                int i2 = i + (j+1) * resolution;
                int i3 = (i+1) + (j+1) * resolution;

                // Create two triangles
                tris.push_back({
                    vertices[i0],
                    vertices[i1],
                    vertices[i3],
                    TexUV{0.0f, 1.0f, 1.0f},
                    TexUV{0.0f, 0.0f, 1.0f},
                    TexUV{1.0f, 0.0f, 1.0f}
                });
                tris.push_back({
                    vertices[i0],
                    vertices[i3],
                    vertices[i2],
                    TexUV{0.0f, 1.0f, 1.0f},
                    TexUV{1.0f, 0.0f, 1.0f},
                    TexUV{1.0f, 1.0f, 1.0f}
                });
            }
        }
    }
};
