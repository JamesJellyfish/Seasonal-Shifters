#include "Game.h"
#include "ExampleBaseScene.h"
#include "MyContactListener.h"
#include <random>

ContactListener listener;

Game::~Game()
{
	//If window isn't equal to nullptr
	if (m_window != nullptr)
	{
		//Delete window
		delete m_window;
		//set window to nullptr
		m_window = nullptr;
	}

	//Goes through the scenes and deletes them
	for (unsigned i = 0; i < m_scenes.size(); i++)
	{
		if (m_scenes[i] != nullptr)
		{
			delete m_scenes[i];
			m_scenes[i] = nullptr;
		}
	}
}

void Game::InitGame()
{

	//Initializes the backend x y
	//BackEnd::InitBackEnd(719.f, 436.f);
	BackEnd::InitBackEnd(1400.f, 800.f);

	//Grabs the initialized window
	m_window = BackEnd::GetWindow();

	//Creates a new HelloWorld scene
	m_scenes.push_back(new ExampleBaseScene("Seasonal Shifters"));
	//m_scenes.push_back(new ExampleBaseScene("Level 2"));
	//m_scenes.push_back(new Scene("Level 3"));
	//Sets active scene reference to our scene
	m_activeScene = m_scenes[0];

	m_activeScene->InitScene(float(BackEnd::GetWindowWidth()), float(BackEnd::GetWindowHeight()));

	//*m_activeScene = File::LoadJSON("Hello World.json");

	//Play the game title music
	PlaySound(TEXT("OPening_SOng.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);

	//Setting the init x position of the hidden box
	autoScroll = 120;

	//Sets m_register to point to the register in the active scene
	m_register = m_activeScene->GetScene();

	BackEnd::SetWindowName(m_activeScene->GetName());

	PhysicsSystem::Init();

	m_activeScene->GetPhysicsWorld().SetContactListener(&listener);
}

bool Game::Run()
{
	//While window is still open
	while (m_window->isOpen())
	{
		//Clear window with activescene clearColor
		m_window->Clear(m_activeScene->GetClearColor());
		//Updates the game
		Update();
		//Draws the game
		BackEnd::Draw(m_register);

		//Draws ImGUI
		if (m_guiActive)
			GUI();

		//Flips the windows
		m_window->Flip();

		//Polls events and then checks them
		BackEnd::PollEvents(m_register, &m_close, &m_motion, &m_click, &m_wheel);
		CheckEvents();

		//does the window have keyboard focus?
		if (Input::m_windowFocus)
		{
			//Accept all input
			AcceptInput();
		}
	}

	return true;
}

void Game::Update()
{
	//Makes the camera focus on this entity
	/*ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()));*/

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(5));
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(5));
	
	//When the remainder of time is equal to 0 move the box
	timeStamp = Timer::time;
	if (remainder(timeStamp, 0.25) == 0) {
		UpdateBox();
	}
	
	StartBox();
	WinScreen();
	GameOverScreen();

	//Update timer
	Timer::Update();
	//Update the backend
	BackEnd::Update(m_register);

	//Update Physics System
	PhysicsSystem::Update(m_register, m_activeScene->GetPhysicsWorld());

	//Updates the active scene
	m_activeScene->Update();
}

void Game::GUI()
{
	UI::Start(BackEnd::GetWindowWidth(), BackEnd::GetWindowHeight());

	ImGui::Text("Place your different tabs below.");

	if (ImGui::BeginTabBar(""))
	{
		BackEnd::GUI(m_register, m_activeScene);

		ImGui::EndTabBar();
	}

	UI::End();
}

void Game::CheckEvents()
{
	if (m_close)
		m_window->Close();

	//if (m_motion)
		//MouseMotion(BackEnd::GetMotionEvent());

	//if (m_click)
		//MouseClick(BackEnd::GetClickEvent());

	//if (m_wheel)
		//MouseWheel(BackEnd::GetWheelEvent());
}

void Game::AcceptInput()
{
	XInputManager::Update();

	//Just calls all the other input functions 
	GamepadInput();

	KeyboardHold();
	KeyboardDown();
	KeyboardUp();

	//Resets the key flags
	//Must be done once per frame for input to work
	Input::ResetKeys();
}

