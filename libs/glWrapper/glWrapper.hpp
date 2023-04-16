#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <array>
#include <vector>

#include "../gl/glad.h"
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"
#include "../libs/tinygltf/tinygltf.hpp"

namespace glWrap
{
    class Shader
    {
        public:
        unsigned int m_ID;
        
        Shader(std::string vertexPath, std::string fragmentPath);
        void Use();

        void SetBool(const std::string &name, bool value) const;
        void SetInt(const std::string &name, int value) const;
        void SetFloat(const std::string &name, float value) const;
        void SetMatrix4(const std::string &name, glm::mat4 mat) const;
    };

    class Texture2D
    {
        public:
        unsigned int m_ID;

        /** @brief Texture2D Constructor 
         *@param[in] image String path to image location on disk
         *@param[in] flip If image should be vertically flipped
         *@param[in] filter Select pixel interpolation: GL_LINEAR or GL_NEAREST
         *@param[in] desiredChannels Select texture channels: GL_RED, GL_RG, GL_RGB or GL_RGBA
         */
        Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels);

        /** @brief Description
         *@param[in] unit GL Texture Unit
         */
        void SetActive(unsigned int unit);
    };

    class Camera{
        
    };

    class Primitive{
    public:
        std::vector<float>          m_vertices;
        std::vector<unsigned int>   m_indices;
        Shader*                     m_shader;
    };

    class Mesh{
    public:
        std::string                 m_name;
        std::vector<Primitive>      m_primitives;

        Mesh() = default;
        Mesh(std::vector<Primitive> primitives, std::string name);
    };
    
    class Model{
    public:
        std::vector<Mesh>           m_meshes;

        Model() = default;
        Model(std::string path);

        bool LoadModel(std::string path);
        bool ParseVertexData();
        bool ParseIndexData();
        bool ParseMaterialData();

    };
}
