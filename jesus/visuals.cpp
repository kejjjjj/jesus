#include "pch.hpp"

void quick_test(entity_s* target)
{
	BulletFireParams fp{};
	BulletTraceResults a{};

	//if ((GetAsyncKeyState(VK_NUMPAD9) & 1) == false) {
	//	return;
	//}


	fvec3 head;

	if (auto pos = target->GetTagPosition(SL_GetStringOfSize("j_head")))
		head = pos.value();
	else
		return;

	//if (auto pos = entity_s(cgs->clientNum).GetTagPosition(SL_GetStringOfSize("j_shoulder_ri")))
	//	mee = pos.value();
	//else
	//	return;

	fvec3 mee = clients->cgameOrigin;
	mee.z += cgs->predictedPlayerState.viewHeightCurrent;

	G_CalculateBulletPenetration(&fp, BG_WeaponNames[cgs->predictedPlayerState.weapon], &a, mee, head, target->getID(), cgs->clientNum);

	bool valid_ent = fvec3(a.hitPos) != 0.f;


	//VectorCopy(fp.end, a.hitPos);
	int dmg = BG_BulletDamage(BG_WeaponNames[cgs->predictedPlayerState.weapon], &fp, &a);
	if (dmg < 0)
		dmg = 0;
	
	float minDistance = mee.dist(head);
	float distanceTravelled = mee.dist(a.hitPos);


	//if (auto xy_o = WorldToScreen(a.hitPos)) {
	//	fvec2 xy = xy_o.value();
	//	float a = 10, b = 10;
	//	//CG_AdjustFrom640(xy.x, xy.y, a, b);

	//	//CG_DrawRotatedPic(0, 0, CG_GetScreenPlacement(), xy.x, xy.y, 40, 40, 0, vec4_t{ 1,1,1,1 }, "compassping_friendly_mp");

	//	R_AddCmdDrawTextWithEffects((char*)std::format("{}", target->getOrigin().z).c_str(), "fonts/bigDevFont", xy.x, xy.y,
	//		1.2, 1.2f, 0.f, vec4_t{ 1,1,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);

	//}



	//float isBehindEnemy = (fvec3(fp.end) - head).toangles().y;
	//float targetAngle = (fvec3(fp.start) - head).toangles().y;


	//std::cout << fvec3(fp.end) << '\n';

	////if (PointWithinLine(fp.start, fp.end, head, 10.f)) {
	////	R_AddCmdDrawTextWithEffects((char*)"can kill", "fonts/bigDevFont", 100, 200,
	////		1.2, 1.2f, 0.f, vec4_t{ 1,1,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);
	////}
	////else {
	////	R_AddCmdDrawTextWithEffects((char*)"can't kill", "fonts/bigDevFont", 100, 200,
	////		1.2, 1.2f, 0.f, vec4_t{ 1,0,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);
	////}

	if (valid_ent && distanceTravelled >= minDistance) {
		R_AddCmdDrawTextWithEffects((char*)std::format("can kill ({})", fp.damageMultiplier).c_str(), "fonts/bigDevFont", 100, 200,
			1.2, 1.2f, 0.f, vec4_t{ 1,1,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);

		CG_SetPlayerAngles((head-mee).toangles());
	}
	else {
		R_AddCmdDrawTextWithEffects((char*)"can't kill", "fonts/bigDevFont", 100, 200,
			1.2, 1.2f, 0.f, vec4_t{ 1,0,0,1 }, 1, vec4_t{ 1,0,0,1 }, 0, 0, 0, 0, 0, 0);
	}

}

void R_OwnerDraw(entity_s* target)
{
	if (!target->isValid() || target->isAlive() == false || target->isMe())
		return;

	R_DrawPlayerName(target);
	R_DrawPlayerWeapon(target);
	R_DrawCircularCompass(target);
	R_DrawKillable(target);
	//quick_test(target);

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

void R_DrawKillable(entity_s* target)
{
	if (!target->isEnemy() || Dvar_FindMalleableVar("hack_killableEnemy")->current.enabled == false)
		return;

	std::optional<killable_entity> killable = target->CanBeKilled();

	if (!killable)
		return;

	std::optional<fvec2> xy_opt = WorldToScreen(killable.value().bone);

	if (!xy_opt) {
		return;
	}

	//std::cout << xy_opt.value() << '\n';


	fvec2 xy = xy_opt.value();
	float a = 10, b = 10;

	R_AddCmdDrawTextWithEffects((char*)"w", "fonts/bigDevFont", xy.x, xy.y, 1.2f, 1.2f, 0.f, target->isEnemy() ? vec4_t{1, 0, 0, 1} : vec4_t{1,1,0,1}, 1, vec4_t{1,0,0,1}, 0, 0, 0, 0, 0, 0);


	if (Dvar_FindMalleableVar("hack_silentAim")->current.enabled) {
		CL_GetUserCmd(clients->cmdNumber)->buttons |= cmdEnums::fire;
		CL_SetSilentAngles(killable.value().angles2target);
	}


}