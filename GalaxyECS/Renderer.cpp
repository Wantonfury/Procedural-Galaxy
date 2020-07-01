#include "Renderer.hpp"

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char *message,
	const void *userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cerr << "---------------" << std::endl;
	std::cerr << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cerr << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cerr << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cerr << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cerr << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cerr << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cerr << "Source: Other"; break;
	} std::cerr << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cerr << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cerr << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cerr << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cerr << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cerr << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cerr << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cerr << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cerr << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cerr << "Type: Other"; break;
	} std::cerr << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cerr << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cerr << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cerr << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cerr << "Severity: notification"; break;
	} std::cerr << std::endl;
	std::cerr << std::endl;
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cerr << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

const std::unordered_map<Renderer::TEXTURE, const char*> Renderer::textureNames({
	{ TEXTURE_STAR_SPECTRUM, "star_spectrum.png" },
	{ TEXTURE_STAR_FLARE, "star_flare.png" },
	{ TEXTURE_GAS_GIANT, "gas_giant.png" }
	});

const GLfloat Renderer::coronaData[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
};

void Renderer::init()
{
	glfwMakeContextCurrent(NULL);

	glfwSetErrorCallback(Renderer::glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "Error: GLFW could not be initialised." << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	window = glfwCreateWindow(width, height, TITLE.c_str(), NULL, NULL);
	if (!window)
	{
		std::cerr << "GLFW Error: Game window could not be created. OpenGL version available may be below 2.0." << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, (mode->width - width) / 2, (mode->height - height) / 2);

	glfwSwapInterval(0);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cerr << "GLEW Error: GLEW initialisation failed." << std::endl;
		exit(EXIT_FAILURE);
	}

	glEnable(GL_DEPTH_TEST);
	glCheckError();
	glDepthFunc(GL_LEQUAL);
	glCheckError();

	glEnable(GL_CULL_FACE);
	glCheckError();
	glCullFace(GL_BACK);
	glCheckError();

	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);
	glCheckError();

	glEnable(GL_BLEND);
	glCheckError();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCheckError();

	glGenFramebuffers(1, &hdrFBO);
	glCheckError();
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glCheckError();

	glGenTextures(2, colourBuffers);
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, colourBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colourBuffers[i], 0);
	}

	glGenRenderbuffers(1, &rboDepth);
	glCheckError();
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glCheckError();
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glCheckError();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	glCheckError();

	attachments[0] = GL_COLOR_ATTACHMENT0;
	attachments[1] = GL_COLOR_ATTACHMENT1;
	glDrawBuffers(2, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Error: Framebuffer not complete." << std::endl;
		exit(EXIT_FAILURE);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColourBuffers);
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColourBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColourBuffers[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Error: Framebuffer not complete." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
	}
	
	loadTextures();
}

void Renderer::loadTextures()
{
	for (std::uint32_t i = 0; i < NUM_TEXTURES; ++i)
	{
		switch (i)
		{
		/*case TEXTURE_STAR_SPECTRUM:
			texturePool.insert({ i, std::make_shared<Texture>() });
			break;*/
		//case TEXTURE_STAR_FLARE:
				//texturePool.insert({ i, std::make_shared<Texture>() });
				//break;
		case TEXTURE_GAS_GIANT:
			texturePool.insert({ i, std::make_shared<Texture>() });
			glActiveTexture(GL_TEXTURE0 + i + 2);
			glBindTexture(GL_TEXTURE_2D, texturePool[TEXTURE_GAS_GIANT]->ID);
			break;
		}
		//texturePool.insert({ i, std::make_shared<Texture>() });
	}

	for (const auto&[key, val] : textureNames)
	{
		std::shared_ptr<Texture> texture = texturePool[key];
		if (texture == nullptr) continue;
		texture->fileName = val;

		assertFunc(loadPngImage((TEXTURE_PATH + texture->fileName).c_str(), texture->width, texture->height, texture->alpha, &texture->data), [&]()
		{
			std::cerr << "Could not load texture: " << texture->fileName << std::endl;
		});

		glGenTextures(1, &texture->ID);
		glCheckError();
		glBindTexture(GL_TEXTURE_2D, texture->ID);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glCheckError();
		//glTexImage2D(GL_TEXTURE_2D, 0, texture->alpha ? 4 : 3, texture->width, texture->height, 0, texture->alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, texture->data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture->data);
		glCheckError();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glCheckError();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glCheckError();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glCheckError();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glCheckError();

		std::cout << "Loaded texture: " << texture->fileName << "(" << texture->width << ", " << texture->height << ")" << std::endl;
		//glEnable(GL_TEXTURE_2D);
		//glCheckError();
	}
}

