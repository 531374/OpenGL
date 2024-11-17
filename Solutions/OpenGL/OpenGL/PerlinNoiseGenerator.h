#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>

#include "Shader.h"

#include <ctime>

struct Quad
{
	glm::vec3 topLeft;
	glm::vec3 topRight;
	glm::vec3 bottomLeft;
	glm::vec3 bottomRight;
};

struct Vert
{
	glm::vec3 position;
	glm::vec3 normal;
};

class PerlinNoiseGenerator
{
public:
	std::vector<std::vector<glm::vec2>> grid;
	PerlinNoiseGenerator(int x, int y);
	void Draw(Shader& shader);
	void CreateMesh(float stepSize);

private:
	void createGrid(int x, int y);
	void setupMesh();
	void generateVertices(std::vector<Quad> quads);
	float getValue(float x, float y);

	std::vector<Vert> vertices;

	unsigned int VAO, VBO;



};

