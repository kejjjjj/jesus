#include "pch.hpp"

void Elebot::init(const float _start, const float _destination, const Axis_t axis, const cardinalDirection_e direction)
{
	const fvec3 angles(0.f, static_cast<std::underlying_type_t<cardinalDirection_e>>(direction), 0.f);

	CG_SetYaw(angles.y);

	destination = _destination;
	destinationAxis = axis;
	start = _start;
	distance_moved = 0.f;
	origin = start;
	old_origin = origin;
	total_distance = std::abs(_start - _destination);
	positionZ = clients->cgameOrigin[2];
	step_ele = false;
	test_prediction = 8;
	frameTime = 5;
	bmove_forward = true;
	angle_delta = std::nullopt;

	fps = 1000 / frameTime;

	cmds.clear();

	memset(&pm, 0, sizeof(pmove_t));
	memset(&pml, 0, sizeof(pml_t));
	memset(&ps, 0, sizeof(playerState_s));

	pm = PM_Create(&cgs->predictedPlayerState, cinput->GetUserCmd(cinput->currentCmdNum), cinput->GetUserCmd(cinput->currentCmdNum - 1));
	pml = PML_Create(&pm, 333);



	memcpy(&ps, pm.ps, sizeof(playerState_s));
	pm.ps = &ps;

	initialized = true;

	Com_Printf("Elevator[%s]: %.6f\n", axis == Axis_t::X ? "X" : "Y", destination);

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

	fps = 1000 / frameTime;

	if (distance_moved >= total_distance) {
		initialized = false;
		Com_Printf("^1oopsie it's ruined!\n");
		return;
	}

	update_origin();
	move_closer(cmd);
	
}
void Elebot::do_playback(usercmd_s* cmd)
{

	const int idx = static_cast<std::underlying_type_t<cardinalDirection_e>>(destinationAxis);


	//if (clients->cgameOrigin[idx] != predicted_position) {
	//	Com_Printf("^1prediction failed! (%.6f != %.6f)\n", clients->cgameOrigin[idx], predicted_position);
	//	pm.ps->origin[idx] = clients->cgameOrigin[idx];

	//}
	
}
void Elebot::move_closer(usercmd_s* cmd)
{
	if (move_wait_for_prediction()) {
		if (test_prediction == NULL) {
			if (prediction_failed()) {
				Com_Printf("^1prediction failed!\n");

				if (!bmove_forward) {
					initialized = false;
					Com_Printf("^1epic failure\n");
				}

				Com_Printf("^3expected %.6f instead of %.6f\n", predicted_origin, origin);

				reset_prediction();
			}
			else {
				Com_Printf("^2big success\n");
			}
		}
		return;
	}

	if (is_interrupted()) {
		initialized = false;
		return;
	}

	if (bmove_forward) {
		if (!move_forward(cmd))
			return;
	}
	else {
		if (angle_delta == std::nullopt) {
			angle_delta = eval_angle(cmd);
			fvec3 angles = clients->cgameViewangles;
			angles.y = angle_delta.value();
			CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, angles);
		}

		if (!move_sideways(cmd))
			return;
	}



	playback.SetPlayback(cmds);
	playback.StartPlayback(cmd->serverTime, &cgs->predictedPlayerState, cmd, cmds.front(), true);
	playback.doPlayback(cmd);
}
bool Elebot::move_forward(usercmd_s* cmd)
{
	fvec3 end = P_PredictNextPosition(cmd, 127, 0, pm.ps->viewangles[YAW]);

	//predicted_origin = end[static_cast<int>(destinationAxis)];

	fps = 1000 / frameTime;

	//Com_Printf("^2end: %.6f vs %.6f\n", predicted_origin, origin);
	predicted_origin = end[static_cast<int>(destinationAxis)];

	const float delta = std::fabs(end[static_cast<int>(destinationAxis)] - origin);

	if (delta + distance_moved >= 0.125f) {

		if (frameTime > 2) {
			test_prediction = 8;
			frameTime--;
			return false;
		}
		bmove_forward = false;
		//initialized = false;
		return false;
	}
	//else {
	//	test_prediction = 8;

	//}

	return true;
}
bool Elebot::move_sideways(usercmd_s* cmd)
{
	fvec3 target;
	fvec3 self = clients->cgameOrigin;
	int axis = static_cast<int>(destinationAxis);

	target[axis] = destination;
	target[axis == false] = clients->cgameOrigin[axis == false];
	target[2] = positionZ;

	const float yaw2target = AngleNormalize180((target - fvec3(origin)).toangles().y);
	const float dist = target.dist(cgs->predictedPlayerState.origin);

	clientInput_t cl = CL_GetInputsFromAngle(AngleNormalize180(yaw2target - clients->cgameViewangles[YAW]));

	fvec3 predicted_pos = P_PredictNextPosition(cmd, cl.forwardmove, cl.rightmove, pm.ps->viewangles[YAW]);

	const float delta = std::fabs(predicted_pos[axis] - origin);


	if (destination == predicted_pos[axis]) {
		initialized = false;
		return false;
	}

	float moveDirection = (predicted_pos - target).toangles().y;

	cl = CL_GetInputsFromAngle(AngleNormalize180(moveDirection - clients->cgameViewangles[YAW]));

	if (delta + distance_moved >= 0.125f) {
		//FIX MEEEEEEEEE
		
		//cmds.back().forwardmove = -cl.forwardmove;
		//cmds.back().rightmove = -cl.rightmove;
		//reset_prediction();
		//	initialized = false;
		return true;
	}

	cmds.back().forwardmove = cl.forwardmove;
	cmds.back().rightmove = cl.rightmove;

	return true;

}
float Elebot::eval_angle(usercmd_s* cmd) noexcept
{
	//fps = 333;
	//frameTime = 3;
	//1st get the end position of a move exactly forwards
	std::cout << "predicting from: " << std::fixed << fvec3(clients->cgameOrigin) << '\n';
	float predicted_pos = P_PredictNextPosition(cmd, 127, 0, pm.ps->viewangles[YAW]);
	cmds.clear();

	std::cout << "predicting to: " << std::fixed << fvec3(pm.ps->origin) << '\n';

	reset_prediction();
	
	int axis = static_cast<int>(destinationAxis);

	float org_delta = destination - origin;
	float step_size = predicted_pos - origin;

	float theta = org_delta / step_size;
	float delta = std::atan(theta) * 180.f / PI;

	//Com_Printf("best delta would be ^2%.6f\n", angle);


	float yaw_delta = pm.ps->delta_angles[YAW];

	std::cout << "delta: " << delta << '\n';
	float delta_loop = pm.ps->viewangles[YAW] + delta;

	do {

		predicted_pos = P_PredictNextPosition(cmd, 0, 127, delta_loop);
		cmds.clear();
		float move_delta = std::abs(predicted_pos - origin);

		std::cout << std::format("{:.6f} + {:.6f} = {:.6f}\n", move_delta, distance_moved, move_delta + distance_moved);

		if (move_delta + distance_moved == total_distance) {
			break;
		}
		//didn't move enough
		else if (move_delta + distance_moved < total_distance) {
			
			delta_loop += 0.01f;
		}
		//moved too much
		else if (move_delta + distance_moved > total_distance) {
			delta_loop -= 0.01f;
		}

		pm.ps->delta_angles[YAW] = yaw_delta;

		reset_prediction();

	} while (true);


	return 0.f;
}
bool Elebot::is_interrupted() const noexcept
{
	/*constexpr*/ byte keys[] = { 'W', 'A', 'S', 'D' };

	for (byte key : keys)
		if (GetAsyncKeyState(key) & 1)
			return true;

	return false;

}
void Elebot::update_origin() noexcept
{
	origin = clients->cgameOrigin[static_cast<int>(destinationAxis)];
	float move_delta = std::abs(((origin) - old_origin));
	distance_moved += move_delta;

	old_origin = origin;
	positionZ = clients->cgameOrigin[2];

}
void Elebot::reset_prediction() noexcept
{
	pm.ps->origin[int(destinationAxis)] = origin;
	pm.ps->origin[2] = positionZ;

}

