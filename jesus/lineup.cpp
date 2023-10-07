#include "pch.hpp"

void Lineup::prepare_for_journey() noexcept
{
	total_distance = dst.dist(origin);

	for (int i = 0; i < 3; i++) {
		delta_angles[i] = AngularDistance(viewangles[i], destination_angles[i]) / total_distance;
		//	Com_Printf("delta angles[%i]: %.6f\n", i, delta_angles[i]);
	}

	for (int i = 0; i < 3; i++)
		destination_angles[i] = AngleNormalize180(destination_angles[i]);

	old_origin = origin;

	rng_crouch = bool(std::round(random(1.f)));

	//	std::cout << "moving to: " << dst << '\n';

	viewangles_ok = false;
	finished = false;
}

void Lineup::move(usercmd_s* cmd) noexcept
{
	
	if (can_pathfind_to_target() == false) {
		give_up = true;
		return;
	}

	Dvar_FindMalleableVar("com_maxfps")->current.integer = 333;

	if (waiting_for_prediction_accuracy()) {
		if (test_prediction_accuracy()) {

			if (predicted_pos.dist(origin) > lineup_dist) {
				//Com_Printf("^1restart needed\n");
				restart_required = true;

			}
			else {
				//Com_Printf("^2gg\n");
				finished = true;
			}

		}
		return;
	}

	update_viewangles(cmd);
	update_origin();

	yaw2target = AngleNormalize180((dst - fvec3(origin)).toangles().y);

	move_closer(cmd);


}
void Lineup::update_viewangles(usercmd_s* cmd) noexcept
{

	if (fvec3(viewangles).dist(destination_angles) < 1.f) {
		viewangles_ok = true;
		return;
	}
	if (viewangles_ok == true) {
		finished = true;
		return;
	}

	for (int i = 0; i < 3; i++) {
		float angular_difference = destination_angles[i] - viewangles[i];
		float destination_angle = destination_angles[i];

		if (abs(angular_difference) > 180)
			destination_angle += 360;

		angular_difference = destination_angle - viewangles[i];
		delta_angles[i] = angular_difference / abs(total_distance - distance_moved);
	}
	CL_SetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, fvec3(viewangles) + delta_angles);



}
void Lineup::update_origin() noexcept
{
	fvec3 move_delta = (fvec3(origin) - old_origin).abs();
	
	distance_moved += move_delta.mag();

	old_origin = origin;

}
void Lineup::move_closer(usercmd_s* cmd) noexcept
{
	clientInput_t cl = CL_GetInputsFromAngle(AngleNormalize180(yaw2target - viewangles[YAW]));

	float dist = dst.dist(cgs->predictedPlayerState.origin);


	if (dist < 100) {
		//if(rng_crouch)
		//	cmd->buttons |= cmdEnums::crouch;
		fvec3 predicted_pos = predict_stop_position();

		float xy_dist = fvec2(dst).dist(fvec2(predicted_pos));
		float z_dist = std::abs(predicted_pos.z - dst.z);

		if (xy_dist <= lineup_dist && z_dist < 100) {
			time_until_prediction_check = 8;
			return;
		}

		float moveDirection = (predicted_pos - dst).toangles().y;

		cl = CL_GetInputsFromAngle(AngleNormalize180(moveDirection - viewangles[YAW]));

		cmd->forwardmove = -cl.forwardmove;
		cmd->rightmove = -cl.rightmove;

		return;

	}

	//float speed = cgs->predictedPlayerState.speed;
	//float my_vel = fvec2(clients->cgameVelocity).mag();

	//if (my_vel > 15 && my_vel > dist*4 && dist < speed/1.2f && distance_moved < total_distance) {
	//	cmd->forwardmove = -cl.forwardmove;
	//	cmd->rightmove = -cl.rightmove;
	//	return;
	//}

	cmd->forwardmove = cl.forwardmove;
	cmd->rightmove = cl.rightmove;


}
bool Lineup::can_pathfind_to_target() noexcept
{
	float dist = dst.dist(clients->cgameOrigin);
	if (dist < 10)
		return true;

	fvec3 o = clients->cgameOrigin;
	o.z += 60;

	trace_t trace;
	CG_TracePoint(vec3_t{ 1,1,1 }, &trace, o, vec3_t{-1,-1,-1}, dst, cgs->clientNum, MASK_PLAYERSOLID, 0, 0);

	return trace.fraction >= 0.98f;
}
fvec3 Lineup::predict_stop_position()
{
	pmove_t pm = PM_Create(&cgs->predictedPlayerState, CL_GetUserCmd(clients->cmdNumber), CL_GetUserCmd(clients->cmdNumber - 1));
	pml_t pml = PML_Create(&pm, Dvar_FindMalleableVar("com_maxfps")->current.integer);

	prediction_controller c;

	c.buttons = 0;
	c.forwardmove = 0;
	c.rightmove = 0;

	c.numInputs = 1;

	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn));

	auto fixed_angle = dynamic_cast<prediction_viewangle_fixed_turn*>(c.turntype.get());

	fixed_angle->right = 0.f;
	fixed_angle->up = 0.f;

	simulation_results r;
	while (pm.ps->velocity[0] != 0.f || pm.ps->velocity[1] != 0.f || pm.ps->velocity[2] != 0.f) {
		c.forwardmove = 0;
		c.rightmove = 0;
		r = PmoveSingleSimulation(&pm, &pml, &c);

	}
	predicted_pos = pm.ps->origin;
	return predicted_pos;

}