#include "Water.h"

Water::Water()
{
}

Water::Water(float health)
{
	SetHealth(health);
}

float Water::GetHealth() const
{
	return m_health;
}

void Water::SetHealth(float health)
{
	m_health = health;
}