void Game::GamepadInput()
{
	XInputController* tempCon;
	//Gamepad button stroked (pressed)
	for (int i = 0; i < 3; i++)
	{
		if (XInputManager::ControllerConnected(i))
		{
			tempCon = XInputManager::GetController(i);
			tempCon->SetStickDeadZone(0.1f);

			//If the controller is connected, we run the different input types
			GamepadStroke(tempCon);
			GamepadUp(tempCon);
			GamepadDown(tempCon);
			GamepadStick(tempCon);
			GamepadTrigger(tempCon);
		}
	}
}

void Game::GamepadStroke(XInputController * con)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->GamepadStroke(con);
}

void Game::GamepadUp(XInputController * con)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->GamepadUp(con);
}

void Game::GamepadDown(XInputController * con)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->GamepadDown(con);
}

void Game::GamepadStick(XInputController * con)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->GamepadStick(con);
}

void Game::GamepadTrigger(XInputController * con)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->GamepadTrigger(con);
}

void Game::KeyboardHold()
{
	auto& animController = ECS::GetComponent<AnimationController>(1);

	if (Input::GetKey(Key::S))
	{

		animController.SetActiveAnim(0);
	}
	if (Input::GetKey(Key::W))
	{
		if (m_face == true) {
			animController.SetActiveAnim(1);
		}
		else {
			animController.SetActiveAnim(5);
		}
	}
	if (Input::GetKey(Key::A))
	{
		setFace(false);
		animController.SetActiveAnim(2);
	}
	if (Input::GetKey(Key::D))
	{
		setFace(true);
		animController.SetActiveAnim(3);
	}

	/*auto body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();
	b2Vec2 velocity = b2Vec2(0.f, 0.f);
	float inputVal = 15.f;
	if (Input::GetKeyDown(Key::E))
	{
		velocity += b2Vec2(inputVal + 200, 0.f);
	}
	body->SetLinearVelocity(velocity);*/
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.

	/*if (Input::GetKey(Key::C))
	{
		autoScroll += 5;
		ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		
	}*/
	
	

	m_activeScene->KeyboardHold();
}

void Game::KeyboardDown()
{
	
		if (Input::GetKeyDown(Key::Enter) )
		{
			if (onceEnter == false) {
				ECS::DestroyEntity(6);
				PlaySound(TEXT("Cool_song.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
				ECS::GetComponent<Transform>(5).SetPositionX(120);
				onceEnter = true;
			}
		}
	
	

	//jumping height and dash changes
	auto body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

	b2Vec2 vel = body->GetLinearVelocity();

	//jump only on ground
	if (listener.GetJump() || listener.GetGrounded() || listener.GetIce())
	{
		if (Input::GetKeyDown(Key::W))
		{
		float impulse = body->GetMass() * 77;
		body->ApplyLinearImpulse(b2Vec2(0, impulse), body->GetWorldCenter(), true);
		listener.SetGrounded(false);
		}
	}

	if (Input::GetKeyDown(Key::E))
	{
		float impulse = body->GetMass() * 40;
		body->ApplyLinearImpulse(b2Vec2(impulse, 0), body->GetWorldCenter(), true);
	}

	if (listener.GetIce())
	{
		if (Input::GetKey(Key::A))
		{
			vel.x = -50;//upwards - don't change y velocity
			body->SetLinearVelocity(vel);
		}
		if (Input::GetKey(Key::D))
		{
			vel.x = 50;//upwards - don't change y velocity
			body->SetLinearVelocity(vel);
		}
	}
	
	//if (Input::GetKeyDown(Key::F))
	//{
	//	//autoS = false;
	//	std::cout << ECS::GetComponent<Transform>(1).GetPositionX() << std::endl;

	//}
	//if (Input::GetKeyDown(Key::Z))
	//{
	//	//autoS = false;
	//	std::cout << ECS::GetComponent<Transform>(1).GetPositionY() << std::endl;

	//}


	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->KeyboardDown();
}

void Game::KeyboardUp()
{
	

	/*if (Input::GetKeyUp(Key::F1))
	{
		if (!UI::m_isInit)
		{
			UI::InitImGUI();
		}
		m_guiActive = !m_guiActive;
	}*/
	/*if (Input::GetKeyUp(Key::P))
	{
		PhysicsBody::SetDraw(!PhysicsBody::GetDraw());
	}*/

	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->KeyboardUp();
}

void Game::MouseMotion(SDL_MouseMotionEvent evnt)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->MouseMotion(evnt);

	if (m_guiActive)
	{
		ImGui::GetIO().MousePos = ImVec2(float(evnt.x), float(evnt.y));

		if (!ImGui::GetIO().WantCaptureMouse)
		{

		}
	}

	//Resets the enabled flag
	m_motion = false;
}

void Game::MouseClick(SDL_MouseButtonEvent evnt)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->MouseClick(evnt);

	if (m_guiActive)
	{
		ImGui::GetIO().MousePos = ImVec2(float(evnt.x), float(evnt.y));
		ImGui::GetIO().MouseDown[0] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
		ImGui::GetIO().MouseDown[1] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));
		ImGui::GetIO().MouseDown[2] = (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE));
	}

	//Resets the enabled flag
	m_click = false;
}

