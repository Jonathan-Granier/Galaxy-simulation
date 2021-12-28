#pragma once

#include "Geometry/MeshVertex.h"
#include <glm/vec3.hpp>
#include <filesystem>
#include <vector>

struct Mesh
{

    static Mesh InitCube();
    static Mesh InitQuad();

    void Translate(glm::vec3 iVector);
    [[nodiscard]] glm::vec3 ComputeCentroid() const;

    /// Vertices
    std::vector<MeshVertex> Vertices;

    /// Indices to define mesh's triangles.
    std::vector<uint32_t> Indices;
};