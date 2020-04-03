#pragma once

#include "Scene.h"
#include "MyContactListener.h"

class ExampleBaseScene : public Scene
{
public:
	ExampleBaseScene(std::string name);

	void InitScene(float windowWidth, float windowHeight) override;

	void Update() override;

	//Effects & camera stuff
	void RainbowBackground();
	void AdjustScrollOffset();

	//Controls
	void Aiming(vec2 mousePosition);

	//Input overrides
	void KeyboardHold() override;

	void MouseMotion(SDL_MouseMotionEvent evnt) override;
	void MouseClick(SDL_MouseButtonEvent evnt) override;

	void updataBunny10();
	void updataBunny138();
	void updataBunny139();
	void updataBunny140();
	void updataBunny141();
	void updataBunny142();
	void updataBunny143();
	void updataBunny144();
	void updataBunny145();
	void updataBunny146();
protected:
	//Mouse position in world
	vec2 m_mouseGL = vec2(0.f, 0.f);

	//Time needed for colors to lerp to and fro
	float m_repeatTime = 5.f;
	float m_lerpVal = 0.f;

	//Colors to be lerped between
	vec4 m_clearColor1 = vec4(0.153f, 0.992f, 0.961f, 1.f);
	vec4 m_clearColor2 = vec4(0.969f, 0.396f, 0.722f, 1.f);

	bool m_isInit = false;

	bool m_face;

	bool face10 = false;
	bool face138 = false;
	bool face139 = false;
	bool face140 = false;
	bool face141 = false;
	bool face142 = false;
	bool face143 = false;
	bool face144 = false;
	bool face145 = false;
	bool face146 = false;
};
