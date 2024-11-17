#include "PerlinNoiseGenerator.h"

PerlinNoiseGenerator::PerlinNoiseGenerator(int x, int y)
{
	srand((int)time(0));
	createGrid(x, y);
}

void PerlinNoiseGenerator::createGrid(int x, int y)
{
	for (unsigned int i = 0; i < x + 1; i++)
	{
		std::vector<glm::vec2> row;

		for (unsigned int j = 0; j < y + 1; j++)
		{
			float randomDeg = rand() % 360;
			float randomRad = randomDeg / 180.0f * M_PI;

			glm::vec2 vec = glm::vec2(cos(randomRad), sin(randomRad));


			row.push_back(vec);
		}
		grid.push_back(row);
	}
}

float smoothstep(float a, float b, float t)
{
	return (b - a) * (3.0 - t * 2.0) * t * t + a	;
}

float PerlinNoiseGenerator::getValue(float x, float y)
{
	if (x >= grid.size() - 1 || y >= grid[0].size() - 1 || x < 0 || y < 0)
	{
		std::cout << "Position out of range!" << std::endl;
		return 0;
	}
	else
	{
		int lowerX = static_cast <int> (std::floor(x));
		int upperX = lowerX + 1;

		int lowerY = static_cast <int> (std::floor(y));
		int upperY = lowerY + 1;

		glm::vec2 dst = glm::vec2(x, y) - glm::vec2(lowerX, lowerY);
		float dotLL = glm::dot(grid[lowerX][lowerY], dst);

		dst = glm::vec2(x, y) - glm::vec2(upperX, lowerY);
		float dotUL = glm::dot(grid[upperX][lowerY], dst);

		dst = glm::vec2(x, y) - glm::vec2(lowerX, upperY);
		float dotLR = glm::dot(grid[lowerX][upperY], dst);

		dst = glm::vec2(x, y) - glm::vec2(upperX, upperY);
		float dotUR = glm::dot(grid[upperX][upperY], dst);

		float dx = x - lowerX;
		float dy = y - lowerY;

		float lerp1 = smoothstep(dotLL, dotLR, dx);
		float lerp2 = smoothstep(dotUL, dotUR, dx);

		return smoothstep(lerp1, lerp2, dy);
	}
}

void PerlinNoiseGenerator::CreateMesh(float stepSize)
{

	std::vector<Quad> quads;

	for (float x = 0; x < grid.size() - 1 - stepSize; x += stepSize)
	{
		for (float z = 0; z < grid[0].size() - 1 - stepSize; z += stepSize)
		{
			Quad quad;
			float y = getValue(x, z);
			quad.topLeft = glm::vec3(x, y, z);

			y = getValue(x + stepSize, z);
			quad.topRight = glm::vec3(x + stepSize, y, z);

			y = getValue(x, z + stepSize);
			quad.bottomLeft = glm::vec3(x, y, z + stepSize);

			y = getValue(x + stepSize, z + stepSize);
			quad.bottomRight = glm::vec3(x + stepSize, y, z + stepSize);

			quads.push_back(quad);
		}
	}

	generateVertices(quads);
	setupMesh();
	
	std::cout << "Succesfully created mesh!" << std::endl;
}

glm::vec3 calculateNormal(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
{
	glm::vec3 edge1 = v2 - v1;
	glm::vec3 edge2 = v3 - v1;
	glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));
	return normal;
}

void PerlinNoiseGenerator::generateVertices(std::vector<Quad> quads)
{
	vertices.clear();

	for (unsigned int i = 0; i < quads.size(); i++)
	{
		Quad quad = quads[i];
		
		Vert BL;
		BL.position = quad.bottomLeft;

		Vert BR;
		BR.position = quad.bottomRight;

		Vert TL;
		TL.position = quad.topLeft;

		Vert TR;
		TR.position = quad.topRight;

		glm::vec3 normal1 = calculateNormal(BL.position, BR.position, TR.position);
		glm::vec3 normal2 = calculateNormal(BL.position, TL.position, TR.position);

		BL.normal = normal1;
		BR.normal = normal1;
		TR.normal = normal1;

		BL.normal += normal2;
		TL.normal = normal2;
		TR.normal += normal2;

		BL.normal = glm::normalize(BL.normal);
		BR.normal = glm::normalize(BR.normal);
		TL.normal = glm::normalize(TL.normal);
		TR.normal = glm::normalize(TR.normal);

		vertices.push_back(BL);
		vertices.push_back(BR);
		vertices.push_back(TR);

		vertices.push_back(BL);
		vertices.push_back(TL);
		vertices.push_back(TR);

	}
}

void PerlinNoiseGenerator::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vert), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*)offsetof(Vert, normal));


	glBindVertexArray(0);
}

void PerlinNoiseGenerator::Draw(Shader& shader)
{
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
}