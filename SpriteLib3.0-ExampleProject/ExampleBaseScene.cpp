#include "ExampleBaseScene.h"
#include "Utilities.h"
#include "Timer.h"
#include "EffectManager.h"
#include "Box2D/Box2D.h"
#include "MyContactListener.h"


ExampleBaseScene::ExampleBaseScene(std::string name)
	: Scene(name)
{
	//Gravity
	m_gravity = b2Vec2(0.f, -100.f);
	m_physicsWorld->SetGravity(m_gravity);
}

void ExampleBaseScene::InitScene(float windowWidth, float windowHeight)
{
	if (m_isInit) {
		return;
	}
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup Main Camera entity E:0
	{
		//Creates Camera entity
		auto entity = ECS::CreateEntity();

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//attaches the camera to the vertical scroll
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::CameraBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Horizontal Scrolling Cam");
		ECS::SetIsMainCamera(entity, true);
	}

	//Setup new Entity Box Main Player E:1
	{
		//Our attack animation file
		auto Move = File::LoadJSON("Dan.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Main Player
		EntityIdentifier::MainPlayer(entity);

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		ECS::AttachComponent<MainCharacter>(entity);

		//Sets up components
		//std::string fileName = "RetiringStudios.jpg";//20 20
		std::string fileName = "Dan.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["IdleRight"]); //	0
		animController.AddAnimation(Move["JumpRight"]); //	1
		animController.AddAnimation(Move["WalkingLeft"]);// 2
		animController.AddAnimation(Move["WalkingRight"]);//3
		animController.AddAnimation(Move["Dash"]);		//	4
		animController.AddAnimation(Move["JumpLeft"]);	//	5
		animController.AddAnimation(Move["IdleLeft"]);	//	6

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 11/1.5, 16/1.5, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(11.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-127.f, 0.f, 15.f));

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		//Physics body covers the entire sprite
		//ID type is player
		//Collides with Enemies and Environments
		//Moves
		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-127.f), float32(-20.f));
		//tempDef.position.Set(float32(50.f), float32(5.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempBody->SetUserData((void*)entity);
		tempBody->SetEntityNumber(entity);
		tempBody->SetEntityType(0);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

		//Makes the box not able to rotate
		tempBody->SetFixedRotation(true);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::MainCharacterBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Main Box");
		ECS::SetIsMainPlayer(entity, true);
	}

	//Ground E:2
	{
		
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<Ground>(entity);

		std::string filename = "B.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 5385, 10);
		ECS::GetComponent<Transform>(entity).SetPosition(0.f, -75.f, 5.f);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(-75.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempBody->SetUserData((void*)entity);
		tempBody->SetEntityNumber(entity);
		tempBody->SetEntityType(1);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);
	
		unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit()|EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "ground test");

	}
	
	//Creates entity Blue Sky E:3
	{
		
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "Sky2B.png";
		//ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100 * 5, 45 * 5);
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, (11296 / 3.41)+600, 512 / 3.41);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1524.8f, 0.f, 8.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "BackSkys");
	}
	//Creates entity Clouds E:4
	{
		
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		//std::string fileName = "treesV1.png";
		//std::string fileName = "winter.png";
		std::string fileName = "Cloud9.png";
		//ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 100 * 5, 45 * 5);
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, (11296 / 3.41) + 500, 512 / 3.41);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(1524.8f, 0.f, 9.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Clouds");
	}
	//Creates entity Hidden Box E:5
	{
		
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "RetiringStudios.jpg";//20 20
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 0.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Unkown");
	}
	//Creates entity Title Screen E:6
	{
		
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Sets up components
		std::string fileName = "Game Title Image.png";//20 20
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, (1024/4) + 7, 608/4);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 25));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Title");
	}
	//Creates entity Level E:7
	{
		
		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		//ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Transform>(entity);
		//ECS::AttachComponent<Ground>(entity);

		std::string filename = "Level.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 11296/3.41, 512/ 3.41);
		ECS::GetComponent<Transform>(entity).SetPosition(1524.8f, 0.f, 10.f);

		unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() /*| EntityIdentifier::PhysicsBit()*/;
		ECS::SetUpIdentifier(entity, bitHolder, "level");
	}
	//Setup new Entity Sign Walking  E:8
	{
		//Our attack animation file
		auto Move = File::LoadJSON("WalkingSign.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		
		//Sets up components
		std::string fileName = "walkSign.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["WalkSign"]); //	0
		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128/3, 128/3, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 128.f), vec2(128.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-115.f, -26.f, 11.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "SignWalking");
		
	}
	//Setup new Entity Sign Jumping E:9
	{
		//Our attack animation file
		auto Move = File::LoadJSON("JumpingSign.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//Sets up components
		std::string fileName = "jumpSign.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["JumpSign"]); //	0
		animController.SetActiveAnim(0);

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128 / 3, 128 / 3, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 128.f), vec2(128.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-75.f, -26.f, 11.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "SignJump");

	}
	//Setup new Entity Bunny E:10
	{
		//Bunny animation file
		auto Move = File::LoadJSON("Bunny.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		//ECS::AttachComponent<Ground>(entity);
		//ECS::AttachComponent<Water>(entity);

		//Sets up components
		std::string fileName = "bunny.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["BunnyRight"]); //	0
		animController.AddAnimation(Move["BunnyLeft"]); //	1
		animController.SetActiveAnim(1);

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-75.f, -40.f, 12.f));


		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(-75.f), float32(-40.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempBody->SetUserData((void*)entity);
		tempBody->SetEntityNumber(entity);
		tempBody->SetEntityType(1);

		tempBody->SetFixedRotation(true);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Bunny");
	}
	//Setup new Entity Flag E:11
	{
		//Our attack animation file
		auto Move = File::LoadJSON("Flag.json");

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Add components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//Sets up components
		std::string fileName = "flag.png";
		auto& animController = ECS::GetComponent<AnimationController>(entity);
		animController.InitUVs(fileName);

		animController.AddAnimation(Move["Down"]); //	0
		animController.AddAnimation(Move["Rising"]); //	1
		animController.AddAnimation(Move["Top"]); //	2
		animController.SetActiveAnim(2);

		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128 / 4, 128 / 4, true, &animController);
		ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 128.f), vec2(128.f, 1.f));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(3170.f, 35.f, 12.f));

		//Sets up the Identifier
		unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
		ECS::SetUpIdentifier(entity, bitHolder, "Flag");
		//3180
	}


	//All PB
	{

		//Walls - Start and End
		{
			//Wall Start E:12
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 150);
				ECS::GetComponent<Transform>(entity).SetPosition(-137.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-137.f), float32(0.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

		
		}
		//Level 1 E:13
		{

		auto entity = ECS::CreateEntity();

		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<Ground>(entity);

		std::string filename = "B.png";

		ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 6000, 19);
		ECS::GetComponent<Transform>(entity).SetPosition(0.f, -75.f, 5.f);

		auto& tempSpr = ECS::GetComponent<Sprite>(entity);
		auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(-75.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempBody->SetUserData((void*)entity);
		tempBody->SetEntityNumber(entity);
		tempBody->SetEntityType(1);

		tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

		unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
		ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 2 - 1 E:14
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 1877, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, -60.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(0.f), float32(-60.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 2 - 2 E:15
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 2200, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(2076.f, -60.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2076.f), float32(-60.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 1 E:16
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 180, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(-43.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(-43.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 2 E:17
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 131, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(150.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(150.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 3 E:18
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 168, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(319.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(319.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 4 E:19
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 478, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(671.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(671.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 5 E:20
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 55, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(1032.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1032.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 5 E:21
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 75, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(1173.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1173.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 6 E:22
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 132.5, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(1388.5f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1388.5f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 7 E:23
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 75, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(1539.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1539.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 8 E:24
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 244.5, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(1764.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1764.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 9 E:25
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 281, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(2055.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2055.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 10 E:26
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 56, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(2280.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2280.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 11 E:27
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 113, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(2430.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2430.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 3 - 12 E:28
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 94, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(2552.f, -51.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2552.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Spring Hill 1 - small E:29
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(-47.f), float32(-38.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Spring Hill 2 - big E:30
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28.5, 36.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(14.f), float32(-37.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}
		
		//Level Spring Hill 3 - small E:31
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(197.f), float32(-38.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Spring Hill 4 - big E:32
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28.5, 36.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(249.f), float32(-37.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Spring Hill 3 - small E:33
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(385.f), float32(-38.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Spring Hill 4 - big E:34
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28.5, 36.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(455.f), float32(-36.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Level 4 - 1 E:35
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 10);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(520.5f), float32(-42.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - 2 E:36
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 347, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(736.5f), float32(-41.6f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 5 - 1 E:37
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 103, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(800.f, -41.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(643.f), float32(-32.2f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 6,7 - 1  E:38
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 84.5, 16);
			ECS::GetComponent<Transform>(entity).SetPosition(800.f, -41.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(643.f), float32(-17.4f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 8 - 1  E:39
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 38, 20);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(657.f), float32(-0.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 9 - 1 E:40
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28.5, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(661.5f), float32(15.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 1 long E:41
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18.9, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(675.6f), float32(42.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 2 long E:42
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 27.9, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(746.f), float32(-4.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}


		//Level Summer Platform 3 long E:43
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18.9, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(797.4f), float32(-14.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 4 long E:44
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18.9, 8.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(816.4f), float32(14.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 1 tall E:45
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(717.6f), float32(28.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 2 tall E:46
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(774.5f), float32(28.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 3 tall E:47
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1083.5f), float32(0.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 4 tall E:48
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1121.5f), float32(19.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 5 tall E:49
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1252.5f), float32(19.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}
		//Level Summer Platform 6 tall E:50
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1308.9f), float32(0.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 1 single block E:51
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(943.f), float32(14.8f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 2 single block E:52
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(952.5f), float32(-14.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 3 single block E:53
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 8, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1215.f), float32(5.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Winter Platform 1 single block E:54
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2322.f), float32(-23.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Winter Platform 2 single block E:55
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2350.f), float32(-5.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 5 long E:56 
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 8.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(994.5f), float32(5.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 6 long E:57
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 8.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1036.5f), float32(24.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 7 long E:58
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 8.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1055.5f), float32(-22.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level Summer Platform 8 long E:59
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 8.5);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1285.5f), float32(-22.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}


		//Level 5 - 2 E:60
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 84, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(800.f, -41.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(868.f), float32(-32.2f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 6 - 2 E:61
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 75, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(872.5f), float32(-22.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 7,8 - 2 E:62
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 55, 17);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(882.f), float32(-8.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 9 - 1 E:63
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 36, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(891.f), float32(5.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}


		//Level 10 - 1 E:64
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(901.f), float32(15.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - 1 - Summer E:65
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 47, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1168.f), float32(-42.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}


		//Level 5,6 - 2 - Summer E:66
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 37, 18);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1173.f), float32(-28.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 7 - Summer E:67
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 18);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1177.8f), float32(-9.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 8 - Summer E:68
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1173.f), float32(5.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 6-8 - Summer E:69
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 76, 30);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1370.f), float32(-33.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 9 - Summer E:70
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 47, 20);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1365.f), float32(-10.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 1- Fall E:71
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1525.8f), float32(-26.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 2- Fall E:72
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1542.4f), float32(-1.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 3- Fall E:73
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1525.8f), float32(21.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 4- Fall E:74
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1542.4f), float32(46.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 5- Fall E:75
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1525.8f), float32(68.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 1- Fall E:76
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename,66, 28);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1600.f), float32(23.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 2- Fall E:77
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1665.5f), float32(0.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 3- Fall E:78
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1722.f), float32(9.4f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 4- Fall E:79
		{
			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 47, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1797.f), float32(28.2f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 5- Fall E:80
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 29, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2031.5f), float32(28.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 6- Fall E:81
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2097.f), float32(37.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - Fall E:82
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 20, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1735.3f), float32(-41.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - Fall E:83
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1788.f), float32(-41.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - Fall E:84
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2031.f), float32(-41.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Hill 1 - Fall E:85
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 46);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1862.7f), float32(-41.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Hill 1 - Fall E:86
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 62);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1937.7f), float32(-40.2f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 6- Fall E:87
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1985.8f), float32(-26.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 7- Fall E:88
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2002.f), float32(-0.8f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 8- Fall E:89
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1985.8f), float32(21.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 9- Fall E:90
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2002.f), float32(46.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 10- Fall E:91
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 7, 2);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(1985.8f), float32(68.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - Fall/winter E:92
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 112, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2120.4f), float32(-41.5f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level 4 - Fall/winter E:93
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 48, 60);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2153.1f), float32(-30.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level winter single block E:94
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 8);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2172.f), float32(5.4f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Piller 1 - Winter E:95
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 27);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2284.8f), float32(-32.1f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Piller 2 - Winter E:96
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 27);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2472.4f), float32(-32.1f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Piller 3 - Winter E:97
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 45);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2519.4f), float32(-31.8f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Bunny Leaf Hill - Winter E:98
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 9, 80);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2388.0f), float32(-21.1f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Bunny Leaf Hill - Winter E:99
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 43, 40);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2409.f), float32(-29.1f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 1 - Winter E:100
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2571.f), float32(-20.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 2 - Winter E:101
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2542.f), float32(8.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 3 - Winter E:102
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2571.f), float32(36.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 4 - Winter E:103
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2988.f), float32(-20.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 5 - Winter E:104
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3021.f), float32(-1.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 6 - Winter E:105
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2988.f), float32(27.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Tree B 7 - Winter E:106
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 3);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3021.f), float32(55.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Float Island p1 - Winter E:107
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 56, 65);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2627.1f), float32(14.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Float Island p2- Winter E:108
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 75, 37);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2693.1f), float32(28.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Float Island p3- Winter E:19
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2631.9f), float32(52.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Float Island p4- Winter E:110
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2698.1f), float32(52.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 1 - Winter E:111
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2758.6f), float32(18.6f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 2 - Winter E:112
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2810.f), float32(14.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 3 - Winter E:113
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2857.f), float32(33.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 4 - Winter E:114
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2923.f), float32(-4.6f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 5 - Winter E:115
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3050.f), float32(14.3f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 6 - Winter E:116
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3078.f), float32(32.8f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Platform 7 - Winter E:117
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 19, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3115.f), float32(51.7f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:118
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 28, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2857.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:119
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 131, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2740.f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:120
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 122, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(2979.5f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:121
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 60, 9);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3145.5f), float32(-51.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:122
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 40, 19);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3154.f), float32(-37.8f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:123
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 30, 22);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3158.5f), float32(-20.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}

		//Level - Winter E:124
		{

			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "B.png";

			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 27, 30);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_staticBody;
			tempDef.position.Set(float32(3166.5f), float32(4.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");

		}


		//Ice P-Bodys
		{
			//Ice 1 E:125
			{
				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);
				ECS::AttachComponent<Ice>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 55, 8);
				ECS::GetComponent<Transform>(entity).SetPosition(2430.f, -51.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(2223.5f), float32(-51.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::IceBit() | EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Ice 2 E:126
			{
				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);
				ECS::AttachComponent<Ice>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 54, 9);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(2665.f), float32(52.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::IceBit() | EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");
			}

			//Ice 3 E:127
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);
				ECS::AttachComponent<Ice>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 36, 9);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(2824.f), float32(-51.f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::IceBit() | EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 1 - Spring E:128
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(323.5f), float32(-5.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 2 - Spring E:129
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-75.f), float32(33.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 3 - Spring E:130
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-94.f), float32(14.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 4 - Spring E:131
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(-38.f), float32(52.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 5 - Spring E:132
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(9.f), float32(42.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 6 - Spring E:133
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(47.f), float32(14.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

			//Tree B 7 - Spring E:134
			{

				auto entity = ECS::CreateEntity();

				ECS::AttachComponent<Sprite>(entity);
				ECS::AttachComponent<PhysicsBody>(entity);
				ECS::AttachComponent<Transform>(entity);
				ECS::AttachComponent<Ground>(entity);

				std::string filename = "B.png";

				ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 10, 3);
				ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 5.f);

				auto& tempSpr = ECS::GetComponent<Sprite>(entity);
				auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

				float shrinkX = 0.f;
				float shrinkY = 0.f;

				b2Body* tempBody;
				b2BodyDef tempDef;
				tempDef.type = b2_staticBody;
				tempDef.position.Set(float32(295.5f), float32(-14.0f));

				tempBody = m_physicsWorld->CreateBody(&tempDef);

				tempBody->SetUserData((void*)entity);
				tempBody->SetEntityNumber(entity);
				tempBody->SetEntityType(1);

				tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
					vec2(0.f, 0.f), false);

				unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
				ECS::SetUpIdentifier(entity, bitHolder, "ground");

			}

		}
	}
	
	
		//Big rock E:135
		{
			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "Tall_rock.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 20, 30);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 14.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);
	
			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(20.f), float32(10.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);
	
			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");
		}
			
		
		//Big rock E:136
		{
			auto entity = ECS::CreateEntity();

			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<Ground>(entity);

			std::string filename = "Tall_rock.png";
			ECS::GetComponent<Sprite>(entity).LoadSprite(filename, 18, 24);
			ECS::GetComponent<Transform>(entity).SetPosition(0.f, 0.f, 14.f);

			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(2080.f), float32(0.0f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			unsigned int bitHolder = EntityIdentifier::GroundBit() | EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::PhysicsBit();
			ECS::SetUpIdentifier(entity, bitHolder, "ground");
		}

		//Setup new Entity Sign Walking  E:137
		{
			//Our attack animation file
			auto Move = File::LoadJSON("dashSign.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);

			//Sets up components
			std::string fileName = "dashSign.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["DashSign"]); //	0
			animController.SetActiveAnim(0);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 128 / 3, 128 / 3, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 128.f), vec2(128.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-20.f, -26.f, 11.f));

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "SignWalking");

		}
	

		//Setup new Entity Bunny E:138
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(-75.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(0.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);
			
			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
			vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");
			
		}

		//Setup new Entity Bunny E:139
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(270.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(360.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:140
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(270.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(1573.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:141
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1656.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(1650.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:142
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1961.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(1961.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:143
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1961.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(2183.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:144
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1961.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(2530.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:145
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1961.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(2681.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}

		//Setup new Entity Bunny E:145
		{
			//Bunny animation file
			auto Move = File::LoadJSON("Bunny.json");

			//Creates entity
			auto entity = ECS::CreateEntity();

			//Add components
			ECS::AttachComponent<Sprite>(entity);
			ECS::AttachComponent<Transform>(entity);
			ECS::AttachComponent<AnimationController>(entity);
			ECS::AttachComponent<PhysicsBody>(entity);
			//ECS::AttachComponent<Ground>(entity);
			//ECS::AttachComponent<Water>(entity);

			//Sets up components
			std::string fileName = "bunny.png";
			auto& animController = ECS::GetComponent<AnimationController>(entity);
			animController.InitUVs(fileName);

			animController.AddAnimation(Move["BunnyRight"]); //	0
			animController.AddAnimation(Move["BunnyLeft"]); //	1
			animController.SetActiveAnim(1);

			ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 16 / 2.5, 16 / 2.5, true, &animController);
			ECS::GetComponent<Sprite>(entity).SetUVs(vec2(1.f, 16.f), vec2(16.f, 1.f));
			ECS::GetComponent<Transform>(entity).SetPosition(vec3(1961.f, -40.f, 12.f));


			auto& tempSpr = ECS::GetComponent<Sprite>(entity);
			auto& tempPhsBody = ECS::GetComponent<PhysicsBody>(entity);

			float shrinkX = 0.f;
			float shrinkY = 0.f;

			b2Body* tempBody;
			b2BodyDef tempDef;
			tempDef.type = b2_dynamicBody;
			tempDef.position.Set(float32(2930.f), float32(-40.f));

			tempBody = m_physicsWorld->CreateBody(&tempDef);

			tempBody->SetUserData((void*)entity);
			tempBody->SetEntityNumber(entity);
			tempBody->SetEntityType(1);

			tempBody->SetFixedRotation(true);

			tempPhsBody = PhysicsBody(tempBody, float(tempSpr.GetWidth() - shrinkX), float(tempSpr.GetHeight() - shrinkY),
				vec2(0.f, 0.f), false);

			//Sets up the Identifier
			unsigned int bitHolder = EntityIdentifier::SpriteBit() | EntityIdentifier::TransformBit() | EntityIdentifier::AnimationBit();
			ECS::SetUpIdentifier(entity, bitHolder, "Bunny");

		}
	m_isInit = true;


}


	

void ExampleBaseScene::Update()
{

	Aiming(m_mouseGL);

	AdjustScrollOffset();

	updataBunny10();
	updataBunny138();
	updataBunny139();
	updataBunny140();
	updataBunny141();
	updataBunny142();
	updataBunny143();
	updataBunny144();
	updataBunny145();
	updataBunny146();
	//RainbowBackground();

	
}



/*void ExampleBaseScene::RainbowBackground()
{
	//Makes clear color smoothly blend between two colors, going back and forth.
	m_clearColor = Util::Lerp<vec4>(m_clearColor1, m_clearColor2, m_lerpVal);

	if (m_lerpVal >= 1.f)
	{
		vec4 temp = m_clearColor2;

		m_clearColor2 = m_clearColor1;
		m_clearColor1 = temp;

		m_lerpVal = 0.f;
	}

	m_lerpVal += Timer::deltaTime / m_repeatTime;
}
*/

void ExampleBaseScene::AdjustScrollOffset()
{
	float maxSizeX = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().y;
	float maxSizeY = ECS::GetComponent<Camera>(EntityIdentifier::MainCamera()).GetOrthoSize().w;

	float playerHalfSize = ECS::GetComponent<Sprite>(EntityIdentifier::MainPlayer()).GetWidth() / 2.f;

	ECS::GetComponent<HorizontalScroll>(EntityIdentifier::MainCamera()).SetOffset((maxSizeX * BackEnd::GetAspectRatio()) - playerHalfSize);
	ECS::GetComponent<VerticalScroll>(EntityIdentifier::MainCamera()).SetOffset(maxSizeY - playerHalfSize);

}

void ExampleBaseScene::Aiming(vec2 mousePosition)
{
	vec3 playerLoc = ECS::GetComponent<Transform>(EntityIdentifier::MainPlayer()).GetPosition();

	vec2 convertedMouse = mousePosition;
}

void ExampleBaseScene::KeyboardHold()
{

	auto& animController = ECS::GetComponent<AnimationController>(1);
	
	
	
	//run speed changes
	if (ECS::GetComponent<EntityIdentifier>(EntityIdentifier::MainPlayer()).GetIdentifier()& EntityIdentifier::PhysicsBit())
	{
		auto body = ECS::GetComponent<PhysicsBody>(EntityIdentifier::MainPlayer()).GetBody();

		b2Vec2 vel = body->GetLinearVelocity();

			if (Input::GetKey(Key::A))
			{
				vel.x = -25;//upwards - don't change y velocity
				body->SetLinearVelocity(vel);
			}
			if (Input::GetKey(Key::D))
			{
				vel.x = 25;//upwards - don't change y velocity
				body->SetLinearVelocity(vel);
			}
			//dash right E
			if (Input::GetKeyDown(Key::E))
			{
				animController.SetActiveAnim(3);
			}
	}
}

void ExampleBaseScene::MouseMotion(SDL_MouseMotionEvent evnt)
{
	m_mouseGL = Util::ConvertToGL(m_sceneReg, vec2(float(evnt.x), float(evnt.y)));
}

void ExampleBaseScene::MouseClick(SDL_MouseButtonEvent evnt)
{
}

void ExampleBaseScene::updataBunny10()
{
	
	auto body = ECS::GetComponent<PhysicsBody>(10).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(10);
	b2Vec2 vel = body->GetLinearVelocity();
	
	
	if (ECS::GetComponent<Transform>(10).GetPositionX() >= -60) 
	{
		face10 = false;
	}
	else if (ECS::GetComponent<Transform>(10).GetPositionX() <= -114)
	{
		face10 = true;
	}

	if (face10 == true)
	{
		vel.x = 10;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -10;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny138()
{
	
	auto body = ECS::GetComponent<PhysicsBody>(138).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(138);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(138).GetPositionX() >= -3.5)
	{
		face138 = false;
	}
	else if (ECS::GetComponent<Transform>(138).GetPositionX() <= -34)
	{
		face138 = true;
	}

	if (face138 == true)
	{
		vel.x = 10;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -10;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny139()
{

	auto body = ECS::GetComponent<PhysicsBody>(139).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(139);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(139).GetPositionX() >= 368)
	{
		face139 = false;
	}
	else if (ECS::GetComponent<Transform>(139).GetPositionX() <= 268)
	{
		face139 = true;
	}

	if (face139 == true)
	{
		vel.x = 30;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -30;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny140()
{

	auto body = ECS::GetComponent<PhysicsBody>(140).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(140);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(140).GetPositionX() >= 1572)
	{
		face140 = false;
	}
	else if (ECS::GetComponent<Transform>(140).GetPositionX() <= 1507)
	{
		face140 = true;
	}

	if (face140 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny141()
{

	auto body = ECS::GetComponent<PhysicsBody>(141).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(141);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(141).GetPositionX() >= 1721)
	{
		face141 = false;
	}
	else if (ECS::GetComponent<Transform>(141).GetPositionX() <= 1646)
	{
		face141 = true;
	}

	if (face141 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny142()
{
	auto body = ECS::GetComponent<PhysicsBody>(142).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(142);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(142).GetPositionX() >= 2019)
	{
		face142 = false;
	}
	else if (ECS::GetComponent<Transform>(142).GetPositionX() <= 1956)
	{
		face142 = true;
	}

	if (face142 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny143()
{
	auto body = ECS::GetComponent<PhysicsBody>(143).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(143);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(143).GetPositionX() >= 2276)
	{
		face143 = false;
	}
	else if (ECS::GetComponent<Transform>(143).GetPositionX() <= 2182)
	{
		face143 = true;
	}

	if (face143 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny144()
{
	auto body = ECS::GetComponent<PhysicsBody>(144).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(144);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(144).GetPositionX() >= 2590)
	{
		face144 = false;
	}
	else if (ECS::GetComponent<Transform>(144).GetPositionX() <= 2532)
	{
		face144 = true;
	}

	if (face144 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny145()
{
	auto body = ECS::GetComponent<PhysicsBody>(145).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(145);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(145).GetPositionX() >= 2862)
	{
		face145 = false;
	}
	else if (ECS::GetComponent<Transform>(145).GetPositionX() <= 2678)
	{
		face145 = true;
	}

	if (face145 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}
}

void ExampleBaseScene::updataBunny146()
{
	auto body = ECS::GetComponent<PhysicsBody>(146).GetBody();
	auto& animController = ECS::GetComponent<AnimationController>(146);
	b2Vec2 vel = body->GetLinearVelocity();


	if (ECS::GetComponent<Transform>(146).GetPositionX() >= 3032)
	{
		face146 = false;
	}
	else if (ECS::GetComponent<Transform>(146).GetPositionX() <= 2922)
	{
		face146 = true;
	}

	if (face146 == true)
	{
		vel.x = 20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(0);
	}
	else {
		vel.x = -20;//upwards - don't change y velocity
		body->SetLinearVelocity(vel);
		animController.SetActiveAnim(1);

	}

}


