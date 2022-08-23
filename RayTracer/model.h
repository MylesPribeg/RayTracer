#pragma once
#include <vector>
#include "hittable.h"
#include "hittable_list.h"
#include "raytracer.h"
#include "triangle.h"
#include "material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const char* path);
	hittable_list getHitList();

private:
	hittable_list triangles;
	//std::vector<Mesh> meshes;
	std::string directory;

	//temp
	shared_ptr<material> mat_ptr;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	void processMesh(aiMesh* mesh, const aiScene* scene);

};


Model::Model(const char* path)
{
	mat_ptr = make_shared<lambertian>(make_shared<image_texture>("models/testcube/test.png"));
	//mat_ptr = make_shared<dielectic>(1.5, 0.5);
	loadModel(path);

}

hittable_list Model::getHitList()
{
	return triangles;
}

void Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp model loading error: " << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));

	//processNode(scene->mRootNode, scene);
	

	//Just going through all meshes in scene

	for (int i = 0; i < scene->mNumMeshes; i++) {
		processMesh(scene->mMeshes[i], scene);
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all of nodes meshes
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	// continue with nodes children
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	hittable_list tris;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		
		Vertex vertex;
		// processing vertex information
		vec3 vec;
		vec[0] = mesh->mVertices[i].x;
		vec[1] = mesh->mVertices[i].y;
		vec[2] = mesh->mVertices[i].z;
		vertex.Position = vec;

		vec[0] = mesh->mNormals[i].x;
		vec[1] = mesh->mNormals[i].y;
		vec[2] = mesh->mNormals[i].z;
		vertex.Normal = vec;

		if (mesh->mTextureCoords[0]) // does it have tex coords?
		{
			vec2 vec;
			vec[0] = mesh->mTextureCoords[0][i].x;
			vec[1] = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	// processing indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int x = 0; x < face.mNumIndices; x++)
		{
			indices.push_back(face.mIndices[x]);
		}
	}

	for (int i = 0; i < indices.size(); i += 3)
	{
		tris.add(make_shared<triangle>(
			vertices[indices[i]],
			vertices[indices[i + 1]],
			vertices[indices[i + 2]],
			mat_ptr
		));
	}

	std::cout << "Building model BVH";

	triangles.add(make_shared<bvh_node>(tris, 0, 1));
	//triangles = tris;
	//// processing materials
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); //for inserting multiple items

	//	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	//}

	//return Mesh(vertices, indices, textures);
}


