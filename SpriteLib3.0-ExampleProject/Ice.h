#pragma once
class Ice
{
public:
	Ice();
	Ice(float health);

	float GetHealth() const;
	void SetHealth(float health);

private:
	float m_health = 1.f;
};