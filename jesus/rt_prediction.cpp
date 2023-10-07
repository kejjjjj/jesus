#include "pch.hpp"

std::optional<std::list<playback_cmd>> RealTimePrediction(playerState_s* ps, usercmd_s* cmd)
{
	static float old_yaw = ps->viewangles[YAW];

	if (ps->groundEntityNum == 1022 || ps->groundEntityNum == 1023 && (ps->pm_flags & PMF_JUMPING) == 0)
		return std::nullopt;

	//auto ps_copy = *ps;

	pmove_t pm = PM_Create(ps, cmd, CL_GetUserCmd(clients->cmdNumber - 1));
	pml_t pml = PML_Create(&pm, 333);

	prediction_controller c;
	c.buttons = pm.cmd.buttons;
	c.forwardmove = pm.cmd.forwardmove;
	c.rightmove = pm.cmd.rightmove;
	c.numInputs = c.FPS;
	float yaw_delta = -(AngleDelta(ps->viewangles[YAW], old_yaw));
	
	if (ps->viewangles[YAW] != old_yaw) {
		old_yaw = ps->viewangles[YAW];
	}


	//Com_Printf("yaw: %.6f\n", yaw_delta);

	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn(yaw_delta, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_TURN;

	simulation_results results;

	std::list<playback_cmd> cmds;

	for (int i = 0; i < c.numInputs; i++) {

		c.FPS = FPS_GetIdeal(&pm);
		c.numInputs = c.FPS/1.5f;

		float oldZ = pm.ps->velocity[2];

		results = PmoveSingleSimulation(&pm, &pml, &c);

		if (pm.ps->groundEntityNum == 1022 || fvec2(pm.ps->velocity) == 0.f)
			return std::nullopt;

		if ((pm.ps->pm_flags & PMF_JUMPING) == 0 && pm.ps->velocity[2] > (abs(oldZ)) / 1.5f) {
			Com_Printf("^5bunce!\n");
			return cmds;
		}

		cmds.push_back(cmd2playback(pm.ps, &pm.cmd, c.FPS));


	}

	return std::nullopt;

	

}