float Elebot::P_PredictNextPosition(usercmd_s* ucmd, char forwardmove, char rightmove, float yaw)
{
	pm = PM_Create(&cgs->predictedPlayerState, ucmd, CL_GetUserCmd(clients->cmdNumber));
	pml = PML_Create(&pm, 1000 / frameTime);

	prediction_controller c;

	c.buttons = 0;
	c.forwardmove = forwardmove;
	c.rightmove = rightmove;

	c.numInputs = 1;

	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle));

	c.FPS = 1000 / pml.msec;

	auto fixed_angle = dynamic_cast<prediction_viewangle_fixed_angle*>(c.turntype.get());

	fixed_angle->right = yaw;
	//fixed_angle->up = 0.f;

	simulation_results r;

	r = PmoveSingleSimulation(&pm, &pml, &c);

	while (pm.ps->velocity[static_cast<int>(destinationAxis)] != 0.f) {
		c.forwardmove = 0;
		c.rightmove = 0;
		r = PmoveSingleSimulation(&pm, &pml, &c);

	}

	cmds.clear();

	playback_cmd cmd;

	VectorCopy((int*)r.cmd_angles, cmd.angles);
	cmd.forwardmove = forwardmove;
	cmd.rightmove = rightmove;
	cmd.serverTime = pm.cmd.serverTime;
	cmd.FPS = c.FPS;
	cmd.buttons = pm.cmd.buttons;
	cmd.weapon = pm.cmd.weapon;
	cmd.offhand = pm.cmd.offHandIndex;
	cmd.viewangles = pm.ps->viewangles;

	cmds.push_back(cmd);

	return pm.ps->origin[static_cast<int>(destinationAxis)];
}