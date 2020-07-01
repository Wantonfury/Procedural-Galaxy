#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "Components.hpp"

class Shader
{
public:
	enum enumUniforms: std::uint32_t
	{
		TERRAIN_N1,
		TERRAIN_ND2,
		TERRAIN_ND3,

		TRANSFORM_LOCAL,
		TRANSFORM_WORLD,
		VIEW_PROJECTION,
		LIGHT_DIRECTION,
		LIGHT_POSITION,
		LIGHT_SUN_COLOUR,
		LIGHT_SUN_STRENGTH,
		LIGHT_AMBIENT_COLOUR,
		LIGHT_AMBIENT_STRENGTH,
		CAMERA_HEIGHT,
		ALPHA,
		DT,
		RADIUS,

		HEIGHT_OF_NEAR_PLANE,
		STAR_SPECTRUM,

		HDR_ENABLED,
		HDR_EXPOSURE,
		BLUR_HORIZONTAL,
		BLUR_IMAGE,
		BLUR_SIZE,
		BLOOM,
		BLOOM_BUFFER,
		BLOOM_BLUR,
		BLOOM_SCENE,

		ATMO_COLOR,
		ATMO_CAMERA_POS,
		ATMO_CAMERA_HEIGHT,
		ATMO_CAMERA_HEIGHT2,
		ATMO_LIGHT_POS,
		ATMO_WAVELENGTH,
		ATMO_INNER_RADIUS,
		ATMO_INNER_RADIUS2,
		ATMO_OUTER_RADIUS,
		ATMO_OUTER_RADIUS2,
		ATMO_FKRESUN,
		ATMO_FKMESUN,
		ATMO_FKR4PI,
		ATMO_FKM4PI,
		ATMO_SCALE,
		ATMO_SCALE_DEPTH,
		ATMO_SCALE_OVER_SCALE_DEPTH,
		ATMO_ORIGIN,

		PLANET_TERRAIN_COLOR_1,
		PLANET_TERRAIN_COLOR_2,
		PLANET_OCEAN_COLOR,

		NUM_UNIFORMS
	};

private:
	const static std::unordered_map<enumUniforms, const char*> uniformNames;

	enum
	{
		VERTEX_SHADER,
		FRAGMENT_SHADER,

		NUM_SHADERS
	};

	GLuint program;
	GLuint shaders[NUM_SHADERS];
	GLuint uniforms[NUM_UNIFORMS];
	GLuint attribs[2];

	std::string fileName;

	const std::string SHADER_PATH = "shaders/";

	static std::string loadShader(const std::string& fileName);
	static void checkForError(Shader* shaderObj, GLuint shader, GLuint flag, bool isProgram, const std::string& errorType);
	static GLuint createShader(Shader* shaderObj, const std::string& data, GLenum shaderType);

	static std::unordered_map<std::string, Shader*> shaderInstances;

public:
	Shader(const std::string& fileName);
	~Shader();

	void bind();
	void unbind();
	void update(glm::mat4& MV, glm::mat4& MVP);

	void setUniform4f(int location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4);
	void setInt(int location, int v);
	void setFloat(int location, float v);
	void setFloat3(int location, glm::vec3 v);
	void setFloat4(int location, glm::vec4 v);
	void setMat4(int location, glm::mat4 v);
};