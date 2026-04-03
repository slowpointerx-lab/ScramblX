#pragma once

#include <vector>
#include <string>

#include <glm/glm/glm.hpp>

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct TextureStruct {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureStruct> textures;
	glm::vec3 color;
	glm::mat4 localTransform;

	unsigned int VAO;

	Mesh(
		std::vector<Vertex> vertices,
		std::vector<unsigned int> indices,
		std::vector<TextureStruct> textures,
		glm::vec3 meshColor,
		glm::mat4 transform
	);

	void Draw(unsigned int shader, glm::mat4 worldMatrix);

private:
	unsigned int VBO, EBO;

	void setupMesh();
};