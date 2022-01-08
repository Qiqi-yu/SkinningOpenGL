#define STB_IMAGE_IMPLEMENTATION   
#include "Scene.h"

Scene::Scene(string& filePath)
{
	dir = filePath.substr(0, filePath.find_last_of('/'));
	Assimp::Importer importer;
	ascene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!ascene || ascene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ascene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	loadModel();
	loadAnimation();
	readNodesData(rootNode, ascene->mRootNode);
	finalBoneTransformations.resize(boneInfos.size());
}

void Scene::loadModel()
{
	processNode(ascene->mRootNode, ascene);
}

void Scene::loadAnimation()
{
	auto animation = ascene->mAnimations[0];
	duration = animation->mDuration;
	ticksPerSecond = animation->mTicksPerSecond;
	auto size = animation->mNumChannels;

	for (int i = 0; i < size; i++) {
		auto channel = animation->mChannels[i];
		string name = channel->mNodeName.data;
		int idx = useNameSearchIndex(name);
		if (idx != -1) {
			Bone bone(channel->mNodeName.data, idx, channel);
			bones.push_back(bone);
		}
	}
}

void Scene::processNode(aiNode* node,const aiScene* scene)
{
	// 处理节点所有的网格（如果有的话）
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// 接下来对它的子节点重复这一过程
	for (int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Scene::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0]) // 网格是否有纹理坐标？
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		// 初始化boneId & weight
		for (unsigned int i = 0; i < MAX_BONE; i++) {
			vertex.boneIds[i] = -1;
			vertex.weights[i] = 0;
		}
		vertices.push_back(vertex);
	}

	// 处理索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// 处理材质
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = loadTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		vector<Texture> specularMaps = loadTextures(material,aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	computeBoneWeight(mesh, vertices);
	Mesh currMesh = Mesh(vertices, indices, textures);  
	return currMesh;
}

vector<Texture> Scene::loadTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textureLoaded.size(); j++)
		{
			if (std::strcmp(textureLoaded[j].path.data(), str.C_Str()) == 0)
			{
				textureLoaded.push_back(textureLoaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // 如果纹理还没有被加载，则加载它
			Texture texture;
			texture.id = importTextureFromFile(str.C_Str(), dir);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textureLoaded.push_back(texture); // 添加到已加载的纹理中
		}
	}
	return textures;
}

int Scene::importTextureFromFile(const char* path, const string& dir)
{
	string filename = string(path);
	filename = dir + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Scene::Draw(Shader shader)
{
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Scene::computeBoneWeight(aiMesh* mesh, vector<Vertex>& vertices)
{
	for (unsigned int idx = 0; idx < mesh->mNumBones; idx++) {
		string name = mesh->mBones[idx]->mName.C_Str();
		if (useNameSearchIndex(name) == -1) {
			BoneInfo info;
			info.name = name;
			info.offset = mUtils.ConvertMatrixTOGLM(mesh->mBones[idx]->mOffsetMatrix);
			boneInfos.push_back(info);
		}
	}
	float dis[MAX_BONE]; int boneId[MAX_BONE]; float weight[MAX_BONE];
	auto size = boneInfos.size();
	for (int i = 0; i < vertices.size(); i++) {
		for (int i = 0; i < MAX_BONE; i++) {
			dis[i] = 1000000;
		}
		for (unsigned int i = 0; i < size; i++) {
			glm::vec4 pos = boneInfos[i].offset * glm::vec4(vertices[i].Position, 1.0f);
			glm::vec4 truePos = glm::inverse(boneInfos[i].offset) * glm::vec4(0.0f);
			pos = pos / pos.w;
			for (int k = 0; k < MAX_BONE; k++) {
				if (glm::dot(pos, pos) < dis[k]) {
					for (int j = MAX_BONE - 1; j > k; j--) {
						dis[j] = dis[j - 1];
						boneId[j] = boneId[j - 1];
					}
					dis[k] = glm::dot(pos, pos);
					boneId[k] = i;
					break;
				}
			}

		}

		for (int k = 0; k < MAX_BONE; k++) {
			if (k == 0) {
				weight[k] = 1;
			}
			else {
				weight[k] = 0;
			}
		}
		for (int k = 0; k < MAX_BONE; k++) {
			bindBone(vertices[i], boneId, weight);
		}
	}

}

void Scene::bindBone(Vertex& v, int* boneId, float* weight)
{
	for (unsigned int i = 0; i < MAX_BONE; i++) {
		v.weights[i] = weight[i];
		v.boneIds[i] = boneId[i];
	}
}

void Scene::readNodesData(Assimp2GLMNode& out,const aiNode* in)
{
	out.name = in->mName.data;
	out.transfromation = mUtils.ConvertMatrixTOGLM(in->mTransformation);
	
	int num = in->mNumChildren;
	for (int i = 0; i < num; i++) {
		Assimp2GLMNode childNode;
		readNodesData(childNode, in->mChildren[i]);
		out.childrens.push_back(childNode);
	}
}

int Scene::useNameSearchIndex(string name)
{
	auto size = boneInfos.size();
	for (int idx = 0; idx < size; idx++) {
		if (boneInfos[idx].name == name) {
			return idx;
		}
	}
	return -1;
}

void Scene::update(float interval)
{
	currTime += ticksPerSecond * interval;
	currTime = fmod(currTime, duration);
	computeBoneTransform(&rootNode, glm::mat4(1.0f));
}

void Scene::computeBoneTransform(Assimp2GLMNode* node, glm::mat4 parentTransform)
{
	string name = node->name;
	glm::mat4 trans = node->transfromation;

	Bone* curBone;
	auto size = bones.size();
	for (int i = 0; i < size; i++) {
		if (bones[i].name == name) 
		{
			curBone = &bones[i];
			curBone->update(currTime);
			trans = curBone->transfrom;
			break;
		}
	}
	glm::mat4 globalTrans = parentTransform * trans;

	int idx = useNameSearchIndex(name);
	if (idx != -1) {
		glm::mat4 offset = boneInfos[idx].offset;
		finalBoneTransformations[idx] = globalTrans * offset;
	}

	for (int i = 0; i < node->childrens.size(); i++) {
		computeBoneTransform(&node->childrens[i], globalTrans);
	}
}

