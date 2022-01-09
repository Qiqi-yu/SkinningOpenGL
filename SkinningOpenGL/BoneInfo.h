#pragma once
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

// 读取Assimp::Bone，记录骨骼的基本信息
class BoneInfo
{
public:
	string name;
	glm::mat4 offset;
};

