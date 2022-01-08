#include "utils.h"

glm::mat4 utils::ConvertMatrixTOGLM(const aiMatrix4x4& in)
{
	glm::mat4 out;
	//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
	out[0][0] = in.a1; out[1][0] = in.a2; out[2][0] = in.a3; out[3][0] = in.a4;
	out[0][1] = in.b1; out[1][1] = in.b2; out[2][1] = in.b3; out[3][1] = in.b4;
	out[0][2] = in.c1; out[1][2] = in.c2; out[2][2] = in.c3; out[3][2] = in.c4;
	out[0][3] = in.d1; out[1][3] = in.d2; out[2][3] = in.d3; out[3][3] = in.d4;
	return out;
}
