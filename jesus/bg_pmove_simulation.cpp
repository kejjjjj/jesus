#include "pch.hpp"

simulation_results PmoveSingleSimulation(pmove_t* pm, pml_t* pml, prediction_controller* controller)
{
	playerState_s* ps = pm->ps;
	static dvar_s* mantle_enable = Dvar_FindMalleableVar("mantle_enable");
	simulation_results results;

	pm->cmd.rightmove = controller->rightmove;
	pm->cmd.forwardmove = controller->forwardmove;
	pm->cmd.buttons = controller->buttons;
	
	hook::write_addr(0x537D10, "\xC3", 1);


	if ((pm->ps->pm_flags & PMF_MELEE_CHARGE) != 0)
		pm->cmd.forwardmove = 127;

	if ((ps->pm_flags & PMF_FROZEN) != 0)
	{
		pm->cmd.buttons &= 0x1300u;
	}

	if ((ps->pm_flags & PMF_RESPAWNED) != 0)
	{
		pm->cmd.buttons &= 0x1301u;
	}

	if (ps->weaponstate == WEAPON_FIRING)
	{
		int weaponIdx = ps->weapon;
		if (weaponIdx)
		{
			if (BG_WeaponNames[weaponIdx]->freezeMovementWhenFiring)
			{
				pm->cmd.buttons &= 0xFFFFFB3F;
				pm->cmd.forwardmove = 0;
				pm->cmd.rightmove = 0;
				ps->velocity[0] = 0.0;
				ps->velocity[1] = 0.0;
				ps->velocity[2] = 0.0;
			}
		}
	}

	ps->pm_flags &= 0xFFFFEFFF;
	if (ps->pm_type >= PM_DEAD)
		pm->tracemask &= 0xFDFF3FFF;

	if ((ps->pm_flags & PMF_PRONE) == 0 || BG_WeaponNames[ps->weapon]->overlayReticle && ps->fWeaponPosFrac > 0.0) {
		ps->pm_flags &= 0xFFFFFDFF;
	}

	//if (PM_InteruptWeaponWithProneMove_(ps))
	//{
	//	ps->pm_flags &= 0xFFFFFDFF;
	//	PM_ExitAimDownSight_(ps);
	//}

	if ((ps->pm_flags & PMF_SIGHT_AIMING) != 0 && ps->viewHeightTarget == 11)
	{
		if (BG_WeaponNames[ps->weapon]->overlayReticle)
		{
			if (0.0f >= ps->fWeaponPosFrac)
			{
				pm->cmd.forwardmove = 0;
				pm->cmd.rightmove = 0;
			}
		}
		else
		{
			pm->cmd.forwardmove = 0;
			pm->cmd.rightmove = 0;
		}
	}
	ps->eFlags &= 0xFFFFFFBF;

	if ((ps->pm_flags & PMF_RESPAWNED) == 0)
	{
		if ((!ps->weaponstate || ps->weaponstate == 5) && ps->ammoclip[BG_WeaponNames[ps->weapon]->iClipIndex + 4] && (pm->cmd.buttons & 1) != 0)
			ps->eFlags |= EF_FIRING;
	}
	if (ps->pm_type < PM_DEAD && (pm->cmd.buttons & 0x101) == 0)
		ps->pm_flags &= 0xFFFFFBFF;

	//int serverTime = pm->cmd.serverTime;
	//int deltaTime = serverTime - ps->commandTime;

	pml->previous_origin[0] = ps->origin[0];
	pml->previous_origin[1] = ps->origin[1];
	pml->previous_origin[2] = ps->origin[2];
	pml->previous_velocity[0] = ps->velocity[0];
	pml->previous_velocity[1] = ps->velocity[1];
	pml->previous_velocity[2] = ps->velocity[2];


	pml->msec = 1000 / ((controller->FPS == 0) ? 1 : controller->FPS); //simulate fps

	//if (controller->autofps)
	//	pml->msec = 1000 / T::Movement::T_GetIdealFPS(pm);

	pml->frametime = (float)pml->msec / 1000.f;
	pm->cmd.serverTime += pml->msec;
	pm->ps->commandTime += pml->msec;
	pm->cmd.buttons = controller->buttons;


	fvec3 deltas;

	switch (controller->turntype.get()->get_type()) {
	case prediction_angle_enumerator::FIXED_TURN:
		deltas = dynamic_cast<prediction_viewangle_fixed_turn*>(controller->turntype.get())->get_deltas(pm, pml);
		break;
	case prediction_angle_enumerator::STRAFEBOT:
		deltas = dynamic_cast<prediction_viewangle_strafebot*>(controller->turntype.get())->get_deltas(pm, pml);
		break;

	}

	pm->ps->viewangles[PITCH] += deltas.x;
	pm->ps->viewangles[YAW] += deltas.y;
	pm->ps->viewangles[ROLL] += deltas.z;

	pm->cmd.angles[PITCH] += ANGLE2SHORT(deltas.x);
	pm->cmd.angles[YAW] += ANGLE2SHORT(deltas.y);
	pm->cmd.angles[ROLL] += ANGLE2SHORT(deltas.z);

	PM_AdjustAimSpreadScale_(pm, pml);
	_PM_UpdateViewAngles(ps, pml->msec, &pm->cmd, pm->handler);

	AngleVectors(pm->ps->viewangles, pml->forward, pml->right, pml->up); //set viewangles

	char fwd = pm->cmd.forwardmove;
	if (fwd >= 0)
	{
		if (fwd > 0 || pm->cmd.rightmove)
			ps->pm_flags &= 0xFFFFFFDF;
	}
	else
	{
		ps->pm_flags |= PMF_BACKWARDS_RUN;
	}

	if ((ps->pm_flags & PMF_MANTLE) == 0) {
		//PM_UpdateAimDownSightFlag_(pm, pml);
		PM_UpdateSprint_(pm);
		PM_UpdatePlayerWalkingFlag_(pm);
		PM_CheckDuck_(pm, pml);
		PM_GroundTrace_(pm, pml);
	}

	//Mantle_Check(pm, pml);

	if ((ps->pm_flags & PMF_MANTLE) != 0) {

		if ((ps->pm_flags & PMF_LADDER) != 0)
			ps->pm_flags = ps->pm_flags & 0xFFFFFFF7 | PMF_LADDER_FALL;
		ps->groundEntityNum = 1023;
		pml->groundPlane = 0;
		pml->walking = 0;

		//PM_UpdateAimDownSightFlag_(pm, pml);
		PM_UpdateSprint_(pm);
		PM_UpdatePlayerWalkingFlag_(pm);
		PM_CheckDuck_(pm, pml);
		if (mantle_enable->current.enabled)
			Mantle_Move(ps, pm, pml);

		PM_Weapon(pm, pml);

	}
	else {



		PM_UpdatePronePitch(pml, pm);
		PM_DropTimers(ps, pml);
		//if ( ps->pm_type >= PM_DEAD && pml->walking )
		//	PM_DeadMove(ps);
		PM_CheckLadderMove_(pm, pml);

		if ((ps->pm_flags & PMF_LADDER) != 0)
		{
			PM_LadderMove_(pm, pml);
		}
		else if (pml->walking) {
			engine_call<void __cdecl>(false, 0x40F7A0, pm, pml); //walkmove
		}
		else {
			engine_call<void __cdecl>(false, 0x40F680, pm, pml); //airmove
		}
		PM_GroundTrace_(pm, pml); //call groundtrace after 
		PM_Footsteps_(pm, pml);
		PM_Weapon(pm, pml);
		//PM_FoliageSnd_(pm);
		PM_OverBounce(pm, pml);
		Sys_SnapVector(pm->ps->velocity); //Sys_SnapVector | not called in singleplayer
	}
	
	if (pm->cmd.forwardmove > 0)
		pm->cmd.forwardmove = 127;
	else if (pm->cmd.forwardmove < 0)
		pm->cmd.forwardmove = -127;

	if (pm->cmd.rightmove > 0)
		pm->cmd.rightmove = 127;
	else if (pm->cmd.rightmove < 0)
		pm->cmd.rightmove = -127;

	results.cmd_angles = pm->cmd.angles;
	results.weapon = pm->cmd.weapon;
	results.offhand = pm->cmd.offHandIndex;
	results.viewangles = pm->ps->viewangles;
	results.origin = pm->ps->origin;
	results.velocity = pm->ps->velocity;
	results.mins = pm->mins;
	results.maxs = pm->maxs;
	results.buttons = pm->cmd.buttons;
	results.forwardmove = pm->cmd.forwardmove;
	results.rightmove = pm->cmd.rightmove;
	results.FPS = 1000 / pml->msec;
	memcpy(&pm->oldcmd, &pm->cmd, sizeof(pm->oldcmd));
	pm->cmd.buttons = 0;

	hook::write_addr(0x537D10, "\x81", 1);


	return std::move(results);
}