#include "pch.hpp"

bool entity_s::isValid() const noexcept
{
	if (!cent || !info)
		return false;

	if (!info->infoValid || !info->nextValid)
		return false;

	return true;
}

bool entity_s::isEnemy() const noexcept
{

	const bool teammate = cinfo[clientNum].team == cinfo[cgs->clientNum].team;
	const bool isFFA = strcmp("dm", cgs_s->gametype) == 0;

	return !teammate || isFFA;
}

int entity_s::getHealth() const noexcept
{
	return info->health;
}

bool entity_s::isAlive() const noexcept
{

	return cent->nextValid && cent->nextState.eType == ET_PLAYER;
}
bool entity_s::isMe() const noexcept
{
	return clientNum == cgs->clientNum;
}

fvec3 entity_s::getAngles() const noexcept
{
	return cent->pose.angles;

}
fvec3 entity_s::getOrigin() const noexcept
{
	return cent->pose.origin;

}
char* entity_s::getName() const noexcept
{
	if (is_cod4x() == false)
		return info->name;

	DWORD offs = is_cod4x() + 0x443d200;
	offs += (clientNum * 46);
	return (char*)offs;
}
int entity_s::getWeapon() const noexcept
{
	return cent->nextState.weapon;
}