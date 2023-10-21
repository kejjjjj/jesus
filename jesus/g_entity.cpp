#include "pch.hpp"

std::optional<fvec3> AimTarget_GetTagPos(centity_s* entity, int16_t tagName)
{
	if (entity->nextState.eType != ET_PLAYER)
		return false;

	static vec3_t out{1,1,1};

	static const DWORD fn = 0x4024B0;
	int returnval(0);
	__asm {
		lea edx, out;
		push edx;
		mov eax, 0;
		mov ecx, entity;
		movzx esi, tagName;
		call fn;
		add esp, 0x4;
		mov returnval, eax;
	}

	if (!returnval)
		return std::nullopt;

	return out;
}

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

std::optional<fvec3> entity_s::GetTagPosition(const int16_t tag) const noexcept
{

	return AimTarget_GetTagPos(this->cent, tag);

}

std::optional<killable_entity> entity_s::CanBeKilled() const noexcept
{
	const char* tags[] = { "j_head", "j_spineupper", "j_spinelower", "j_shoulder_ri", "j_shoulder_le", "j_knee_ri", "j_knee_le", "j_ankle_ri", "j_ankle_le"};

	fvec3 bone;
	fvec3 self = clients->cgameOrigin;
	self.z += cgs->predictedPlayerState.viewHeightCurrent;
	for (auto tag : tags) {
		if (auto pos = GetTagPosition(SL_GetStringOfSize(tag)))
			bone = pos.value();
		else
			continue;

		BulletFireParams fp{};
		BulletTraceResults a{};
		G_CalculateBulletPenetration(&fp, BG_WeaponNames[cgs->predictedPlayerState.weapon], &a, self, bone, getID(), cgs->clientNum);

		const bool valid_ent = fvec3(a.hitPos) != 0.f;

		if (!valid_ent)
			continue;

		//int dmg = BG_BulletDamage(BG_WeaponNames[cgs->predictedPlayerState.weapon], &fp, &a);

		const float minDistance = self.dist(bone);
		const float distanceTravelled = self.dist(a.hitPos);

		if (distanceTravelled >= minDistance) {
			return killable_entity{ .clientNum = getID(), .tagName = (char*)tag, .angles2target = (bone - self).toangles(), .bone = bone};
		}

	}
	
	return std::nullopt;

}


void entities_s::update_all(int newClients) noexcept
{
	if (numClients == newClients)
		return;

	entities.clear();

	for (int i = 0; i < newClients; i++) {
		entities.push_back(i);
	}

	numClients = newClients;

}

entity_s* entities_s::find(int idx) noexcept
{
	if (idx < 0 || idx >= numClients)
		return 0;

	return &entities[idx];
}