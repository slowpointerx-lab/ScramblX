#include "Mesh.h"

#include <glad/glad.h>
#include <glm/glm/gtc/type_ptr.hpp>

Mesh::Mesh(
	std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::vector<TextureStruct> textures,
	glm::vec3 meshColor,
	glm::mat4 transform
)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->color = meshColor;
	this->localTransform = transform;

	setupMesh();
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(Vertex),
		&vertices[0],
		GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indices.size() * sizeof(unsigned int),
		&indices[0],
		GL_STATIC_DRAW
	);

	//Pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)0
	);

	//Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, Normal)
	);

	//Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex),
		(void*)offsetof(Vertex, TexCoords)
	);

	glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shader, glm::mat4 worldMatrix) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;

	// Use the stored localTransform for this specific mesh part
	glm::mat4 finalModelMatrix = worldMatrix * this->localTransform;
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(finalModelMatrix));
	glUniform3f(glGetUniformLocation(shader, "baseColor"), color.r, color.g, color.b);
	glUniform1i(glGetUniformLocation(shader, "hasTexture"), textures.size() > 0 ? 1 : 0);

	for (unsigned int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse") number = std::to_string(diffuseNr++);
		else if (name == "texture_specular") number = std::to_string(specularNr++);

		// FIXED: name + number
		glUniform1i(glGetUniformLocation(shader, (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}