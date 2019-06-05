#ifndef SHADER_H
#define SHADER_H
#include "RefCount.h"
#include <string>
#include <glad/glad.h>
#include <stdint.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>

class Shader
{
	public:
		RefCount refCount;

    private:
		static std::map<std::string, Shader> mShaderCache;

        std::uint32_t mProgramId = 0;

        static std::uint32_t createShaderFromFiles(const std::vector<std::string>& paths, GLenum type, bool addVersion = true);

		static std::uint32_t createShader(const std::vector<std::string>& code, GLenum type, GLint& success, bool addVersion = true);

		static std::uint32_t createProgram(std::uint32_t vertexShader, std::uint32_t geometryShader, std::uint32_t fragmentShader, GLint& success);

        static void writeDebugShaderToFile(const std::string& source);

		Shader(std::uint32_t programId);

    public:
		static Shader loadFromFile(const std::vector<std::string>& vertexPaths,
			const std::vector<std::string>& geometryPaths,
			const std::vector<std::string>& fragmentPaths);

		static Shader loadFromFile(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);

		static std::string sourceFromFile(const std::string& path);

		static Shader fromCode(const std::vector<std::string>& vertexCode, const std::vector<std::string>& geometryCode, const std::vector<std::string>& fragmentCode);
		
		Shader();

        std::int32_t getLocationOf(const std::string& name, bool warning = true) const;

        void setFloat(const std::string& name, float value) const;

		void setFloat(std::int32_t location, float value) const;

        void setInt(const std::string& name, int value) const;

		void setInt(std::int32_t location, int value) const;

        void setMat4(const std::string& name, const glm::mat4& value) const;

		void setMat4(std::int32_t location, const glm::mat4& value) const;

        void setVec3(const std::string& name, const glm::vec3& value) const;

		void setVec3(std::int32_t location, const glm::vec3& value) const;

        void bindUniformBlock(const std::string& name, std::uint32_t bindingPoint);

        void use() const;

		operator bool() const;

		bool isValid() const;

        virtual ~Shader();
};

#endif // SHADER_H
