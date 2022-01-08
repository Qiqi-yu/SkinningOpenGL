#pragma once

#include <glm/glm.hpp>  
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<string>
#include<vector>

class utils
{
public:
	glm::mat4 ConvertMatrixTOGLM(const aiMatrix4x4& in);
};

