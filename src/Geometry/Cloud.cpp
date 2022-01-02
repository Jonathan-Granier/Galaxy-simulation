#include "Geometry/Cloud.h"

#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
Cloud Cloud::InitGalaxy()
{
	Cloud cloud;
	cloud.Points.emplace_back(glm::vec3(-0.5, -0.5, -0.5));
    cloud.Points.emplace_back(glm::vec3(0.5, -0.5, -0.5));
    cloud.Points.emplace_back(glm::vec3(0.5, 0.5, -0.5));
    cloud.Points.emplace_back(glm::vec3(-0.5, 0.5, -0.5));
    cloud.Points.emplace_back(glm::vec3(-0.5, -0.5, 0.5));
    cloud.Points.emplace_back(glm::vec3(0.5, -0.5, 0.5));
    cloud.Points.emplace_back(glm::vec3(0.5, 0.5, 0.5));
    cloud.Points.emplace_back(glm::vec3(-0.5, 0.5, 0.5));
	return cloud;
}

//----------------------------------------------------------------------------------------------------------------------
Cloud Cloud::InitAxisX()
{
	Cloud cloud;
	for(int i = 0; i < 10; ++i)
	{
		cloud.Points.emplace_back(glm::vec3{ static_cast<double>(i) / 10.0, 0, 0 });
	}
	return cloud;
}

//----------------------------------------------------------------------------------------------------------------------
Cloud Cloud::InitAxisY()
{
	Cloud cloud;
	for(int i = 0; i < 10; ++i)
	{
		cloud.Points.emplace_back(glm::vec3{0, static_cast<double>(i) / 10.0, 0 });
	}
	return cloud;
}

//----------------------------------------------------------------------------------------------------------------------
Cloud Cloud::InitAxisZ()
{
	Cloud cloud;
	for(int i = 0; i < 10; ++i)
	{
		cloud.Points.emplace_back(glm::vec3{ 0, 0, static_cast<double>(i) / 10.0 });
	}
	return cloud;
}

//----------------------------------------------------------------------------------------------------------------------
void Cloud::Translate(glm::vec3& iVector)
{
	for(glm::vec3& point : Points)
	{
		point += iVector;
	}
}

//----------------------------------------------------------------------------------------------------------------------
glm::vec3 Cloud::ComputeCentroid() const
{
	glm::vec3 centroid;
	for(glm::vec3 point : Points)
	{
		centroid += point;
	}
	centroid /= Points.size();
	return centroid;
}