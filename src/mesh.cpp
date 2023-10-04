#include <mesh.hpp>

float map(float n, float start1, float stop1, float start2, float stop2)
{
    return (n - start1) / (stop1 - start1) * (stop2 - start2) + start2;
}

Mesh::Mesh()
{
    
}

Mesh::~Mesh()
{

}

// Set color of all triangles
void Mesh::SetColor(SDL_Color color)
{
    for (auto& tri : tris)
    {
        tri.color = color;
    }
}

// Load from .obj file
Mesh Mesh::FromOBJFile(std::string fileName)
{
    std::ifstream f(fileName);
    if (!f.is_open())
    {
        std::cout << "Error loading OBJ model from path " << fileName << "\n";
        exit(-1);
    }

    Mesh mesh;

    // Check if file has UVs and normals
    bool hasTexture = false;
    bool hasNormals = false;

    // Local cache of verts
    std::vector<Vec3> verts;
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
                Vec3 vert;
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
                mesh.tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
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

                mesh.tris.push_back({
                    verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],     // Verts
                    texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1]         // UVs
                });
            }
        }
    }
    return mesh;
}

// Common shapes
Mesh Mesh::Cube()
{
    Mesh mesh;
    mesh.tris = {
        // FRONT
        { Vec3(-0.5f, -0.5f, -0.5f),    Vec3(-0.5f,  0.5f, -0.5f),   Vec3( 0.5f,  0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3(-0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f, -0.5f),   Vec3( 0.5f, -0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },

        // RIGHT
        { Vec3( 0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f, -0.5f),   Vec3( 0.5f,  0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3( 0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3( 0.5f, -0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },

        // BACK
        { Vec3( 0.5f, -0.5f,  0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3(-0.5f,  0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3( 0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3(-0.5f, -0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },

        // LEFT
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3(-0.5f,  0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f, -0.5f),   Vec3(-0.5f, -0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },

        // TOP
        { Vec3(-0.5f,  0.5f, -0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3( 0.5f,  0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3(-0.5f,  0.5f, -0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3( 0.5f,  0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },

        // BOTTOM
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f, -0.5f, -0.5f),   Vec3( 0.5f, -0.5f, -0.5f),    TexUV(1.0f, 1.0f),     TexUV(1.0f, 0.0f),     TexUV(0.0f, 0.0f) },
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3( 0.5f, -0.5f, -0.5f),   Vec3( 0.5f, -0.5f,  0.5f),    TexUV(1.0f, 1.0f),     TexUV(0.0f, 0.0f),     TexUV(0.0f, 1.0f) },
    };

    return mesh;
}

Mesh Mesh::MinecraftCube()
{
    Mesh mesh;

    mesh.tris = {
        // FRONT
        { Vec3(-0.5f, -0.5f, -0.5f),    Vec3(-0.5f,  0.5f, -0.5f),   Vec3( 0.5f,  0.5f, -0.5f),    TexUV(0.5f,  2.0f / 3.0f),     TexUV(0.5f,  1.0f / 3.0f),     TexUV(0.25f, 1.0f / 3.0f) },
        { Vec3(-0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f, -0.5f),   Vec3( 0.5f, -0.5f, -0.5f),    TexUV(0.5f,  2.0f / 3.0f),     TexUV(0.25f, 1.0f / 3.0f),     TexUV(0.25f, 2.0f / 3.0f) },

        // RIGHT
        { Vec3( 0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f, -0.5f),   Vec3( 0.5f,  0.5f,  0.5f),    TexUV(0.75f, 2.0f / 3.0f),     TexUV(0.75f, 1.0f / 3.0f),     TexUV(0.5f,  1.0f / 3.0f) },
        { Vec3( 0.5f, -0.5f, -0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3( 0.5f, -0.5f,  0.5f),    TexUV(0.75f, 2.0f / 3.0f),     TexUV(0.5f,  1.0f / 3.0f),     TexUV(0.5f,  2.0f / 3.0f) },

        // BACK
        { Vec3( 0.5f, -0.5f,  0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3(-0.5f,  0.5f,  0.5f),    TexUV(1.0f,  2.0f / 3.0f),     TexUV(1.0f,  1.0f / 3.0f),     TexUV(0.75f, 1.0f / 3.0f) },
        { Vec3( 0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3(-0.5f, -0.5f,  0.5f),    TexUV(1.0f,  2.0f / 3.0f),     TexUV(0.75f, 1.0f / 3.0f),     TexUV(0.75f, 2.0f / 3.0f) },

        // LEFT
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3(-0.5f,  0.5f, -0.5f),    TexUV(0.25f, 2.0f / 3.0f),     TexUV(0.25f, 1.0f / 3.0f),     TexUV(0.0f,  1.0f / 3.0f) },
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f,  0.5f, -0.5f),   Vec3(-0.5f, -0.5f, -0.5f),    TexUV(0.25f, 2.0f / 3.0f),     TexUV(0.0f,  1.0f / 3.0f),     TexUV(0.0f,  2.0f / 3.0f) },

        // TOP
        { Vec3(-0.5f,  0.5f, -0.5f),    Vec3(-0.5f,  0.5f,  0.5f),   Vec3( 0.5f,  0.5f,  0.5f),    TexUV(0.5f,  1.0f / 3.0f),     TexUV(0.5f,         0.0f),     TexUV(0.25f,        0.0f) },
        { Vec3(-0.5f,  0.5f, -0.5f),    Vec3( 0.5f,  0.5f,  0.5f),   Vec3( 0.5f,  0.5f, -0.5f),    TexUV(0.5f,  1.0f / 3.0f),     TexUV(0.25f,        0.0f),     TexUV(0.25f, 1.0f / 3.0f) },

        // BOTTOM
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3(-0.5f, -0.5f, -0.5f),   Vec3( 0.5f, -0.5f, -0.5f),    TexUV(0.5f,        1.0f),      TexUV(0.5f,  2.0f / 3.0f),     TexUV(0.25f, 2.0f / 3.0f) },
        { Vec3(-0.5f, -0.5f,  0.5f),    Vec3( 0.5f, -0.5f, -0.5f),   Vec3( 0.5f, -0.5f,  0.5f),    TexUV(0.5f,        1.0f),      TexUV(0.25f, 2.0f / 3.0f),     TexUV(0.25f,        1.0f) },
    };

    return mesh;
}

Mesh Mesh::Sphere(float radius, int resolution)
{
    Mesh mesh;

    // Cache of vertices
    std::vector<Vec3> vertices;

    // Create vertices
    for (int i = 0; i < resolution; i++)
    {
        float lat = map(i, 0, resolution-1, 0, 2 * M_PI);
        for (int j = 0; j < resolution; j++)
        {
            float lon = map(j, 0, resolution-1, 0, M_PI);

            Vec3 v;
            v.x = std::sin(lon) * std::cos(lat) * radius;
            v.y = std::sin(lon) * std::sin(lat) * radius;
            v.z = std::cos(lon) * radius;

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
            mesh.tris.push_back({
                vertices[i0],
                vertices[i1],
                vertices[i3],
                TexUV{0.0f, 1.0f, 1.0f},
                TexUV{0.0f, 0.0f, 1.0f},
                TexUV{1.0f, 0.0f, 1.0f}
            });
            mesh.tris.push_back({
                vertices[i0],
                vertices[i3],
                vertices[i2],
                TexUV{0.0f, 1.0f, 1.0f},
                TexUV{1.0f, 0.0f, 1.0f},
                TexUV{1.0f, 1.0f, 1.0f}
            });
        }
    }

    return mesh;
}

Mesh Mesh::Cone(float baseRadius, float height, int resolution)
{
    Mesh mesh;
    
    // Cache of vertices
    std::vector<Vec3> vertices;

    // Base vertices
    for (float i = 0; i < resolution; i++)
    {
        float x = std::cos(i / (float)resolution * M_PI * 2) * baseRadius;
        float z = std::sin(i / (float)resolution * M_PI * 2) * baseRadius;

        vertices.push_back({x, -height * .5f, z});
    }

    // Create top faces
    Vec3 topVertex = {0.0f, height * .5f, 0.0f};
    for (int i = 0; i < resolution; i++)
    {
        mesh.tris.push_back({
            vertices[i], topVertex, vertices[(i+1) % resolution],
            TexUV(0.0f, 0.0f), TexUV(0.0f, 1.0f), TexUV(1.0f, 1.0f)
        });
    }

    // Create bottom faces
    Vec3 bottomVertex = {0.0f, -height * .5f, 0.0f};
    for (int i = 0; i < resolution; i++)
    {
        mesh.tris.push_back({
            vertices[i], vertices[(i+1) % resolution], bottomVertex,
            TexUV(1.0f, 0.0f), TexUV(0.0f, 0.0f), TexUV(1.0f, 1.0f)
        });
    }

    return mesh;
}

Mesh Mesh::Cylinder(float baseRadius, float height, int resolution)
{
    Mesh mesh;
    
    // Cache of vertices
    std::vector<Vec3> topVertices;
    std::vector<Vec3> bottomVertices;

    // Base vertices
    for (float i = 0; i < resolution; i++)
    {
        float x = std::cos(i / (float)resolution * M_PI * 2) * baseRadius;
        float z = std::sin(i / (float)resolution * M_PI * 2) * baseRadius;

        bottomVertices.push_back({x, -height * .5f, z});
    }

    // Top vertices
    for (float i = 0; i < resolution; i++)
    {
        float x = std::cos(i / (float)resolution * M_PI * 2) * baseRadius;
        float z = std::sin(i / (float)resolution * M_PI * 2) * baseRadius;

        topVertices.push_back({x, height * .5f, z});
    }

    // Create side faces
    for (int i = 0; i < resolution; i++)
    {
        // First
        mesh.tris.push_back({
            topVertices[(i+1) % resolution], bottomVertices[(i+1) % resolution], bottomVertices[i],
            TexUV(1.0f, 0.0f), TexUV(0.0f, 0.0f), TexUV(1.0f, 1.0f)
        });

        // Second
        mesh.tris.push_back({
            topVertices[i], topVertices[(i+1) % resolution], bottomVertices[i],
            TexUV(1.0f, 0.0f), TexUV(0.0f, 0.0f), TexUV(1.0f, 1.0f)
        });
    }

    // Create top and bottom faces
    Vec3 bottomVertex = {0.0f, -height * .5f, 0.0f};
    Vec3 topVertex = {0.0f, height * .5f, 0.0f};
    for (int i = 0; i < resolution; i++)
    {
        // Top
        mesh.tris.push_back({
            topVertices[(i+1) % resolution], topVertices[i], topVertex,
            TexUV(1.0f, 0.0f), TexUV(0.0f, 0.0f), TexUV(1.0f, 1.0f)
        });

        // Second
        mesh.tris.push_back({
            bottomVertices[i], bottomVertices[(i+1) % resolution], bottomVertex,
            TexUV(1.0f, 0.0f), TexUV(0.0f, 0.0f), TexUV(1.0f, 1.0f)
        });
    }

    return mesh;
}