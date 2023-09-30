#include "pch.hpp"

void Elebot::init(const float _start, const float _destination, const Axis_t axis, const cardinalDirection_e direction)
{
	const fvec3 angles(0.f, static_cast<std::underlying_type_t<cardinalDirection_e>>(direction), 0.f);

	CG_SetYaw(angles.y);

	destination = _destination;
	destinationAxis = axis;
	start = _start;
	distanceMoved = 0.f;
	position = start;
	base_angle = AngleNormalize180(angles.y);
	current_delta = 90.f;
	lowest_delta = 0.f;
	best_delta = 90.f;
	best_fitness = 0.f;
	moveForward = true;
	delta_increment = 0.01f;
	total_distance = std::abs(_start - _destination);
	positionZ = clients->cgameOrigin[2];
	step_ele = false;

	cmds.clear();

	memset(&pm, 0, sizeof(pmove_t));
	memset(&pml, 0, sizeof(pml_t));
	memset(&ps, 0, sizeof(playerState_s));

	pm = PM_Create(&cgs->predictedPlayerState, cinput->GetUserCmd(cinput->currentCmdNum), cinput->GetUserCmd(cinput->currentCmdNum - 1));
	pml = PML_Create(&pm, 333);



	memcpy(&ps, pm.ps, sizeof(playerState_s));
	pm.ps = &ps;

	frameTime = 1000 / 200;

	initialized = true;

	Com_Printf("Elevator[%s]: %.6f\n", axis == Axis_t::X ? "X" : "Y", destination);
	std::cout << "start_pos = " << position << '\n';

	Com_Printf("is_step_ele: %i\n", step_ele = is_step_ele(angles.toforward()));

	if (step_ele)
		total_distance = 0.250f;

	Com_Printf("distance: %.6f\n", total_distance);

}

