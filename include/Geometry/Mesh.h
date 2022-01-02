#pragma once

#include "Geometry/MeshVertex.h"
#include <glm/vec3.hpp>
#include <filesystem>
#include <vector>

struct Mesh
{
    /// Init a cube.
    /// @return Mesh.
    static Mesh InitCube();
    /// Init a triangle that cover the screen.
    /// @return Mesh.
    static Mesh InitQuad();
    /// Init a triangle.
    /// @return Mesh.
    static Mesh InitTriangle();

    /// Translate the mesh.
    /// @param iVector Translation vector.
    void Translate(glm::vec3 iVector);
    /// Compute the centroid of the mesh.
    /// @return A point.
    [[nodiscard]] glm::vec3 ComputeCentroid() const;

    /// Vertices
    std::vector<MeshVertex> Vertices;

    /// Indices to define mesh's triangles.
    std::vector<uint32_t> Indices;
};