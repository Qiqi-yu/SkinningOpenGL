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
// ʵ�ֹ����������ֲο���learnOpenGL��Skeletal-Animationģ��:https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
// ����ģ�Ͳ��ֲο���learnOpenGL��Modelģ�飺https://learnopengl-cn.github.io/03%20Model%20Loading/03%20Model/
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
	// ����vertices��ÿ��������صĹؽ���Ȩ��
	void computeBoneWeight(aiMesh* mesh,vector<Vertex>& vertices);
	void bindBone(Vertex& v, int* boneId, float* weight); 
	// ��ȡNodeAnim֮��ĸ��ӽڵ��ϵ�����ڼ���ؽ�ȫ�ֵı任
	void readNodesData(Assimp2GLMNode& out, const aiNode* in);
	int useNameSearchIndex(string name);
	// ����update��ͨ�������任�Ĳ�ֵʵ�ֶ���
	void update(float interval);
	// update�е��ã�����Bone�ı任����
	void computeBoneTransform(Assimp2GLMNode* node,glm::mat4 parentTransform);

	// ����ʱ��
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

