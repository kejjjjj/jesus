#include "pch.hpp"

void setup_player(pmove_t* pm, pml_t* pml, Axis_t axis, float origin, float destination, float* normals) 
{
	playerState_s* ps = pm->ps;

	VectorClear(ps->velocity);
	ps->origin[int(axis)] = origin;
	ps->origin[2] = 99999.f; //go far up in the sky that we HAVE to be in the air

	float yaw = -(atan2(normals[1], normals[0]) * 180.f / PI);

	if (axis == Axis_t::X)
		yaw = AngleNormalize180(yaw + 180);

	ps->oldVelocity[0] = 0;
	ps->oldVelocity[1] = 0;

	prediction_controller c;
	c.buttons = 0;
	c.forwardmove = 0;
	c.rightmove = 0;
	c.FPS = 333;
	c.turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_angle>(new prediction_viewangle_fixed_angle(0, yaw, 0)));
	c.turntype_enum = prediction_angle_enumerator::FIXED_ANGLE;

	PmoveSingleSimulation(pm, pml, &c);
	PmoveSingleSimulation(pm, pml, &c);
	
}

void elebot_evaluate_angles_midair(playerState_s* ps)
{
	fvec3 start = rg->viewOrg;
	fvec3 end = fvec3(rg->viewOrg) + (fvec3(ps->viewangles).toforward() * 99999);
	trace_t trace;

	CG_TracePoint(vec3_t{ 1,1,1 }, &trace, start, vec3_t{ -1,-1,-1 }, end, ps->clientNum, MASK_PLAYERSOLID, 0, 0);


	if (fabs(trace.normal[0]) != 1.f && fabs(trace.normal[1]) != 1.f) {
		return Com_Printf("invalid surface\n");
	}

	Axis_t hitAxis = fabs(trace.normal[0]) == 1.f ? Axis_t::X : Axis_t::Y;

	fvec3 hitpos = start + ((end - start) * trace.fraction);

	hitpos += (fvec3(trace.normal)) * 14;
	float origin = hitpos[int(hitAxis)];
	hitpos += (fvec3(trace.normal).inverse()) * 0.125f;
	float destination = hitpos[int(hitAxis)];


	Com_Printf("hit: %.6f\n", hitpos[int(hitAxis)]);

	pmove_t pm = PM_Create(&cgs->predictedPlayerState, cinput->GetUserCmd(cinput->currentCmdNum), cinput->GetUserCmd(cinput->currentCmdNum - 1));
	pml_t pml = PML_Create(&pm, 333);
	playerState_s _ps;
	memcpy(&_ps, pm.ps, sizeof(playerState_s));
	pm.ps = &_ps;

	setup_player(&pm, &pml, hitAxis, origin, destination, trace.normal);


}