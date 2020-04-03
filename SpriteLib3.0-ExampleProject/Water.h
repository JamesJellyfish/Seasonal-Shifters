#pragma once
class Water
{
public:
	Water();
	Water(float health);

	float GetHealth() const;
	void SetHealth(float health);

private:
	float m_health = 1.f;
};