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

// �洢Assimp::NodeAnim�е���Ϣ����������ʱ��ı任
class Bone
{
public:
	Bone(const string& name, int ID, const aiNodeAnim* channel);
	void update(float time);
	// ����ʱ���ֵ�任����
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

