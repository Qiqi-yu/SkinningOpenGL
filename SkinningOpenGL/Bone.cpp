#include "Bone.h"

Bone::Bone(const string& name, int ID, const aiNodeAnim* channel)
{
	this->name = name;
	this->id = ID;
	this->transfrom = glm::mat4(1.0f);
	int numPos = channel->mNumPositionKeys;
	int numRot = channel->mNumRotationKeys;
	int numScale = channel->mNumScalingKeys;
	for (int idx = 0; idx < numPos; idx++) {
		aiVector3D aiPos = channel->mPositionKeys[idx].mValue;
		auto aiTimestamp = channel->mPositionKeys[idx].mTime;
		AnimPos pos;
		pos.pos = glm::vec3(aiPos.x, aiPos.y, aiPos.z);
		pos.timestamp = aiTimestamp;
		posSet.push_back(pos);
	}
	
	for (int idx = 0; idx < numRot; idx++) {
		aiQuaternion aiRot =channel->mRotationKeys[idx].mValue;
		auto aiTimestamp = channel->mRotationKeys[idx].mTime;
		AnimRot rot;
		rot.rot = glm::quat(aiRot.w,aiRot.x,aiRot.y,aiRot.z);
		rot.timestamp = aiTimestamp;
		rotSet.push_back(rot);
	}
	
	for (int idx = 0; idx < numScale; idx++) {
		aiVector3D aiScale = channel->mScalingKeys[idx].mValue;
		auto aiTimestamp = channel->mScalingKeys[idx].mTime;
		AnimScale scale;
		scale.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
		scale.timestamp = aiTimestamp;
		scaleSet.push_back(scale);
	}
}

void Bone::update(float time)
{
	glm::mat4 trans = interpolatePos(time);
	glm::mat4 rot = interpolateRot(time);
	glm::mat4 scale = interpolateScale(time);
	this->transfrom = trans * rot * scale;
}

glm::mat4 Bone::interpolatePos(float time)
{
	auto size = posSet.size();
	if (size == 1) {
		return glm::translate(glm::mat4(1.0f), posSet[0].pos);
	}

	int idxT0 = 0; int idxT1 = 0;
	for (int i = 0; i < size - 1 ; i++) {
		if (time < posSet[i + 1].timestamp) {
			idxT0 = i;
			break;
		}
	}
	idxT1 = idxT0 + 1;
	float interpolateF = getInterpolateFactor(posSet[idxT0].timestamp, posSet[idxT1].timestamp, time);
	glm::vec3 interpolatePos = glm::mix(posSet[idxT0].pos, posSet[idxT1].pos, interpolateF);
	
	return glm::translate(glm::mat4(1.0f), interpolatePos);
}

glm::mat4 Bone::interpolateRot(float time)
{
	auto size = rotSet.size();
	if (size == 1) {
		return glm::toMat4(glm::normalize(rotSet[0].rot));
	}

	int idxT0 = 0; int idxT1 = 0;
	for (int i = 0; i < size - 1; i++) {
		if (time < rotSet[i + 1].timestamp) {
			idxT0 = i;
			break;
		}
	}
	idxT1 = idxT0 + 1;
	float interpolateF = getInterpolateFactor(rotSet[idxT0].timestamp, rotSet[idxT1].timestamp, time);
	glm::quat interpolateRot = glm::slerp(rotSet[idxT0].rot, rotSet[idxT1].rot, interpolateF);
	interpolateRot = glm::normalize(interpolateRot);

	return glm::toMat4(interpolateRot);
}

glm::mat4 Bone::interpolateScale(float time)
{
	auto size = scaleSet.size();
	if (size == 1) {
		return glm::scale(glm::mat4(1.0f), scaleSet[0].scale);
	}

	int idxT0 = 0; int idxT1 = 0;
	for (int i = 0; i < size - 1; i++) {
		if (time < scaleSet[i + 1].timestamp) {
			idxT0 = i;
			break;
		}
	}
	idxT1 = idxT0 + 1;
	float interpolateF = getInterpolateFactor(scaleSet[idxT0].timestamp, scaleSet[idxT1].timestamp, time);
	glm::vec3 interpolateScale = glm::mix(scaleSet[idxT0].scale, scaleSet[idxT1].scale, interpolateF);

	return glm::scale(glm::mat4(1.0f), interpolateScale);
}

float Bone::getInterpolateFactor(float timestamp1, float timestamp2, float curTime)
{
	return (curTime - timestamp1) / (timestamp2 - timestamp1);
}
