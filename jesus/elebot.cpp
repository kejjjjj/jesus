#include "pch.hpp"

void Elebot::start_ground()
{
	decltype(auto) elebot = Elebot::getInstance();


	Axis_t axis = CG_GetAxisBasedOnYaw(clients->cgameViewangles[YAW]);
	const cardinalDirection_e direction = CG_RoundAngleToCardinalDirection(clients->cgameViewangles[YAW]);

	const fvec3 angles(0.f, static_cast<std::underlying_type_t<cardinalDirection_e>>(direction), 0.f);
	const fvec3 fwd_angles = angles.toforward();

	CG_SetYaw(angles.y);

	const int idx = static_cast<std::underlying_type_t<cardinalDirection_e>>(axis);

	elebot.destination = clients->cgameOrigin[idx] + (fwd_angles[idx] * 0.125f);
	elebot.destinationAxis = axis;
	elebot.start = clients->cgameOrigin[idx];
	elebot.distanceMoved = 0.f;
	elebot.position = elebot.start;

	elebot.pm = PM_Create(&cgs->nextSnap->ps, cinput->GetUserCmd(cinput->currentCmdNum), cinput->GetUserCmd(cinput->currentCmdNum-1));
	elebot.pml = PML_Create(&elebot.pm, 333);
	memcpy(&elebot.ps, elebot.pm.ps, sizeof(playerState_s));
	elebot.pm.ps = &elebot.ps;
	elebot.frameTime = 1000 / 200;

	elebot.initialized = true;

	Com_Printf("Elevator[%s]: %.6f", axis==Axis_t::X ? "X" : "Y", elebot.destination);
}

void Elebot::move(usercmd_s* cmd)
{
	if (!initialized)
		return;

	if (fvec2(clients->cgameVelocity).MagSq() != 0)
		return;

	if (playback.isPlayback() || testPlayback)
		return;
	
	cmds.clear();
	predicted_position = start_simulation();

	if (step_is_too_big(predicted_position)) {
		frameTime--;
		Com_Printf("distMoved: ^1%.6f\nrestoring: %.6f\n", distanceMoved, position);
		pm.ps->origin[int(destinationAxis)] = position;
		//moveForward = false;
		if(frameTime == 3)
			initialized = false;
	}
	else {
		Com_Printf("predicted: ^2%.6f\n", predicted_position);
		playback.SetPlayback(cmds);
		playback.StartPlayback(cmds.front().serverTime, &cgs->nextSnap->ps, cmd, cmds.front(), false);
	}


	//initialized = false;
}
void Elebot::do_playback(usercmd_s* cmd)
{
	if (playback.isPlayback()) {
		playback.doPlayback(cmd);
		testPlayback = cgs->frametime;
		return;
	}

	if (testPlayback)
		testPlayback--;

	if (testPlayback != 1)
		return;

	const int idx = static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis);


	if (clients->cgameOrigin[idx] != predicted_position) {
		Com_Printf("^1prediction failed! (%.6f != %.6f)\n", clients->cgameOrigin[idx], predicted_position);
		pm.ps->origin[idx] = clients->cgameOrigin[idx];

	}

	update_stats(clients->cgameOrigin[idx]);
	
}
float Elebot::start_simulation()
{
	int FPS = 1000 / frameTime;
	prediction_controller c;
	simulation_results r;
	c.buttons = 0;
	c.forwardmove = 127;
	c.rightmove = 0;
	c.FPS = FPS;
	c.numInputs = 1;

	c.turntype_enum = prediction_angle_enumerator::FIXED_TURN;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn));

	//predicted_pm = pm;
	//predicted_pml = pml;
	//memcpy(&predicted_ps, pm.ps, sizeof(playerState_s));
	//predicted_pm.ps = &ps;

	r = PmoveSingleSimulation(&pm, &pml, &c);

	playback_cmd cmd;


	//keep predicting while the player has velocity
	while ((r.velocity[0] != 0.0f || r.velocity[1] != 0.0f)) {
		c.forwardmove = 0;
		c.rightmove = 0;
		c.forwardmove = 0;
		c.rightmove = 0;
		//std::cout << "velocity: " << sqrt(pm_og->ps->velocity[0] * pm_og->ps->velocity[0] + pm_og->ps->velocity[1] * pm_og->ps->velocity[1]) << '\n';

		r = PmoveSingleSimulation(&pm, &pml, &c);
	}


	VectorCopy((int*)r.cmd_angles, cmd.angles);
	cmd.forwardmove = 127;
	cmd.rightmove = 0;
	cmd.serverTime = pm.cmd.serverTime;
	cmd.FPS = FPS;
	cmd.buttons = 0;
	cmd.weapon = pm.cmd.weapon;
	cmd.offhand = pm.cmd.offHandIndex;
	cmd.viewangles = pm.ps->viewangles;

	cmds.push_back(cmd);

	return r.origin[static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis)];
}
bool Elebot::step_is_too_big(const float new_position)
{
	const float delta = abs(new_position - position);

	return (distanceMoved + delta) > 0.125f;
}
void Elebot::update_stats(const float new_position)
{
	const float delta = abs(new_position - position);
	position = clients->cgameOrigin[static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis)];

	distanceMoved += delta;

}