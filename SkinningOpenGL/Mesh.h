#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"


// reference to learnopengl Mesh Content: https://learnopengl-cn.github.io/03%20Model%20Loading/02%20Mesh/

using namespace std;
#define MAX_BONE 2
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // save the effect bone id & weight
    int boneIds[MAX_BONE];
    float weights[MAX_BONE];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};


class Mesh
{
public:
    /*  ��������  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    
    /*  ����  */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader shader);

    
private:
    /*  ��Ⱦ����  */
    unsigned int VAO, VBO, EBO;
    
    /*  ����  */
    void setupMesh();
   
};

