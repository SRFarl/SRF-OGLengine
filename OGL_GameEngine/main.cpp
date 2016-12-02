#include "Headers.h"
#include "Shader.h"
#include "SDLInputHandler.h"
#include "SDLFrameRateController.h"
#include "SpinningSquare.h"
#include "FPSCamera.h"
#include "AssetBox.h"
#include "EntityEngine.h"
#include "Math.h"
#include "StaticObject.h"
#include "GameSphere.h"

class CmainApp
{
public:
	CmainApp(int argc, char *argv[]);
	virtual ~CmainApp();

	bool BInit();
	bool BInitShaders();
	bool HandleInput();
	void BInitGame();
	void DebugCalls();
	void Shutdown();

	void RunMainLoop();

	GLuint CompileGLShader(const char *pchShaderName, const char *pchVertexShader, const char *pchFragmentShader);

private:
	bool m_bVsync; //vsync
	bool m_bDebugOpenGL;
	bool m_bDebugGame;
	unsigned int m_uiFrameRate;
	float m_fCameraSens;

	AssetBox *m_assetBox;
	SDLInputHandler *m_GEinput;
	SDLFrameRateController *m_GEframeRateController;
	EntityEngine *m_entityEngine;

private: //SDL
	SDL_Window *m_Window;
	SDL_GLContext m_Glcontext;

private: //GL
	Shader *m_sceneShader;

private: //Game objects
	FPSCamera* m_fpsCam;
	PointLight* m_mainLight;
	DirectionalLight* m_mainDLight;

	std::vector<GameObject*> m_gOList;
	StaticObject* m_woodFloor;
	GameSphere* m_gameSphere;
};

CmainApp::CmainApp(int argc, char *argv[])
	: m_bVsync(false)
	, m_bDebugOpenGL(false)
	, m_bDebugGame(true)
	, m_uiFrameRate(30)
	, m_Window(NULL)
	, m_Glcontext(NULL)
	, m_fCameraSens(16.0f)
{
	for (int i = 1; i < argc; i++)
	{
		if (!_stricmp(argv[i], "-gldebug"))
		{
			m_bDebugOpenGL = true;
		}
		else if (!_stricmp(argv[i], "-vsync"))
		{
			m_bVsync = true;
		}
		else if (!_stricmp(argv[i], "-mousesens") && (argc > i + 1) && (*argv[i + 1] != '-'))
		{
			m_fCameraSens = atoi(argv[i + 1]);
			i++;
		}
	}

}

CmainApp::~CmainApp()
{
}

