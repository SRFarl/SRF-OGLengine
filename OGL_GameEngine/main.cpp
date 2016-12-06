#include "Headers.h"
#include "SDLFrameRateController.h"
#include "StateManager.h"
#include "GameState.h"
#include "PauseState.h"

class CmainApp
{
public:
	CmainApp(int argc, char *argv[]);
	virtual ~CmainApp();

	bool BInit();
	void DebugCalls();
	bool HandleInput();
	void Shutdown();

	void RunMainLoop();

private:
	bool m_bVsync; //vsync
	bool m_bDebugOpenGL;
	bool m_bDebugGame;
	unsigned int m_uiFrameRate;
	float m_fCameraSens;

	StateManager* m_stateManager;
	SDLFrameRateController* m_GEframeRateController;
	SDLInputHandler* m_GEinput;

private: //SDL
	SDL_Window* m_Window;
	SDL_GLContext m_Glcontext;
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
	//go over the program arguments and assign anything
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
	//init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	//set opengl version to 4.3
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//then we say whether we want the core profile or the compatibility profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (m_bDebugOpenGL)
		//debug option
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	//create window
	m_Window = SDL_CreateWindow("RENAME ME", WINPOSX, WINPOSY, WINWIDTH, WINHEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (m_Window == NULL)
	{
		printf("%s - Window could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}
	//create gl context
	m_Glcontext = SDL_GL_CreateContext(m_Window);
	if (m_Glcontext == NULL)
	{
		printf("%s - OpenGL context could not be created! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	//check for errors
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		std::cerr << "Error: GLEW failed to initialise with message: " << glewGetErrorString(err) << std::endl;
		return false;
	}
	glGetError(); // to clear the error caused deep in GLEW

	//vsync
	if (SDL_GL_SetSwapInterval(m_bVsync ? 1 : 0) < 0)
	{
		printf("%s - Warning: Unable to set VSync! SDL Error: %s\n", __FUNCTION__, SDL_GetError());
		return false;
	}

	//debug info
	std::cout << "INFO: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "INFO: OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "INFO: OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	
	//set the mouse to be locked in the window
	//SDL_SetRelativeMouseMode(SDL_TRUE);

	glEnable(GL_DEPTH_TEST);

	//init input handler
	SDL_Keycode tempKeys[6] = { SDLK_w, SDLK_a, SDLK_s, SDLK_d, SDLK_p, SDLK_f };
	m_GEinput = new SDLInputHandler(tempKeys, sizeof(tempKeys) / sizeof(tempKeys[0]));

	//frame rate controller
	m_GEframeRateController = new SDLFrameRateController(FRAMERATE);

	//StateManager
	m_stateManager = new StateManager();
	m_stateManager->AddState(new GameState(m_stateManager, m_Window, m_GEinput));

	return true;
}

void CmainApp::DebugCalls()
{
	//all the debug calls are called from here
	m_GEframeRateController->DisplayDebugInfo();
}

bool CmainApp::HandleInput()
{
	//handle inputs and return if quit was pressed
	m_GEinput->UpdateSDLInputs();

	return m_GEinput->getQuitPressed();
}

void CmainApp::RunMainLoop()
{
	while (1)
	{
		//start the frame timer
		m_GEframeRateController->StartFrame();

		//handle inputs
		if (HandleInput() || m_stateManager->CheckIfEmpty())
			return;

		//handle states
		//nothing at the moment because there's only one state to handle in this demo

		//update states
		m_stateManager->Update(m_GEframeRateController->getDeltaT());

		//debug calls
		if (m_bDebugGame)
			DebugCalls();

		//delay for frame
		m_GEframeRateController->DelayForFrameRate();
	}
}

void CmainApp::Shutdown()
{
	//managers
	delete m_GEinput;
	delete m_GEframeRateController;
	delete m_stateManager;

	//clean GL and SDL
	SDL_GL_DeleteContext(m_Glcontext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	//initalise
	CmainApp *pCmainApp = new CmainApp(argc, argv);
	if (!pCmainApp->BInit())
	{
		pCmainApp->Shutdown();
		return 1;
	}

	//run
	pCmainApp->RunMainLoop();

	//shutdown
	pCmainApp->Shutdown();
	delete pCmainApp;

	return 0;
}