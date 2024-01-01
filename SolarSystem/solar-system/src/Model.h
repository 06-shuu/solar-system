#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image/stb_image.h>



class Model
{
private:
    Assimp::Importer importer;
    const aiScene* scene = nullptr;
    aiNode* root_node = nullptr;

    struct Mesh
    {
        unsigned int VAO, VBO1, VBO2, VBO3, EBO;
        std::vector<glm::vec3> vert_positions;
        std::vector<glm::vec3> vert_normals;
        std::vector<glm::vec2> tex_coords;
        std::vector<unsigned int> vert_indices;
        unsigned int tex_handle;
    };

    struct Texture
    {
        unsigned int textureID;
        std::string image_name;
    };

public:
    unsigned int num_meshes;
    std::vector<Mesh> mesh_list;
    std::vector<Texture> texture_list;

    Model(const char* model_path);

private:
    void load_model();
    void load_model_cout_console();
    void set_buffer_data(unsigned int index);
    int is_image_loaded(std::string file_name);
    unsigned int load_texture_image(std::string file_name, bool& load_complete);
};

#endif // MODEL_H