bool CmainApp::BInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	// This is how we set the context profile
	// We need to do this through SDL, so that it can set up the OpenGL drawing context that matches this
	// (of course in reality we have no guarantee this will be available and should provide fallback methods if it's not!)
	// Anyway, we basically first say which version of OpenGL we want to use
	// So let's say 4.3:
	// Major version number (4):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	// Minor version number (3):
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// Then we say whether we want the core profile or the compatibility profile
	// Flag options are either: SDL_GL_CONTEXT_PROFILE_CORE   or   SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
	// We'll go for the core profile
	// This means we are using the latest version and cannot use the deprecated functions
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (m_bDebugOpenGL)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	m_Window = SDL_CreateWindow("RENAME ME", WINPOSX, WINPOSY, WINWIDTH, WINHEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (m_Window == NULL)
	{
		printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}
	m_Glcontext = SDL_GL_CreateContext(m_Window);
	if (m_Glcontext == NULL)
	{
		printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: GLEW failed to initialise with message: " << glewGetErrorString(err) << std::endl;
		return false;
	}
	glGetError(); // to clear the error caused deep in GLEW

	if (SDL_GL_SetSwapInterval(m_bVsync ? 1 : 0) < 0)
	{
		printf("%s - Warning: Unable to set VSync! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	glEnable(GL_DEPTH_TEST);

	//init input handler
	SDL_Keycode tempKeys[4] = { SDLK_w, SDLK_a, SDLK_s, SDLK_d };
	m_GEinput = new SDLInputHandler(tempKeys, sizeof(tempKeys) / sizeof(tempKeys[0]));

	//frame rate controller
	m_GEframeRateController = new SDLFrameRateController(FRAMERATE);

	return true;
}

bool CmainApp::BInitShaders()
{
	m_sceneShader = new Shader("scene", "Shaders\\scene_shader.vs", "Shaders\\scene_shader.frag");
	if (m_sceneShader->getIsShaderOkay() != true)
	{
		return false;
	}

	return true;
}

bool CmainApp::HandleInput()
{
	m_GEinput->UpdateSDLInputs();

	return m_GEinput->getQuitPressed();
}

void CmainApp::BInitGame()
{
	m_assetBox = new AssetBox();
	m_entityEngine = new EntityEngine();

	m_mainLight = new PointLight;
	m_mainLight->position = glm::vec3(0.0f, 0.5f, 0.0f);
	m_mainLight->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	m_mainLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_mainLight->constant = 1.0f;
	m_mainLight->linear = 0.09f;
	m_mainLight->quadratic = 0.032f;
	m_entityEngine->AddRenderPointLight(m_mainLight);

	m_mainDLight = new DirectionalLight;
	m_mainDLight->direction = glm::vec3(-1.0f, -5.0f, 1.0f);
	m_mainDLight->diffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	m_mainDLight->specular = glm::vec3(1.0f, 1.0f, 1.0f);
	m_mainDLight->ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	m_entityEngine->AddRenderDirectionalLight(m_mainDLight);

	m_fpsCam = new FPSCamera(m_fCameraSens);
	m_entityEngine->AddCamera(m_fpsCam);

	//load assets
	m_assetBox->LoadAsset("sphere", "Models/Sphere/Sphere.obj");
	m_assetBox->LoadAsset("woodfloor", "Models/woodfloor/woodfloor.obj");

	//m_mainLight->m_tComp = new TransformComponent(glm::vec3(0.0f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);
	//m_mainLight->m_tNode = new TransformNode(m_mainLight->m_tComp);
	//m_entityEngine->AddTransformNode(m_mainLight->m_tNode);

	//m_mainLight->m_rComp = new RenderComponent(m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj());
	//m_mainLight->m_rComp->m_rcModel = m_assetBox->GetAsset("cube");
	//m_mainLight->m_renderNode = new RenderNode(m_mainLight->m_rComp, m_mainLight->m_tComp);
	//m_entityEngine->AddRenderNode(m_mainLight->m_renderNode);

	//m_testSpinningSquare = new SpinningSquare("test", m_assetBox->GetAsset("nanosuit"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, 0, 0), glm::vec3(glm::radians(10.0f), 0, 0));
	//m_gOList.push_back(m_testSpinningSquare);
	m_gameSphere = new GameSphere("sphere1", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0.1f, 8, 0), glm::vec3(0, 0, 0),true);
	m_gOList.push_back(m_gameSphere);
	m_gameSphere = new GameSphere("sphere2", m_assetBox->GetAsset("sphere"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), false);
	m_gOList.push_back(m_gameSphere);

	m_woodFloor = new StaticObject("floor", m_assetBox->GetAsset("woodfloor"), m_sceneShader->getShaderProgram(), m_fpsCam->GetView(), m_fpsCam->GetProj(), m_entityEngine, glm::vec3(0, -1, 0), glm::vec3(0, 0, 0));
	m_gOList.push_back(m_woodFloor);
}

void CmainApp::DebugCalls()
{
	m_GEframeRateController->DisplayDebugInfo();
}

void CmainApp::RunMainLoop()
{
	bool quit = false;
	while (quit == false)
	{
		m_GEframeRateController->StartFrame();

		quit = HandleInput();

		m_fpsCam->UpdateCamera(m_GEframeRateController->getDeltaT(), m_GEinput->getKeyPressed(SDLK_w),
				m_GEinput->getKeyPressed(SDLK_s), m_GEinput->getKeyPressed(SDLK_a), m_GEinput->getKeyPressed(SDLK_d),
				m_GEinput->getMouseXMotion(), m_GEinput->getMouseYMotion());

		for (std::vector<GameObject*>::iterator it = m_gOList.begin(); it != m_gOList.end();)
		{
			(*it)->Update(m_GEframeRateController->getDeltaT());
			it++;
		}

		m_entityEngine->Update(m_GEframeRateController->getDeltaT());

		SDL_GL_SwapWindow(m_Window);

		if (m_bDebugGame)
			DebugCalls();

		m_GEframeRateController->DelayForFrameRate();
	}
}

void CmainApp::Shutdown()
{
	//clean game
	delete m_mainLight;
	delete m_mainDLight;
	delete m_woodFloor;
	delete m_fpsCam;

	//clean app classes
	delete m_sceneShader;
	delete m_GEinput;
	delete m_GEframeRateController;
	delete m_entityEngine;
	delete m_assetBox;

	//clean GL and SDL
	SDL_GL_DeleteContext(m_Glcontext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	CmainApp *pCmainApp = new CmainApp(argc, argv);

	if (!pCmainApp->BInit() || !pCmainApp->BInitShaders())
	{
		pCmainApp->Shutdown();
		return 1;
	}

	pCmainApp->BInitGame();

	pCmainApp->RunMainLoop();

	pCmainApp->Shutdown();

	return 0;
}