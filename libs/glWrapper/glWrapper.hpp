#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <array>

#include "../gl/glad.h"

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
    };
}