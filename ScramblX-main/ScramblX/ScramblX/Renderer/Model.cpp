#include "Model.h"
#include "TextureClass.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <iostream>

glm::mat4 convertMatrix(aiMatrix4x4 from) {
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	
	return to;
}

Model::Model(const std::string& path) {
	loadModel(path);
}

void Model::Draw(unsigned int shader, glm::mat4 worldMatrix) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader, worldMatrix);
	}
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace	|
		aiProcess_JoinIdenticalVertices	|
		aiProcess_OptimizeMeshes	|
		aiProcess_GenUVCoords	|
		aiProcess_GenNormals	|
		aiProcess_FindInvalidData
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene, glm::mat4(1.0f));
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform) {
	glm::mat4 nodeTransform = convertMatrix(node->mTransformation);
	glm::mat4 globalTransform = parentTransform * nodeTransform;

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, globalTransform));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, globalTransform);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transformation) {
	std::cout << "Loading Mesh: " << mesh->mName.C_Str() << "| Vertices: " << mesh->mNumVertices << std::endl;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TextureStruct> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		vertex.Normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	aiColor4D diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);
	glm::vec3 meshColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);

	std::vector<TextureStruct> diffuseMaps = loadMaterialTextures(
		material,
		aiTextureType_DIFFUSE,
		"texture_diffuse"
	);

	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	std::vector<TextureStruct> specularMaps = loadMaterialTextures(
		material,
		aiTextureType_SPECULAR,
		"texture_specular"
	);

	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return Mesh(vertices, indices, textures, meshColor, transformation);
}

std::vector<TextureStruct> Model::loadMaterialTextures(
	aiMaterial* mat,
	aiTextureType type,
	std::string typeName
)
{
	std::vector<TextureStruct> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;

		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (textures_loaded[j].path == str.C_Str()) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			std::string filename = std::string(str.C_Str());

			if (filename == "*0") filename = "textures/gltf_embedded_0.png";
			else if (filename == "*1") filename = "textures/gltf_embedded_1.png";
			else if (filename == "*2") filename = "textures/gltf_embedded_2.png";
			else if (filename == "*3") filename = "textures/gltf_embedded_3.jpeg";

			else {
				filename = filename.substr(filename.find_last_of("\\/") + 1);
			}
			std::string fullPath = this->directory + "/" + filename;

			Texture texture(fullPath.c_str());

			TextureStruct tex;
			tex.id = texture.ID;
			tex.type = typeName;
			tex.path = str.C_Str();

			textures.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}
	return textures;
}