void Renderer::update(float dt)
{
	int width, height;
	float ratio;

	glfwMakeContextCurrent(window);

	if (Globals::Inputs.cursor) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetWindowSize(window, &width, &height);
	ratio = (float)width / (float)height;

	// Resize buffer textures in case screen size changes
	if (this->width != width || this->height != height)
	{
		this->width = width;
		this->height = height;

		for (int i = 0; i < 2; ++i)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glBindTexture(GL_TEXTURE_2D, colourBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);

			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
			glBindTexture(GL_TEXTURE_2D, pingpongColourBuffers[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glClearColor(0.0, 0.0, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	{
		// Wireframe
		if (Globals::Inputs.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto viewCamera = reg.view<CController, CTransform>();
		if (!viewCamera.empty())
		{
			CTransform& cameraTransform = viewCamera.get<CTransform>(viewCamera.front());
			CController& cameraData = viewCamera.get<CController>(viewCamera.front());

			// update height of near plane for point sprites
			int viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			heightOfNearPlane = (float)abs(viewport[3] - viewport[1]) / (2 * tan(0.5 * cameraData.fov * 3.141592653f / 180.0));

			renderStars(dt, cameraTransform, cameraData);
			renderSpheres(dt, cameraTransform, cameraData);
			renderCoronas(dt, cameraTransform, cameraData);
		}

		if (Globals::Inputs.wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool horizontal = true, first = true;
	int max = 20;

	shaderBlur->bind();
	for (int i = 0; i < max; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
		shaderBlur->setInt(Shader::BLUR_HORIZONTAL, horizontal);
		glBindTexture(GL_TEXTURE_2D, first ? colourBuffers[1] : pingpongColourBuffers[!horizontal]);
		renderScreenQuad();
		horizontal = !horizontal;
		if (first) first = false;
	}
	shaderBlur->unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shaderBloom->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colourBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColourBuffers[!horizontal]);
	shaderBloom->setInt(Shader::HDR_ENABLED, Globals::Inputs.hdr);
	shaderBloom->setFloat(Shader::HDR_EXPOSURE, exposure);
	shaderBloom->setInt(Shader::BLOOM, Globals::Inputs.bloom);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	if (Globals::Inputs.gui) renderGUI(dt);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	renderScreenQuad();
	shaderBloom->unbind();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Renderer::renderGUI(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	auto view = reg.view<CRenderable, CWindow>();
	for (const auto& entity : view)
	{
		auto& win = view.get<CWindow>(entity);

		win.draw();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::generateStars(CGalaxy& cluster)
{
	glGenVertexArrays(1, &cluster.vao);
	glBindVertexArray(cluster.vao);

	glEnableVertexAttribArray(0);
	glGenBuffers(1, &cluster.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cluster.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * cluster.stars.size(), &cluster.stars[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	glEnableVertexAttribArray(1);
	glGenBuffers(1, &cluster.colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cluster.colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * cluster.colours.size(), &cluster.colours[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::renderStars(float dt, CTransform& cameraTransform, CController& cameraData)
{
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_CLAMP);
	

	auto view = reg.view<CRenderable, CTransform, CGalaxy>();
	for (const auto& entity : view)
	{
		auto& renderable = view.get<CRenderable>(entity);
		auto& transform = view.get<CTransform>(entity);
		auto& galaxy = view.get<CGalaxy>(entity);
		std::shared_ptr<Shader> shader = Globals::shm->getShader(renderable.shaderFromSpace);

		CTransform transformRelative;
		transformRelative.position = cameraTransform.position - transform.position;
		transformRelative.rotation = transform.rotation;
		transformRelative.scale = transform.scale;

		glm::mat4 MV = getModelViewMatrix(transformRelative);
		glm::mat4 MVP = cameraData.PM * MV;

		if (!galaxy.vao) generateStars(galaxy);

		glBindVertexArray(galaxy.vao);
		shader->bind();
		shader->update(MV, MVP);
		shader->setFloat(Shader::HEIGHT_OF_NEAR_PLANE, heightOfNearPlane);

		glDrawArrays(GL_POINTS, 0, galaxy.starCount);

		shader->unbind();
	}

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void Renderer::generateCorona(CCorona& corona)
{
	glGenVertexArrays(1, &corona.vao);
	//glBindVertexArray(corona.vao);

	/*glEnableVertexAttribArray(0);
	glGenBuffers(1, &corona.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, corona.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(coronaData), &coronaData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);*/

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

void Renderer::generateFlare(CFlare& flare)
{
	glGenVertexArrays(1, &flare.vao);
}

void Renderer::renderFlares(float dt, CTransform& cameraTransform, CController& cameraData)
{
	//entt::entity sun = reg.view<CSun>().front();
	//auto& sunTransform = reg.get<CTransform>(sun);
	//auto& sunRenderable = reg.get<CRenderable>(sun);

	coronaShader->bind();

	auto view = reg.view<CRenderable, CTransform, CFlare, COrbit>();
	for (const auto& entity : view)
	{
		auto& transform = view.get<CTransform>(entity);
		auto& renderable = view.get<CRenderable>(entity);
		auto& flare = view.get<CFlare>(entity);
		auto& orbit = view.get<COrbit>(entity);

		if (!reg.valid(orbit.target) || !reg.has<CTransform>(orbit.target)) continue;
		auto& sunTransform = reg.get<CTransform>(orbit.target);
		auto& sun = reg.get<CSun>(orbit.target);

		transform.scale = sunTransform.scale * 1.0f;

		CTransform transformRelative;
		transformRelative.position = cameraTransform.position - transform.position;
		transformRelative.rotation = transform.rotation;
		transformRelative.scale = transform.scale;

		glm::mat4 MV = getModelViewMatrix(transformRelative);

		// Spherical billboard
		MV[0][0] = 1.0;
		MV[0][1] = 0.0;
		MV[0][2] = 0.0;

		MV[1][0] = 0.0;
		MV[1][1] = 1.0;
		MV[1][2] = 0.0;

		MV[2][0] = 0.0;
		MV[2][1] = 0.0;
		MV[2][2] = 1.0;

		glm::mat4 MVP = cameraData.PM * MV;

		if (!flare.vao) generateFlare(flare);
		coronaShader->update(MV, MVP);
		coronaShader->setFloat(Shader::RADIUS, transform.scale.x * 10.0f);
		coronaShader->setFloat(Shader::HEIGHT_OF_NEAR_PLANE, heightOfNearPlane);
		coronaShader->setFloat3(Shader::LIGHT_SUN_COLOUR, sun.color);

		glBindVertexArray(flare.vao);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	glBindVertexArray(0);
	coronaShader->unbind();
}

void Renderer::renderCoronas(float dt, CTransform& cameraTransform, CController& cameraData)
{
	coronaShader->bind();

	auto view = reg.view<CRenderable, CTransform, CCorona, COrbit>();
	for (const auto& entity : view)
	{
		auto& transform = view.get<CTransform>(entity);
		auto& renderable = view.get<CRenderable>(entity);
		auto& corona = view.get<CCorona>(entity);
		auto& orbit = view.get<COrbit>(entity);

		if (!reg.valid(orbit.target) || !reg.has<CTransform>(orbit.target)) continue;
		auto& sunTransform = reg.get<CTransform>(orbit.target);
		auto& sun = reg.get<CSun>(orbit.target);

		transform.scale = sunTransform.scale * 1.0f;

		CTransform transformRelative;
		transformRelative.position = cameraTransform.position - transform.position;
		transformRelative.rotation = transform.rotation;
		transformRelative.scale = transform.scale;

		glm::mat4 MV = getModelViewMatrix(transformRelative);

		// Spherical billboard
		MV[0][0] = 1.0;
		MV[0][1] = 0.0;
		MV[0][2] = 0.0;

		MV[1][0] = 0.0;
		MV[1][1] = 1.0;
		MV[1][2] = 0.0;

		MV[2][0] = 0.0;
		MV[2][1] = 0.0;
		MV[2][2] = 1.0;

		glm::mat4 MVP = cameraData.PM * MV;

		if (!corona.vao) generateCorona(corona);
		coronaShader->update(MV, MVP);
		coronaShader->setFloat(Shader::RADIUS, transform.scale.x * 10.0f);
		coronaShader->setFloat(Shader::HEIGHT_OF_NEAR_PLANE, heightOfNearPlane);
		coronaShader->setFloat3(Shader::LIGHT_SUN_COLOUR, sun.color);
		coronaShader->setFloat(Shader::DT, sun.time);

		glBindVertexArray(corona.vao);
		glDrawArrays(GL_POINTS, 0, 1);
	}

	glBindVertexArray(0);
	coronaShader->unbind();
}

void Renderer::renderSpheres(float dt, CTransform& cameraTransform, CController& cameraData)
{
	auto viewSun = reg.view<CTransform, CSun>();
	if (viewSun.empty()) return;

	CTransform& sunTransform = viewSun.get<CTransform>(viewSun.front());
	CSun& sun = viewSun.get<CSun>(viewSun.front());
	bool nearClip = false;
	bool isSun = false;

	auto viewPlanets = reg.view<CRenderable, CSphere, CTransform, CPlanet>();
	auto viewGasGiants = reg.view<CRenderable, CSphere, CTransform, CGasGiant>();
	auto viewOceans = reg.view<CRenderable, CSphere, CTransform, COcean>();
	auto viewAtmospheres = reg.view<CRenderable, CSphere, CTransform, CAtmosphere>();

	renderSphere(dt, cameraTransform, cameraData, sunTransform, sun, viewSun.front(), nearClip);
	for (const auto& entity : viewPlanets)
		renderSphere(dt, cameraTransform, cameraData, sunTransform, sun, entity, nearClip);

	for (const auto& entity : viewGasGiants)
		renderSphere(dt, cameraTransform, cameraData, sunTransform, sun, entity, nearClip);

	for (const auto& entity : viewOceans)
		renderSphere(dt, cameraTransform, cameraData, sunTransform, sun, entity, nearClip);

	for (const auto& entity : viewAtmospheres)
		renderSphere(dt, cameraTransform, cameraData, sunTransform, sun, entity, nearClip);
	
	if (nearClip)
	{
		cameraData.zNear = planetNear;
		cameraData.zFar = planetFar;
	}
	else
	{
		cameraData.zNear = spaceNear;
		cameraData.zFar = spaceFar;
	}
}

void Renderer::renderSphere(float dt, CTransform& cameraTransform, CController& cameraData, CTransform& sunTransform, CSun& sun, entt::entity entity, bool& nearClip)
{
	CSphere& sphere = reg.get<CSphere>(entity);
	CRenderable& renderable = reg.get<CRenderable>(entity);
	CTransform& transform = reg.get<CTransform>(entity);
	bool isAtmosphere = false;
	bool isPlanet = false;
	bool isOcean = false;
	bool isSun = false;
	bool isGasGiant = false;

	CTransform transformRelative;
	transformRelative.position = cameraTransform.position - transform.position;
	transformRelative.rotation = transform.rotation;
	transformRelative.scale = transform.scale;

	glm::mat4 MV = getModelViewMatrix(transformRelative);
	glm::mat4 MVP = cameraData.PM * MV;

	float innerRadius, outerRadius, camHeight;
	glm::vec3 lightPos, camPos;

	camPos = (transform.position - cameraTransform.position).getVec3f();
	camHeight = glm::length(camPos);
	lightPos = (transform.position - sunTransform.position).getVec3f();

	if (reg.has<CPlanet>(entity))
	{
		innerRadius = transform.scale.x;
		outerRadius = innerRadius * Globals::atmosphereRadius;
		isPlanet = true;
	}
	else if (reg.has<CAtmosphere>(entity))
	{
		CAtmosphere atmo = reg.get<CAtmosphere>(entity);
		if (!reg.valid(atmo.activeAtmosphere) || (reg.valid(atmo.activeAtmosphere) && entity == atmo.activeAtmosphere))
		{

			CTransform& planetTransform = reg.get<CTransform>(atmo.planet);

			innerRadius = planetTransform.scale.x;
			outerRadius = transform.scale.x;
			isAtmosphere = true;
		}
		else return;
	}
	else if (reg.has<COcean>(entity))
	{
		CTransform& planetTransform = reg.get<CTransform>(reg.get<COcean>(entity).planet);

		innerRadius = planetTransform.scale.x;
		outerRadius = innerRadius * Globals::atmosphereRadius;
		isOcean = true;
	}
	else if (reg.has<CSun>(entity))
	{
		innerRadius = transform.scale.x;
		outerRadius = innerRadius * Globals::atmosphereRadius;
		lightPos = (transform.position - cameraTransform.position).getVec3f();
		isSun = true;
	}
	else if (reg.has<CGasGiant>(entity))
	{
		innerRadius = transform.scale.x;
		outerRadius = innerRadius * Globals::atmosphereRadius;
		isGasGiant = true;
	}
	lightPos = glm::normalize(lightPos / glm::length(lightPos));

	CTransform origin;
	origin.position = vec64(0LL);
	origin.rotation = transform.rotation;
	origin.scale = transform.scale;

	std::shared_ptr<Shader> shader;
	if (camHeight >= outerRadius)
	{
		shader = Globals::shm->getShader(renderable.shaderFromSpace);
		sphere.inAtmosphere = false;
		if (isAtmosphere) CAtmosphere::activeAtmosphere = entt::null;
	}
	else
	{
		shader = Globals::shm->getShader(renderable.shaderFromAtmosphere);
		nearClip = true;
		sphere.inAtmosphere = true;
		if (isAtmosphere) CAtmosphere::activeAtmosphere = entity;
	}

	shader->bind();
	shader->update(MV, MVP);

	if (isSun)
	{
		shader->setFloat3(Shader::LIGHT_SUN_COLOUR, sun.color);
		shader->setFloat(Shader::LIGHT_SUN_STRENGTH, 1.0f);
		shader->setFloat(Shader::RADIUS, transform.scale.x);
		shader->setFloat(Shader::DT, sun.time);
		sun.time += 0.0085f * dt;
		isSun = false;
	}
	else if (isPlanet)
	{
		auto& planet = reg.get<CPlanet>(entity);
		shader->setFloat3(Shader::PLANET_TERRAIN_COLOR_1, planet.terrainColor1);
		shader->setFloat3(Shader::PLANET_TERRAIN_COLOR_2, planet.terrainColor2);
		shader->setFloat3(Shader::PLANET_OCEAN_COLOR, planet.oceanColor);
	}
	else if (isOcean)
	{
		auto& planet = reg.get<CPlanet>(reg.get<COcean>(entity).planet);
		shader->setFloat3(Shader::PLANET_OCEAN_COLOR, planet.oceanColor);
	}
	else if (isGasGiant)
	{
		auto& gas = reg.get<CGasGiant>(entity);
		shader->setFloat3(Shader::PLANET_OCEAN_COLOR, gas.colorModifier);
		shader->setInt(Shader::BLOOM_SCENE, 2);
	}

	shader->setMat4(Shader::ATMO_ORIGIN, getModelViewMatrix(origin));
	shader->setFloat(Shader::ATMO_INNER_RADIUS, innerRadius);
	shader->setFloat(Shader::ATMO_INNER_RADIUS2, innerRadius * innerRadius);
	shader->setFloat(Shader::ATMO_OUTER_RADIUS, outerRadius);
	shader->setFloat(Shader::ATMO_OUTER_RADIUS2, outerRadius * outerRadius);
	shader->setFloat(Shader::ATMO_CAMERA_HEIGHT, camHeight);
	shader->setFloat(Shader::ATMO_CAMERA_HEIGHT2, camHeight * camHeight);
	shader->setFloat3(Shader::ATMO_WAVELENGTH, glm::vec3(1.0f / pow(sphere.w1, 4.0f), 1.0f / pow(sphere.w2, 4.0f), 1.0f / pow(sphere.w3, 4.0f)));
	shader->setFloat3(Shader::ATMO_LIGHT_POS, lightPos);
	shader->setFloat3(Shader::ATMO_CAMERA_POS, camPos);
	shader->setFloat(Shader::ATMO_FKRESUN, sphere.fKr * sphere.eSun);
	shader->setFloat(Shader::ATMO_FKMESUN, sphere.fKm * sphere.eSun);
	shader->setFloat(Shader::ATMO_FKR4PI, sphere.fKr * 4.0f * 3.141592653f);
	shader->setFloat(Shader::ATMO_FKM4PI, sphere.fKm * 4.0f * 3.141592653f);
	shader->setFloat(Shader::ATMO_SCALE, 1.0f / (outerRadius - innerRadius));
	shader->setFloat(Shader::ATMO_SCALE_DEPTH, sphere.rayleighScaleDepth);
	shader->setFloat(Shader::ATMO_SCALE_OVER_SCALE_DEPTH, (1.0f / (outerRadius - innerRadius)) / sphere.rayleighScaleDepth);

	draw(sphere, shader, transformRelative.position.getVec3f() / transformRelative.scale.x, MVP);
	shader->unbind();

	if (isGasGiant)
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glActiveTexture(GL_TEXTURE0);
	}
}

void Renderer::renderScreenQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

glm::mat4 Renderer::getModelViewMatrix(CTransform& transform)
{
	glm::mat4 defaultMatrix = glm::mat4(1.0f);

	glm::mat4 posMatrix = glm::translate(defaultMatrix, transform.position.getVec3f());
	glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);
	glm::mat4 scaleMatrix = glm::scale(defaultMatrix, transform.scale);

	return posMatrix * rotMatrix * scaleMatrix;
}

bool Renderer::loadPngImage(const char* name, int& outWidth, int& outHeight, bool& outAlpha, GLubyte** outData)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	FILE *fp;

	if ((fp = fopen(name, "rb")) == NULL)
		return false;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return false;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return false;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	png_uint_32 width, height;
	int bit_depth;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
	outWidth = width;
	outHeight = height;

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	*outData = (unsigned char*)malloc(row_bytes * outHeight);

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for (int i = 0; i < outHeight; i++)
	{
		memcpy(*outData + (row_bytes * (outHeight - 1 - i)), row_pointers[i], row_bytes);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	return true;
}

bool Renderer::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Renderer::updateDeltaTime(float &dt)
{
	timeCurrent = glfwGetTime();
	dt = timeCurrent - timeLast;
	timeLast = timeCurrent;
}

void Renderer::glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error: " << description << std::endl;
}

void Renderer::setupShaders()
{
	shaderBlur = Globals::shm->getShader(Globals::shm->BLUR);
	shaderBlur->bind();
	shaderBlur->setInt(Shader::BLUR_IMAGE, 1);
	shaderBlur->unbind();

	shaderBloom = Globals::shm->getShader(Globals::shm->BLOOM);
	shaderBloom->bind();
	shaderBloom->setInt(Shader::BLOOM_SCENE, 0);
	shaderBloom->setInt(Shader::BLOOM_BLUR, 1);
	shaderBloom->unbind();

	coronaShader = Globals::shm->getShader(Globals::shm->CORONA);
	//flareShader = Globals::shm->getShader(Globals::shm->FLARE);
}

GLFWwindow* Renderer::getWindow()
{
	return window;
}

int Renderer::getWidth()
{
	return width;
}

int Renderer::getHeight()
{
	return height;
}

void Renderer::draw(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 center, glm::mat4& VPM)
{
	double t = (1.0 + sqrt(5.0)) / 2.0;

	std::vector<glm::dvec3> p({
		{ -1, t, 0 },{ 1, t, 0 },{ -1, -t, 0 },{ 1, -t, 0 },
		{ 0, -1, t },{ 0, 1, t },{ 0, -1, -t },{ 0, 1, -t },
		{ t, 0, -1 },{ t, 0, 1 },{ -t, 0, -1 },{ -t, 0, 1 },
		});
	std::vector<int> idx({
		0, 11, 5, 0, 5, 1, 0, 1, 7, 0, 7, 10, 0, 10, 11,
		1, 5, 9, 5, 11, 4, 11, 10, 2, 10, 7, 6, 7, 1, 8,
		3, 9, 4, 3, 4, 2, 3, 2, 6, 3, 6, 8, 3, 8, 9,
		4, 9, 5, 2, 4, 11, 6, 2, 10, 8, 6, 7, 9, 8, 1
		});

	//tris_rendered = 0;

	patchBegin(sphere, shader);

	for (int i = 0; i < idx.size() / 3; ++i)
	{
		glm::dvec3 p3[3] = { glm::normalize(p[idx[i * 3 + 0]]), glm::normalize(p[idx[i * 3 + 1]]), glm::normalize(p[idx[i * 3 + 2]]) };
		glm::dvec2 uv[3] = { glm::dvec2(0, 0), glm::dvec2(0.5, sqrt(3) / 2), glm::dvec2(1, 0) };

		drawRecursive(sphere, shader, p3[0], p3[1], p3[2], uv[0], uv[1], uv[2], center, VPM, sphere.size_max);
	}

	patchEnd(sphere, shader);

	//std::cout << "Size: " << tris_rendered << std::endl;
}

void Renderer::drawRecursive(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec2 uv1, glm::dvec2 uv2, glm::dvec2 uv3, glm::dvec3 center, glm::mat4& VPM, double size)
{
	double ratioSize = size * sphere.lodSize;
	double minSize = 0.01;
	glm::dvec3 edge_center[3] = { (p1 + p2) / 2.0, (p2 + p3) / 2.0, (p3 + p1) / 2.0 };
	bool edge_test[3];
	double angle[3];

	for (int i = 0; i < 3; ++i)
	{
		glm::dvec3 d = center + edge_center[i];
		edge_test[i] = glm::length(d) > ratioSize;
		angle[i] = acos(glm::clamp(glm::dot(edge_center[i], glm::normalize(d)), -1.0, 1.0));
	}

	//if (std::max(angle[0], std::max(angle[1], angle[2])) < boost::math::constants::pi<double>() / 2 - 0.2) return; //0.75) return;

	/*if (size < 0.25)
	{
		glm::dvec3 p[3] = { p1, p2, p3 };
		int xp = 0, xm = 0, yp = 0, ym = 0, zp = 0;

		for (int i = 0; i < 6; ++i)
		{
			glm::dvec3 v = glm::normalize(p[i % 3]);
			if (i >= 3) v *= 0.994;
			glm::dvec4 cs = VPM * glm::vec4(v.x, v.y, v.z, 1);
			if (cs.z < cs.w) zp++;
			if (cs.x < -cs.w) xm++;
			if (cs.x > cs.w) xp++;
			if (cs.y < -cs.w) ym++;
			if (cs.y > cs.w) yp++;
		}
		if (zp == 0 || xm == 6 || xp == 6 || ym == 6 || yp == 6) return;
	}*/

	if ((edge_test[0] && edge_test[1] && edge_test[2]) || size < minSize)
	{
		drawPatch(sphere, shader, p1, p2, p3, uv1, uv2, uv3, center, VPM, size);
		return;
	}

	glm::dvec3 p[6] = { p1, p2, p3, edge_center[0], edge_center[1], edge_center[2] };
	glm::dvec2 uv[6] = { uv1, uv2, uv3, (uv1 + uv2) * 0.5, (uv2 + uv3) * 0.5, (uv3 + uv1) * 0.5 };
	int idx[12] = { 0, 3, 5,   5, 3, 4,   3, 1, 4,   5, 4, 2 };
	bool valid[4] = { true, true, true, true };

	if (edge_test[0])
	{
		p[3] = p1;
		uv[3] = uv1;
		valid[0] = false;
	}
	if (edge_test[1])
	{
		p[4] = p2;
		uv[4] = uv2;
		valid[2] = false;
	}
	if (edge_test[2])
	{
		p[5] = p3;
		uv[5] = uv3;
		valid[3] = false;
	}

	for (int i = 0; i < 4; ++i)
	{
		if (valid[i])
		{
			int i1 = idx[3 * i + 0], i2 = idx[3 * i + 1], i3 = idx[3 * i + 2];
			drawRecursive(sphere, shader, glm::normalize(p[i1]), glm::normalize(p[i2]), glm::normalize(p[i3]), uv[i1], uv[i2], uv[i3], center, VPM, size / 2.0);
		}
	}
}

void Renderer::drawPatch(CSphere& sphere, std::shared_ptr<Shader>& shader, glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 p3, glm::dvec2 uv1, glm::dvec2 uv2, glm::dvec2 uv3, glm::dvec3 center, glm::mat4& VPM, double size)
{
	glm::dvec3 d2 = p2 - p1, d3 = p3 - p1;

	shader->setUniform4f(Shader::TERRAIN_N1, p1.x, p1.y, p1.z, 1);
	shader->setUniform4f(Shader::TERRAIN_ND2, d2.x, d2.y, d2.z, 1);
	shader->setUniform4f(Shader::TERRAIN_ND3, d3.x, d3.y, d3.z, 1);

	bool normalise = size > 0.0001 ? true : false;

	if (!normalise)
	{
		std::cout << "TESTEST" << std::endl;
		//std::cerr << "Not NORMALISED";
		//exit(-1);
	}

	glDrawElements(GL_TRIANGLES, sphere.indices, GL_UNSIGNED_INT, 0);
	//tris_rendered += sphere.patch_max * sphere.patch_max;

}

void Renderer::patchBegin(CSphere& sphere, std::shared_ptr<Shader>& shader)
{
	if (sphere.vao == 0)
	{
		std::vector<GLfloat> v;
		std::vector<GLuint> idx;

		for (int i = 0; i < sphere.patch_max + 1; ++i)
		{
			for (int j = 0; j < i + 1; ++j)
			{
				v.push_back(GLfloat(double(1.0 - double(i) / double(sphere.patch_max))));
				v.push_back(GLfloat(double(double(j) / double(sphere.patch_max))));
				v.push_back(GLfloat(0.0f));

				if (i > 0 && j > 0)
				{
					idx.push_back(GLuint(v.size() / 3 - 1));
					idx.push_back(GLuint(v.size() / 3 - 2));
					idx.push_back(GLuint(v.size() / 3 - 2 - i));

					if (j < i)
					{
						idx.push_back(GLuint(v.size() / 3 - 2 - i));
						idx.push_back(GLuint(v.size() / 3 - 1 - i));
						idx.push_back(GLuint(v.size() / 3 - 1));
					}
				}
			}
		}

		sphere.indices = idx.size();

		glGenVertexArrays(1, &sphere.vao);
		glBindVertexArray(sphere.vao);

		glGenBuffers(1, &(sphere.vbo));
		glGenBuffers(1, &(sphere.vboidx));
		
		glBindBuffer(GL_ARRAY_BUFFER, sphere.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * v.size(), &v[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.vboidx);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * idx.size(), &idx[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.vboidx);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glBindVertexArray(sphere.vao);

	if (sphere.renderInside)
	{
		glCullFace(GL_FRONT);
		glBlendFunc(GL_ONE, GL_ONE);
	}
}

void Renderer::patchEnd(CSphere& sphere, std::shared_ptr<Shader>& shader)
{
	glBindVertexArray(0);

	if (sphere.renderInside)
	{
		glCullFace(GL_BACK);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}