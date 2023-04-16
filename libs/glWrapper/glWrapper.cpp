#include "glWrapper.hpp"
#include "../tinygltf/stb_image.h"

// 
// *SHADER
// 

static std::string ExtractFile(std::string path) // Extracts the contents of a file to a string
{
    std::ifstream file;
    std::string line;
    std::string returnValue;
    file.exceptions(/*std::ifstream::failbit |*/ std::ifstream::badbit); // Set fstream exceptions (failbit removed for unneccesary exceptions)

    try
    {
        file.open(path);

        while(std::getline(file, line)){ // Extract lines
        returnValue += line + '\n';
        }
    }
    catch(std::ifstream::failure e)
    {
        std::cout << e.what() << '\n';
    }
    
    return returnValue; // Return string
}

static void CreateShader(unsigned int &id, GLenum type, std::string code){
    id = glCreateShader(type);
    const char* source = code.c_str();

    glShaderSource(id, 1, &source, NULL);

    glCompileShader(id);

    int success; // Error handle type
    char log[512]; // Error message
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(id, 512, NULL, log);
        std::cout << "FAILED COMPILE: " << log << '\n';
    }

    return;
}

glWrap::Shader::Shader(std::string vertexPath, std::string fragmentPath){
    
    unsigned int vertex, fragment; // Shader objects

    CreateShader(vertex, GL_VERTEX_SHADER, ExtractFile(vertexPath));
    CreateShader(fragment, GL_FRAGMENT_SHADER, ExtractFile(fragmentPath));


    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);

    glLinkProgram(m_ID);

    int success; // Error handle type
    char log[512]; // Error message
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, log);
        std::cout << "FAILED LINK: " << log << '\n';
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return;
}

void glWrap::Shader::Use(){
    glUseProgram(m_ID);
}

void glWrap::Shader::SetBool(const std::string &name, bool value) const{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void glWrap::Shader::SetInt(const std::string &name, int value) const{
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetFloat(const std::string &name, float value) const{
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void glWrap::Shader::SetMatrix4(const std::string &name, glm::mat4 mat) const{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

// 
// *TEXTURE
// 

static GLenum GetChannelType(unsigned int channels){
    switch(channels)
    {
        case 1:
        return GL_RED;

        case 2:
        return GL_RG;

        case 3:
        return GL_RGB;

        case 4:
        return GL_RGBA;
    }

    return GL_RGB;
}

glWrap::Texture2D::Texture2D(std::string image, bool flip, GLenum filter, GLenum desiredChannels){
    stbi_set_flip_vertically_on_load(flip);
    int width, height, channels;
    unsigned char *data = stbi_load(image.c_str(), &width, &height, &channels, 0);

    // std::cout << channels << " channels\n";

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, desiredChannels, width, height, 0, GetChannelType(channels), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else std::cout << "Texture not loaded correctly\n";

    stbi_image_free(data);
}

void glWrap::Texture2D::SetActive(unsigned int unit){
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

glWrap::Model::Model(std::string path){
    LoadModel(path);
}

bool glWrap::Model::ParseVertexData(){

}

bool glWrap::Model::LoadModel(std::string path)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int materialIndex;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool success = loader.LoadASCIIFromFile(&model, &err, &warn, path);

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << '\n';
    }

    if (!err.empty()) {
        std::cout << "Error: " << err << '\n';
    }

    if (!success) {
        std::cout << "Failed to load glTF file\n";
        return 0;
    }

    const tinygltf::Scene& scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

    const tinygltf::Node& node = model.nodes[scene.nodes[0]];

    const tinygltf::Mesh& mesh = model.meshes[node.mesh];

    if (mesh.primitives.empty()) {
        std::cout << "No primitives found in mesh\n";
        return 0;
    }

    const tinygltf::Primitive& primitive = mesh.primitives[0];

    for (auto const& mesh : model.meshes){

        for (auto const& primitive : mesh.primitives){

            // Get vertex data

            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
            const tinygltf::BufferView& positionView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer& positionBuffer = model.buffers[positionView.buffer];
            const void* positionData = &positionBuffer.data[positionAccessor.byteOffset + positionView.byteOffset];

            const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.at("NORMAL")];
            const tinygltf::BufferView& normalView = model.bufferViews[normalAccessor.bufferView];
            const tinygltf::Buffer& normalBuffer = model.buffers[normalView.buffer];
            const void* normalData = &normalBuffer.data[normalAccessor.byteOffset + normalView.byteOffset];

            const tinygltf::Accessor& texCoordAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
            const tinygltf::BufferView& texCoordView = model.bufferViews[texCoordAccessor.bufferView];
            const tinygltf::Buffer& texCoordBuffer = model.buffers[texCoordView.buffer];
            const void* texCoordData = &texCoordBuffer.data[texCoordAccessor.byteOffset + texCoordView.byteOffset];
            
            const int numVertices = positionAccessor.count;
            const int numFloats = numVertices * 8; 

            vertices.resize(numFloats);

            for (int i{}; i < numVertices; ++i){
                const float* position = reinterpret_cast<const float*>(positionData) + i * 3;
                const float* normal = reinterpret_cast<const float*>(normalData) + i * 3;
                const float* texCoord = reinterpret_cast<const float*>(texCoordData) + i * 2;

                std::vector<float> vertex;
                vertex.resize(8);

                vertex[0] = position[0];
                vertex[1] = position[1];
                vertex[2] = position[2];
                vertex[3] = normal[0];
                vertex[4] = normal[1];
                vertex[5] = normal[2];
                vertex[6] = texCoord[0];
                vertex[7] = texCoord[1];

                vertices.insert(vertices.end(), vertex.begin(), vertex.end());
            }


        }
    }

    // Get index data

    const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
    const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];
    const void* indexData = &indexBuffer.data[indexAccessor.byteOffset + indexView.byteOffset];

    const int numIndices = indexAccessor.count;
    const int indexByteSize = indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? 2 : 4;
    const int indexFloats = numIndices * indexByteSize / sizeof(float);

    indices.resize(indexFloats);

    if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
        const uint16_t* index16 = reinterpret_cast<const uint16_t*>(indexData);

        for (int i = 0; i < numIndices; i++) {
            indices[i] = index16[i];
        }
    }
    else {
        const uint32_t* index32 = reinterpret_cast<const uint32_t*>(indexData);

        for (int i = 0; i < numIndices; i++) {
            indices[i] = index32[i];
        }
    }

    // Get material index

    materialIndex = primitive.material;
}