void Elebot::start_ground()
{
	decltype(auto) elebot = Elebot::getInstance();


	Axis_t axis = CG_GetAxisBasedOnYaw(clients->cgameViewangles[YAW]);
	const cardinalDirection_e direction = CG_RoundAngleToCardinalDirection(clients->cgameViewangles[YAW]);

	const fvec3 angles(0.f, static_cast<std::underlying_type_t<cardinalDirection_e>>(direction), 0.f);
	const fvec3 fwd_angles = angles.toforward();

	const int idx = static_cast<std::underlying_type_t<Axis_t>>(axis);

	elebot.init(clients->cgameOrigin[idx], clients->cgameOrigin[idx] + (fwd_angles[idx] * 0.125f), axis, direction);
}
void Elebot::start_ground(const float destination, const cardinalDirection_e direction, const Axis_t axis)
{
	const int idx = static_cast<std::underlying_type_t<Axis_t>>(axis);
	init(clients->cgameOrigin[idx], destination, axis, direction);


}
bool Elebot::is_step_ele(const fvec3& fwd_angles) const noexcept
{
	fvec3 org = pm.ps->origin;
	const int idx = static_cast<std::underlying_type_t<Axis_t>>(destinationAxis);
	trace_t trace;

	org[idx] = destination + (fwd_angles[idx] * 0.001f);
	CG_TracePoint(pm.maxs, &trace, org, pm.mins, org, pm.ps->clientNum, pm.tracemask, 0, 0);
	return trace.startsolid;

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

	if (step_is_too_big(predicted_position) && moveForward) {
		forward_on_exceeded(predicted_position);
	}
	else {

		if (step_ele && distanceMoved >= 0.125f) {
			//Com_Printf("step ele time!\n");
			return start_ground(destination, CG_RoundAngleToCardinalDirection(-base_angle), destinationAxis);
		}

		if (moveForward) {
			Com_Printf("predicted: ^2%.6f\n", predicted_position);
			playback.SetPlayback(cmds);
			playback.StartPlayback(cmds.front().serverTime, &cgs->predictedPlayerState, cmd, cmds.front(), false);
		}

		else {

			update_deltas(cmd, predicted_position);


		}


	}


	//initialized = false;
}
void Elebot::update_deltas(usercmd_s* cmd, const float new_position)
{
	const float fitness = get_fitness_for_move(new_position);

	if (fitness == destination_reached()) {
		return on_destination_reached(cmd);
	}

	if (lowest_delta && fitness == destination_overstep()) {
		return on_impossible_coordinate(cmd, new_position);
	}

	if (fitness == -1.f) {
		//delta_increment /= 2;
		//delta_increment = std::clamp(delta_increment, 0.01f, delta_increment);

		if (!lowest_delta/* || current_delta < lowest_delta*/) {
			Com_Printf("angles less than ^2%.3f ^7will overstep\n", base_angle + current_delta); //FIX FIX IFX FIX POSITION OUTDATED 
			lowest_delta = current_delta;
		}
		current_delta = lowest_delta + (delta_increment += 0.01f);


	}
	else {

		if(!lowest_delta)
			current_delta -= random(1.f);

		else {
			current_delta = lowest_delta + (delta_increment+=0.01f);
			
		}

	}
	current_delta = AngleNormalize180(current_delta);

	if (fitness >= best_fitness && !lowest_delta) {
		best_fitness = fitness;
		best_delta = current_delta;
		current_delta = best_delta;
		backup_delta = current_delta;
		Com_Printf("best: ^2%.6f -> %.6f\n", best_delta, fitness);
		current_delta -= random(1.f);

	}
	////a worse result than best
	//else {
	//	Com_Printf("distance: ^3%.6f (%.6f)\n", abs(destination - new_position), new_position); //FIX FIX IFX FIX POSITION OUTDATED 
	//}



	//restore position
	reset_prediction();

}
void Elebot::on_destination_reached(usercmd_s* cmd)
{
	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, { 0.f, base_angle + current_delta, 0.f });
	Com_Printf("finished: ^2%.6f ^7with ^2%.6f^7 = ^2%.6f\n", position, base_angle + current_delta, predicted_position);

	//move to last position
	playback.SetPlayback(cmds);
	playback.StartPlayback(cmds.front().serverTime, &cgs->predictedPlayerState, cmd, cmds.front(), false);

	initialized = false;
	return;
}
void Elebot::on_impossible_coordinate(usercmd_s* cmd, const float new_position)
{
	//restore position
	reset_prediction();

	//go to the delta we know is OK
	current_delta = backup_delta;

	float predicted_pos = 0;

	int iterations = 0;

	do {
		cmds.clear();
		reset_prediction();
		current_delta = abs(AngleNormalize90(current_delta += 0.1));
		predicted_pos = start_simulation();


		if (iterations++ > 10000) {
			return FatalError(std::format("expected step less than {:.6f}, but got {:.6f} instead", total_distance - distanceMoved, std::abs(predicted_pos - position)));
		}

	} while (step_is_too_big(predicted_pos));

	playback.SetPlayback(cmds);
	playback.StartPlayback(cmds.front().serverTime, &cgs->predictedPlayerState, cmd, cmds.front(), false);

	//now reset everything we know and then try again if we fail again!
	current_delta = 90.f;
	lowest_delta = 0.f;
	best_delta = 90.f;
	best_fitness = 0.f;
	delta_increment = 0.01f;

	Com_Printf("^2fuck you\n");
	//initialized = false;
	//return;
}
void Elebot::do_playback(usercmd_s* cmd)
{
	if (playback.isPlayback()) {
		playback.doPlayback(cmd);
		testPlayback = 8;
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
	c.buttons = pm.cmd.buttons;
	c.forwardmove = 127;
	c.rightmove = 0;
	c.FPS = FPS;
	c.numInputs = 1;

	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle));

	auto fixed_angle = dynamic_cast<prediction_viewangle_fixed_angle*>(c.turntype.get());

	fixed_angle->forward = 0.f;
	fixed_angle->right = base_angle;
	fixed_angle->up = 0.f;

	if (moveForward == false) {
		fixed_angle->right += current_delta;
	}

	//pm.ps->viewangles[PITCH] = fixed_angle->forward;
	//pm.ps->viewangles[YAW] = base_angle;
	//pm.ps->viewangles[ROLL] = fixed_angle->up;


	//predicted_pm = pm;
	//predicted_pml = pml;
	//memcpy(&predicted_ps, pm.ps, sizeof(playerState_s));
	//predicted_pm.ps = &ps;
	int axis = static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis);

	float old = pm.ps->origin[axis];

	std::cout << "\nfirst origin: " << fvec3(pm.ps->origin) << '\n';

	r = PmoveSingleSimulation(&pm, &pml, &c);

	std::cout << "origin: " << fvec3(pm.ps->origin) << '\n';


	playback_cmd cmd;
	
	int iters = 0;
	//keep predicting while the player has velocity
	while (r.velocity[2] != 0.f || (r.velocity[0] != 0.0f || r.velocity[1] != 0.0f)) {
		c.forwardmove = 0;
		c.rightmove = 0;
		//std::cout << "velocity: " << sqrt(pm_og->ps->velocity[0] * pm_og->ps->velocity[0] + pm_og->ps->velocity[1] * pm_og->ps->velocity[1]) << '\n';

		r = PmoveSingleSimulation(&pm, &pml, &c);

		std::cout << "origin: " << fvec3(pm.ps->origin) << '\n';

		iters++;
		if (iters >= 4)
			break;
	}

	std::cout << "iterations: " << iters << '\n';

	//if (pm.ps->origin[axis] == old) {
	//	Com_Printf("coordinate didn't change... trying to fix\n");

	//	c.FPS = 1000 / (frameTime + 3); //try to get up with a lower fps
	//	c.forwardmove = 127;

	//	do {
	//		r = PmoveSingleSimulation(&pm, &pml, &c);

	//		if (pm.ps->origin[axis] == old) {
	//			Com_Printf("why didn't the coordinate change\n");
	//			//initialized = false;
	//		}
	//	} while ((pm.ps->origin[axis] == old));

	//	while ((r.velocity[0] != 0.0f || r.velocity[1] != 0.0f)) {
	//		c.forwardmove = 0;
	//		c.rightmove = 0;
	//		c.forwardmove = 0;
	//		c.rightmove = 0;
	//		//std::cout << "velocity: " << sqrt(pm_og->ps->velocity[0] * pm_og->ps->velocity[0] + pm_og->ps->velocity[1] * pm_og->ps->velocity[1]) << '\n';

	//		r = PmoveSingleSimulation(&pm, &pml, &c);
	//	}

	//}

	VectorCopy((int*)r.cmd_angles, cmd.angles);
	cmd.forwardmove = 127;
	cmd.rightmove = 0;
	cmd.serverTime = pm.cmd.serverTime;
	cmd.FPS = FPS;
	cmd.buttons = pm.cmd.buttons;
	cmd.weapon = pm.cmd.weapon;
	cmd.offhand = pm.cmd.offHandIndex;
	cmd.viewangles = pm.ps->viewangles;

	cmds.push_back(cmd);

	Com_Printf("^3%.6f -> ^3%.6f (%.6f)\n", old, pm.ps->origin[static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis)], pm.ps->viewangles[YAW]);


	return r.origin[axis];
}
bool Elebot::step_is_too_big(const float new_position)
{
	const float delta = abs(new_position - position);

	std::cout << "move delta: " << delta << '\n';

	return (distanceMoved + delta) > total_distance;
}
void Elebot::forward_on_exceeded(const float new_position) noexcept
{
	Com_Printf("exceeded forward: ^1%.6f (%i FPS)\n", distanceMoved + std::abs(new_position-position), 1000 / frameTime);
	reset_prediction();

	if (frameTime == 2) {
		moveForward = false;
		return;
	}
	frameTime--;

}
void Elebot::angled_on_exceeded(const float new_position) noexcept
{
	Com_Printf("exceeded: ^1%.6f ^7(%.2f <-> %.2f), (%.6f)\n", distanceMoved + std::abs(new_position - position), lowest_delta, best_delta, get_fitness_for_move(new_position));

	lowest_delta = current_delta;
	current_delta = random(lowest_delta, best_delta);
	reset_prediction();
	if (std::abs(lowest_delta - best_delta) < 1.f)
		(lowest_delta -= 1.f) < 0 ? lowest_delta = 0 : 0;

}
float Elebot::get_overstep_fitness(const float new_position) noexcept
{
	//assumes that the current delta is too deep
	//returns a value from 0.f to 1.f
	//1.f 

}
float Elebot::get_fitness_for_move(const float new_position) noexcept
{
	if (step_is_too_big(new_position)) {
		return -1.f;
	}

	const float delta = abs(new_position - position);
	
	std::cout << std::format("({} + {}) = {}\n", distanceMoved, delta, distanceMoved + delta);

	return (distanceMoved + delta) / total_distance;

}
float Elebot::get_new_delta() const noexcept
{
	return random(lowest_delta, 90.f);
}
void Elebot::update_stats(const float new_position)
{
	//if (moveForward == false) {
	//	const float fitness = get_fitness_for_move(new_position);

	//	if (fitness >= best_fitness) {
	//		best_delta = current_delta;
	//		Com_Printf("best: ^2%.6f\n", best_delta);
	//	}
	//	current_delta = random(lowest_delta, best_delta);


	//}

	const float delta = abs(new_position - position);
	position = clients->cgameOrigin[static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis)];
	positionZ = clients->cgameOrigin[2];
	std::cout << "new position: " << std::fixed << position << '\n';

	distanceMoved += delta;

	if (distanceMoved > total_distance) {
		Com_Printf("^1oops we went over the coordinate!\n");

		start_ground(destination, CG_RoundAngleToCardinalDirection(base_angle + 180), destinationAxis);


	}

}

void Elebot::reset_prediction() noexcept
{
	pm.ps->origin[int(destinationAxis)] = position;
	pm.ps->origin[2] = positionZ;

}