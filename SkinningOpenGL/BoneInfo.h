#pragma once
#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

using namespace std;

// ��ȡAssimp::Bone����¼�����Ļ�����Ϣ
class BoneInfo
{
public:
	string name;
	glm::mat4 offset;
};

