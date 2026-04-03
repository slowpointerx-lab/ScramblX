#pragma once

#include <vector>
#include <string>

#include <glm/glm/glm.hpp>

#include "Mesh.h"

#include <assimp/scene.h>

class Model {
public:
	Model(const std::string& path);

	void Draw(unsigned int shader, glm::mat4 worldMatrix = glm::mat4(1.0f));

private:
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<TextureStruct> textures_loaded;

	void loadModel(std::string path);

	void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformation);

	std::vector<TextureStruct> loadMaterialTextures(
		aiMaterial* mat,
		aiTextureType type,
		std::string typeName
	);
};