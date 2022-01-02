#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include <filesystem>

struct Cloud{

	static Cloud InitGalaxy();
	static Cloud InitAxisX();
	static Cloud InitAxisY();
	static Cloud InitAxisZ();

	void Translate(glm::vec3& iVector);
	[[nodiscard]] glm::vec3 ComputeCentroid() const;

	std::vector<glm::vec3> Points;
};
