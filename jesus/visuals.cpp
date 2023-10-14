#include "pch.hpp"

void R_OwnerDraw(entity_s* target)
{
	if (!target->isValid() || target->isAlive() == false || target->isMe())
		return;

	R_DrawPlayerName(target);
	R_DrawPlayerWeapon(target);
	R_DrawCircularCompass(target);

	//CG_DrawRotatedPic(CG_GetScreenPlacement(cgs->clientNum), 400, 400, 50, 50, 0.f, vec4_t{ 1,1,1,1 }, R_RegisterMaterial("compassping_friendly_mp"));
	//CG_DrawRotatedPic(0, 0, CG_GetScreenPlacement(), 400, 400, 40, 40, 0.f, vec4_t{ 1,1,1,1 }, "compassping_friendly_mp");
}

void R_DrawPlayerName(entity_s* target)
{
	if (Dvar_FindMalleableVar("hack_playerNames")->current.enabled == false)
		return;

	char* name = target->getName();
	fvec2 xy;
	std::optional<fvec2> xy_opt = WorldToScreen(target->getOrigin());

	if (!xy_opt) {
		return;
	}

	xy = xy_opt.value();

	float scale = ScaleFontByDistance(fvec3(clients->cgameOrigin).dist(target->getOrigin()));

	//R_DrawText(name, "fonts/bigDevFont", xy.x, xy.y, scale, scale, 0.f, target->isEnemy() ? vec4_t{1, 0, 0, 1} : vec4_t{ 0,1,0,1 }, 1);
	R_AddCmdDrawTextWithEffects(name, "fonts/bigDevFont", xy.x, xy.y, scale, scale, 0.f, target->isEnemy() ? vec4_t{1, 0, 0, 1} : vec4_t{ 1,1,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);
}
void R_DrawPlayerWeapon(entity_s* target)
{
	if (Dvar_FindMalleableVar("hack_playerWeapons")->current.enabled == false)
		return;

	std::optional<fvec2> xy_opt = WorldToScreen(target->getOrigin());

	if (!xy_opt) {
		return;
	}

	fvec2 xy = xy_opt.value();

	float scale = ScaleFontByDistance(fvec3(clients->cgameOrigin).dist(target->getOrigin())) * 30.f;

	R_AddCmdDrawStretchPic(BG_WeaponNames[target->getWeapon()]->ammoCounterIcon, xy.x, xy.y, scale, scale, 0.f, 0.f, 1.f, 1.f, vec4_t{ 1,1,1,1 });

}

void R_DrawCircularCompass(entity_s* target)
{
	if (Dvar_FindMalleableVar("hack_circularCompass")->current.enabled == false)
		return;

	if (!target->isEnemy())
		return;

	float angle = (target->getOrigin() - fvec3(clients->cgameOrigin)).toangles().y;

	float relative_angle = AngleDelta(clients->cgameViewangles[YAW], angle);

	fvec2 center = { 320, 240 };

	center.x += std::sin(DEG2RAD(relative_angle)) * 150.f;
	center.y -= std::cos(DEG2RAD(relative_angle)) * 150.f;

	CG_DrawRotatedPic(0, 0, CG_GetScreenPlacement(), center.x, center.y, 40, 40, relative_angle, vec4_t{ 1,1,1,1 }, "compassping_friendly_mp");

}