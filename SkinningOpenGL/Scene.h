#pragma once
 
#include"stb_image.h"
#include<vector>
#include"Mesh.h"  
#include "BoneInfo.h"
#include"Bone.h"


using namespace std;

struct Assimp2GLMNode {
	string name;
	glm::mat4 transfromation;
	std::vector<Assimp2GLMNode> childrens;
};
// 实现骨骼动画部分参考了learnOpenGL的Skeletal-Animation模块:https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
// 加载模型部分参考了learnOpenGL的Model模块：https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/
// read .dae file, extract meshes,bone,Animation simliar to Assimp::aiScene
// 
class Scene
{
public:
	Scene(string& filePath);
	void loadModel();
	void loadAnimation();
	void processNode(aiNode* node,const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadTextures(aiMaterial* mat, aiTextureType type, string typeName);
	int importTextureFromFile(const char* path, const string& dir);
	void Draw(Shader shader);
	void computeBoneWeight(aiMesh* mesh,vector<Vertex>& vertices);
	void bindBone(Vertex& v, int* boneId, float* weight); 
	void readNodesData(Assimp2GLMNode& out, const aiNode* in);
	int useNameSearchIndex(string name);
	void update(float interval);
	void computeBoneTransform(Assimp2GLMNode* node,glm::mat4 parentTransform);

	double duration;
	double ticksPerSecond;
	vector<Bone> bones;
	// mesh include vertices & boneInfo, simliar to Assimp::aiMesh
	vector<Mesh> meshes;
	vector<Texture> textureLoaded;
	string dir;
	const aiScene* ascene;
	vector<BoneInfo> boneInfos;
	utils mUtils;
	Assimp2GLMNode rootNode;
	float currTime;
	vector<glm::mat4> finalBoneTransformations;
};

