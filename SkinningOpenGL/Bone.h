#pragma once
#include"utils.h"

using namespace std;

struct AnimPos {
	glm::vec3 pos;
	float timestamp;
};

struct AnimRot {
	glm::quat rot;
	float timestamp;
};

struct AnimScale {
	glm::vec3 scale;
	float timestamp;
};

// 存储Assimp::NodeAnim中的信息，即骨骼随时间的变换
class Bone
{
public:
	Bone(const string& name, int ID, const aiNodeAnim* channel);
	void update(float time);
	// 根据时间插值变换矩阵
	glm::mat4 interpolatePos(float time);
	glm::mat4 interpolateRot(float time);
	glm::mat4 interpolateScale(float time);
	float getInterpolateFactor(float timestamp1, float timestamp2, float curTime);

	int id;
	string name;
	vector<AnimPos> posSet;
	vector<AnimRot> rotSet;
	vector<AnimScale> scaleSet;
	utils mUtils;
	glm::mat4 transfrom;
};

