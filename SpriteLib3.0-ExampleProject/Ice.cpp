#include "Ice.h"

Ice::Ice()
{
}

Ice::Ice(float health)
{
	SetHealth(health);
}

float Ice::GetHealth() const
{
	return m_health;
}

void Ice::SetHealth(float health)
{
	m_health = health;
}
