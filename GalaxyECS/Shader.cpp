#include "Shader.hpp"

const std::unordered_map<Shader::enumUniforms, const char*> Shader::uniformNames({
	{ TERRAIN_N1, "n1" },
	{ TERRAIN_ND2, "nd2" },
	{ TERRAIN_ND3, "nd3" },

	{ TRANSFORM_LOCAL, "localTransform" },
	{ TRANSFORM_WORLD, "worldTransform" },

	{ VIEW_PROJECTION, "viewProjection" },
	{ ALPHA, "alpha" },
	{ DT, "dt" },
	{ RADIUS, "radius" },
	{ HEIGHT_OF_NEAR_PLANE, "heightOfNearPlane" },
	{ STAR_SPECTRUM, "sunSpectrum" },
	{ HDR_ENABLED, "hdr" },
	{ HDR_EXPOSURE, "exposure" },

	{ BLUR_HORIZONTAL, "blurHorizontal" },
	{ BLUR_IMAGE, "blurImage" },
	{ BLUR_SIZE, "blurSize" },

	{ BLOOM, "bloom" },
	{ BLOOM_BUFFER, "hdrBuffer" },
	{ BLOOM_BLUR, "bloomBlur" },
	{ BLOOM_SCENE, "scene" },

	{ ATMO_COLOR, "atmoColor" },
	{ ATMO_CAMERA_POS, "v3CameraPos" },
	{ ATMO_CAMERA_HEIGHT, "fCameraHeight" },
	{ ATMO_CAMERA_HEIGHT2, "fCameraHeight2" },
	{ ATMO_LIGHT_POS, "v3LightPos" },
	{ ATMO_WAVELENGTH, "v3InvWavelength" },
	{ ATMO_INNER_RADIUS, "fInnerRadius" },
	{ ATMO_INNER_RADIUS2, "fInnerRadius2" },
	{ ATMO_OUTER_RADIUS, "fOuterRadius" },
	{ ATMO_OUTER_RADIUS2, "fOuterRadius2" },
	{ ATMO_FKRESUN, "fKrESun" },
	{ ATMO_FKMESUN, "fKmESun" },
	{ ATMO_FKR4PI, "fKr4PI" },
	{ ATMO_FKM4PI, "fKm4PI" },
	{ ATMO_SCALE, "fScale" },
	{ ATMO_SCALE_DEPTH, "fScaleDepth" },
	{ ATMO_SCALE_OVER_SCALE_DEPTH, "fScaleOverScaleDepth" },
	{ ATMO_ORIGIN, "atmoOrigin" },

	{ PLANET_TERRAIN_COLOR_1, "terrainColor1" },
	{ PLANET_TERRAIN_COLOR_2, "terrainColor2" },
	{ PLANET_OCEAN_COLOR, "oceanColor" },

	{ LIGHT_SUN_COLOUR, "sunColour" },
	{ LIGHT_SUN_STRENGTH, "sunStrength" }
	});

Shader::Shader(const std::string& fileName)
{
	this->fileName = fileName;

	program = glCreateProgram();
	shaders[VERTEX_SHADER] = createShader(this, loadShader(SHADER_PATH + fileName + ".vs"), GL_VERTEX_SHADER);
	shaders[FRAGMENT_SHADER] = createShader(this, loadShader(SHADER_PATH + fileName + ".fs"), GL_FRAGMENT_SHADER);

	for (int i = 0; i < NUM_SHADERS; ++i)
		glAttachShader(program, shaders[i]);

	glLinkProgram(program);
	checkForError(this, program, GL_LINK_STATUS, true, "Linking");

	glValidateProgram(program);
	checkForError(this, program, GL_VALIDATE_STATUS, true, "Validation");

	for (auto it = uniformNames.begin(); it != uniformNames.end(); ++it)
		uniforms[it->first] = glGetUniformLocation(program, it->second);

	std::cout << "Loaded shader: " << fileName << std::endl;
}

Shader::~Shader()
{

}

void Shader::update(glm::mat4& MV, glm::mat4& MVP)
{
	glUniformMatrix4fv(uniforms[TRANSFORM_LOCAL], 1, GL_FALSE, &MV[0][0]);
	glUniformMatrix4fv(uniforms[TRANSFORM_WORLD], 1, GL_FALSE, &MVP[0][0]);
}

void Shader::bind()
{
	glUseProgram(program);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::setFloat3(int location, glm::vec3 v)
{
	glUniform3fv(uniforms[location], 1, &v[0]);
}

void Shader::setFloat4(int location, glm::vec4 v)
{
	glUniform4fv(uniforms[location], 1, &v[0]);
}

void Shader::setMat4(int location, glm::mat4 v)
{
	glUniformMatrix4fv(uniforms[location], 1, GL_FALSE, &v[0][0]);
}

void Shader::setInt(int location, int v)
{
	glUniform1iv(uniforms[location], 1, &v);
}

void Shader::setFloat(int location, float v)
{
	glUniform1fv(uniforms[location], 1, &v);
}

void Shader::setUniform4f(int location, GLfloat f1, GLfloat f2, GLfloat f3, GLfloat f4)
{
	glUniform4f(uniforms[location], f1, f2, f3, f4);
}

GLuint Shader::createShader(Shader* shaderObj, const std::string& data, GLenum shaderType)
{
	if (data == "") return 0;
	const GLchar* shaderData = data.c_str();

	GLuint shader = glCreateShader(shaderType);

	if (shader == 0)
		std::cerr << "Error: Shader type " << shaderType << " could not be created." << std::endl;

	glShaderSource(shader, 1, &shaderData, NULL);
	glCompileShader(shader);

	checkForError(shaderObj, shader, GL_COMPILE_STATUS, false, "Compilation " + shaderType);

	return shader;
}

std::string Shader::loadShader(const std::string& fileName)
{
	std::ifstream file(fileName.c_str());
	std::string output, line;

	if (file.is_open())
		while (std::getline(file, line))
			output += line + "\n";
	else return "";//std::cerr << "Unable to load shader: " << fileName << std::endl;
	file.close();

	return output;
}

void Shader::checkForError(Shader *shaderObj, GLuint shader, GLuint flag, bool isProgram, const std::string& errorType)
{
	GLint success = 0;
	GLchar error[1024] = { 0 };

	if (isProgram) glGetProgramiv(shader, flag, &success);
	else glGetShaderiv(shader, flag, &success);

	if (success == GL_FALSE)
	{
		if (isProgram) glGetProgramInfoLog(shader, sizeof(error), NULL, error);
		else glGetShaderInfoLog(shader, sizeof(error), NULL, error);

		std::cerr << (isProgram ? "Program" : "Shader") << " (" << (shaderObj ? shaderObj->fileName : "") << ") " + errorType << " Error: " << error << std::endl;
	}
}