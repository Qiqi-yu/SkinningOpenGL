#version 430 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;
layout(location = 3) in ivec2 boneIds;
layout(location = 4) in vec2 weights;

const int MAX_BONES_EFFECT = 2;
const int MAX_BONES = 100;

uniform mat4 mvpMat;
uniform mat4 boneTrans[MAX_BONES];

out vec2 TexCoords;

void main()
{
	vec4 totalBonePos = vec4(0.0f);
	for(int i = 0 ;i < MAX_BONES_EFFECT; i++)
	{
		vec4 localPos = boneTrans[boneIds[i]] * vec4(pos,1.0f);
		totalBonePos += localPos * weights[i]; 
	}

	gl_Position = mvpMat * totalBonePos;
	TexCoords = tex;
}