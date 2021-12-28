#include "Geometry/Mesh.h"
#include <glm/geometric.hpp>

//----------------------------------------------------------------------------------------------------------------------
Mesh Mesh::InitCube()
{
    Mesh m;

    m.Vertices.emplace_back(MeshVertex{{-0.5, -0.5, -0.5}, {-0.5, -0.5, -0.5}});
    m.Vertices.emplace_back(MeshVertex{{0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}});
    m.Vertices.emplace_back(MeshVertex{{0.5, 0.5, -0.5}, {0.5, 0.5, -0.5}});
    m.Vertices.emplace_back(MeshVertex{{-0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}});
    m.Vertices.emplace_back(MeshVertex{{-0.5, -0.5, 0.5}, {-0.5, -0.5, 0.5}});
    m.Vertices.emplace_back(MeshVertex{{0.5, -0.5, 0.5}, {0.5, -0.5, 0.5}});
    m.Vertices.emplace_back(MeshVertex{{0.5, 0.5, 0.5}, {0.5, 0.5, 0.5}});
    m.Vertices.emplace_back(MeshVertex{{-0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}});

    for (MeshVertex &vertex : m.Vertices)
    {
        vertex.Normal = glm::normalize(vertex.Normal);
    }

    m.Indices = {0, 1, 2, 2, 3, 0, 1, 2, 6, 6, 5, 1, 0, 1, 5, 5, 4, 0,
                 3, 0, 4, 4, 7, 3, 6, 2, 3, 3, 7, 6, 4, 5, 6, 6, 7, 4};
    return m;
}

//----------------------------------------------------------------------------------------------------------------------
Mesh Mesh::InitQuad()
{
    Mesh m;
    m.Indices = {0, 1, 2};
    m.Vertices.emplace_back(MeshVertex{{-3, 1.0, 0}, {0.0, 0.0, 1.0}});
    m.Vertices.emplace_back(MeshVertex{{3, 1, 0}, {0.0, 0.0, 1.0}});
    m.Vertices.emplace_back(MeshVertex{{0, -2, 0}, {0.0, 0.0, 1.0}});
    return m;
}

//----------------------------------------------------------------------------------------------------------------------
void Mesh::Translate(glm::vec3 iVector)
{
    for (MeshVertex v : Vertices)
    {
        v.Pos += iVector;
    }
}

//----------------------------------------------------------------------------------------------------------------------
glm::vec3 Mesh::ComputeCentroid() const
{
    glm::vec3 centroid;
    for (MeshVertex v : Vertices)
    {
        centroid += v.Pos;
    }
    centroid /= Vertices.size();
    return centroid;
}