void Game::MouseWheel(SDL_MouseWheelEvent evnt)
{
	//Active scene now captures this input and can use it
	//Look at base Scene class for more info.
	m_activeScene->MouseWheel(evnt);

	if (m_guiActive)
	{
		ImGui::GetIO().MouseWheel = float(evnt.y);
	}
	//Resets the enabled flag
	m_wheel = false;
}


void Game::UpdateBox()
{
	if (autoS == true) 
	{
		//Beginning Spring Speed Up
		if (ECS::GetComponent<Transform>(5).GetPositionX() <= 300) {
			autoScroll += 1.25;
			ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		}
		//Summer Auto Scroll Speed
		else if (ECS::GetComponent<Transform>(1).GetPositionX() >= 615 && ECS::GetComponent<Transform>(1).GetPositionX() <=1376) {
			autoScroll += 0.30;
			ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		}
		//Fall Auto Scroll Speed
		else if (ECS::GetComponent<Transform>(1).GetPositionX() >= 1378 && ECS::GetComponent<Transform>(1).GetPositionX() <= 2132) {
			autoScroll += 0.40;
			ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		}
		//Winter Auto Scroll Speed
		else if (ECS::GetComponent<Transform>(1).GetPositionX() >= 2143 ) {
			autoScroll += 0.50;
			ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		}
		//Spring Auto Scroll Speed
		else {
			autoScroll += 0.20;
			ECS::GetComponent<Transform>(5).SetPositionX(autoScroll);
		}
		
	}
}

void Game::StartBox()
{
	float player;

	player = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	
	if (player >= 83 && player <=110) {
		autoS = true;
		
	}

	
}

void Game::GameOverScreen()
{
	gameOver = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPositionX();
	playerOver = ECS::GetComponent<Transform>(5).GetPositionX();

	if ((playerOver - gameOver) >= 270.f) 
	{
		autoS = false;
		ECS::GetComponent<Transform>(5).SetPositionX(0);
		createGameOverScreen();		
	};

}

void Game::WinScreen()
{
	
	if (ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPositionX() >= 3170)
	{
		autoS = false;
		ECS::GetComponent<Transform>(5).SetPositionX(0);
		createWinScreen();
	};
}

void Game::createGameOverScreen()
{
	if(LoseO == false){
		//Creates entity Game Over Screen
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "GameOver.png";//20 20
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, (1024 / 4) + 7, 608 / 4);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(128.f, 0.f, 20));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "GameOver");
		LoseO = true;
	}
}

void Game::createWinScreen()
{
	if (WinO == false) {
		//Creates entity Game Over Screen
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "Win.png";//20 20
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, (1024 / 4) + 7, 608 / 4);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(128.f, 0.f, 20));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Win");
		WinO = true;
	}
}

bool Game::getFace()
{
	return (m_face);
}

void Game::setFace(bool f)
{

	m_face = f;
	
}
