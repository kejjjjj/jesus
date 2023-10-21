#pragma once

#include "pch.hpp"

std::optional<fvec3> AimTarget_GetTagPos(centity_s* entity, int16_t tagName);

struct killable_entity
{
	int clientNum;
	char* tagName;
	fvec3 angles2target;
	fvec3 bone;
};

struct entity_s
{
	entity_s() = delete;

	entity_s(int _pIndex) :
		clientNum(_pIndex),
		cent(&centity[_pIndex]),
		info(&cinfo[_pIndex]) {}
	entity_s(centity_s* _cent) : 
		clientNum(cent->nextState.clientNum), 
		cent(_cent), 
		info(&cinfo[clientNum]) {}

	bool isValid() const noexcept;
	bool isEnemy() const noexcept;
	int getHealth() const noexcept;
	bool isAlive() const noexcept;
	bool isMe() const noexcept;
	fvec3 getAngles() const noexcept;
	fvec3 getOrigin() const noexcept;
	char* getName() const noexcept;
	int getWeapon() const noexcept;
	std::optional<fvec3> GetTagPosition(const int16_t tag) const noexcept;
	int getID() const noexcept { return clientNum; }
	std::optional<killable_entity> CanBeKilled() const noexcept;

private:
	int clientNum;
	centity_s* cent;
	clientInfo_t* info;
};

struct entities_s
{
	static entities_s& get() { static entities_s e; return e; }

	void update_all(int newClients) noexcept;
	entity_s* find(int idx) noexcept;
private:

	int numClients = 0;
	std::vector<entity_s> entities;
};