#include "pch.hpp"
void CG_SetYaw(const float ang)
{
	float ref = clients->cgameViewangles[YAW];
	//ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[1] += ref;

}
void CG_SetPitch(const float ang)
{
	float ref = clients->cgameViewangles[PITCH];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[0] += ref;

}
void CG_SetRoll(const float ang)
{
	float ref = clients->cgameViewangles[ROLL];
	ref = fmodf(ref, 360);
	ref -= ref * 2 - ang;
	clients->viewangles[2] += ref;

}
void CG_SetPlayerAngles(const fvec3& target)
{
	CG_SetPitch(target.x);
	CG_SetYaw(target.y);
	CG_SetRoll(target.z);
}
void CL_SetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target)
{
	for (int i = 0; i < 3; i++) {

		float cmdAngle = SHORT2ANGLE(cmd->angles[i]);

		float delta = AngleDelta(delta_angles[i], cmdAngle);
		float real_delta = AngleDelta(delta, delta_angles[i]);
		float final = AngleDelta(delta_angles[i], ((float*)&target)[i]);

		clients->viewangles[i] += real_delta - final;
		cmd->angles[i] += ANGLE2SHORT(real_delta - final);

	}
}
void CL_SetSilentAngles(const fvec3& target)
{
	usercmd_s* cmd = CL_GetUserCmd(clients->cmdNumber - 1);

	cmd->serverTime = cgs->oldTime + 1;
	
	const ivec3 deltas = CL_GetPlayerAngles(cmd, cgs->predictedPlayerState.delta_angles, target);

	cmd->angles[PITCH] += deltas.x;
	cmd->angles[YAW] += deltas.y;
	cmd->angles[ROLL] += deltas.z;

}
ivec3 CL_GetPlayerAngles(usercmd_s* cmd, float* delta_angles, const fvec3& target)
{
	fvec3 results;
	for (int i = 0; i < 3; i++) {

		float cmdAngle = SHORT2ANGLE(cmd->angles[i]);

		float delta = AngleDelta(delta_angles[i], cmdAngle);
		float real_delta = AngleDelta(delta, delta_angles[i]);
		float final = AngleDelta(delta_angles[i], ((float*)&target)[i]);

		results[i] = ANGLE2SHORT(real_delta - final);

	}

	return results;
}
bool CG_SetDeltaAngles(float* dst)
{
	if (!cgs->nextSnap || !cgs->snap)
		return false;

	VectorCopy(dst, cgs->nextSnap->ps.delta_angles);
	VectorCopy(dst, cgs->snap->ps.delta_angles);

	return true;
}
Axis_t CG_GetAxisBasedOnYaw(float yaw)
{
	float nearest = CG_GetNearestCardinalAngle(yaw);
	return (nearest == 0 || nearest == 180 ? Axis_t::X : Axis_t::Y);
}
cardinalDirection_e CG_RoundAngleToCardinalDirection(const float a)
{
	const float rounded = CG_GetNearestCardinalAngle(a);

	if (rounded == 0)
		return cardinalDirection_e::N;
	if (rounded == 90)
		return cardinalDirection_e::W;
	if (rounded == 270)
		return cardinalDirection_e::E;
	return cardinalDirection_e::S;
}
float CG_GetNearestCardinalAngle(float yawangle)
{
	if (yawangle > 180 || yawangle < -180)
		yawangle = AngleNormalize180(yawangle);

	if (yawangle >= 135 || yawangle <= -135)
		return 180;  //X backward
	else if (yawangle <= -45 && yawangle >= -135)
		return 270; //Y backward
	else if (yawangle >= 45 && yawangle <= 135)
		return 90;  //Y forward
	return 0; // x forward


}
float CG_GetNearestCardinalAngle()
{
	const float yawangle = clients->cgameViewangles[YAW];

	if (yawangle >= 135 || yawangle <= -135)
		return 180;  //X backward
	else if (yawangle <= -45 && yawangle >= -135)
		return 270; //Y backward
	else if (yawangle >= 45 && yawangle <= 135)
		return 90;  //Y forward
	return 0; // x forward


}
void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float angle;
	static float sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	angle = angles[YAW] * (M_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);

	angle = angles[PITCH] * (M_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);

	angle = angles[ROLL] * (M_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward) {
		forward[0] = cp * cy;
		forward[1] = cp * sy;
		forward[2] = -sp;
	}
	if (right) {
		right[0] = (-1 * sr * sp * cy + -1 * cr * -sy);
		right[1] = (-1 * sr * sp * sy + -1 * cr * cy);
		right[2] = -1 * sr * cp;
	}
	if (up) {
		up[0] = (cr * sp * cy + -sr * -sy);
		up[1] = (cr * sp * sy + -sr * cy);
		up[2] = cr * cp;
	}
}
float ProjectionX(float angle, float fov)
{
	float SCREEN_WIDTH = cgs->refdef.width;
	float const half_fov_x = DEG2RAD(fov) / 2;
	if (angle >= half_fov_x)
	{
		return 0;
	}
	if (angle <= -half_fov_x)
	{
		return SCREEN_WIDTH;
	}

	return SCREEN_WIDTH / 2 * (1 - angle / half_fov_x);

}
range_t AnglesToRange(float start, float end, float yaw, float fov)
{
	start = DEG2RAD(start);
	end = DEG2RAD(end);
	yaw = DEG2RAD(yaw);

	if (fabsf(end - start) > 2 * (float)M_PI)
	{
		return std::move(range_t{ 0, float(cgs->refdef.width), false });
	}

	bool split = end > start;
	start = AngleNormalizePI(start - yaw);
	end = AngleNormalizePI(end - yaw);


	if (end > start)
	{
		split = !split;
		std::swap(start, end);
	}

	return std::move(range_t{ ProjectionX(start, fov), ProjectionX(end, fov), split });

}

float AngleNormalizePI(float angle)
{
	angle = fmodf(angle + (float)M_PI, 2 * (float)M_PI);
	return angle < 0 ? angle + (float)M_PI : angle - (float)M_PI;
}
float AngleNormalize360(float angle) {
	return (360.0f / 65536) * ((int)(angle * (65536 / 360.0f)) & 65535);
}
float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}
float AngleNormalize90(float angle)
{
	return fmodf(angle + 180 + 90, 180) - 90;

}
float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}

std::optional<float> CG_GetOptYawDelta(pmove_t* pm, pml_t* pml)
{
	char forwardmove = pm->cmd.forwardmove;
	char rightmove = pm->cmd.rightmove;

	float speed = fvec2(pm->ps->velocity).mag();

	if (speed < 1 || forwardmove != 127 || rightmove == 0)
		return std::nullopt;


	float g_speed = pm->ps->speed;
	float FPS = 1000.f / pml->msec;

	float accel = FPS / g_speed * pow(333 / FPS, 2);

	if (accel < 1)
		accel = g_speed / FPS;

	WeaponDef* weapon = BG_WeaponNames[pm->ps->weapon];

	if (pm->ps->groundEntityNum == 1022) {
		g_speed = pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) / 1.12150f;


		if ((pm->cmd.buttons & 8194) != 0){
			g_speed = (pm->ps->speed / (weapon->moveSpeedScale * (pml->groundTrace.normal[2])) * 1.5f) / 1.30506f;
		}

	}

	const float velocitydirection = atan2(pm->ps->velocity[1], pm->ps->velocity[0]) * 180.f / PI;
	const float accelerationAng = atan2(-rightmove, forwardmove) * 180.f / PI;
	const float diff = acos((g_speed - accel) / speed) * 180.f / PI;

	float yaw = pm->ps->viewangles[YAW];

	if (std::isnan(diff))
		return std::nullopt;

	if (rightmove > 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection - diff));
	}
	else if (rightmove < 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection + diff));
	}

	return std::nullopt;
}
std::optional<float> CG_GetOptYawDelta(playerState_s* ps, usercmd_s* cmd, usercmd_s* oldcmd)
{
	char forwardmove = cmd->forwardmove;
	char rightmove = cmd->rightmove;

	float speed = fvec2(ps->velocity).mag();

	if (speed < 1 || forwardmove != 127 || rightmove == 0)
		return std::nullopt;


	float g_speed = ps->speed;
	float FPS = 1000.f / (cmd->serverTime - oldcmd->serverTime);

	float accel = FPS / g_speed * pow(333 / FPS, 2);

	if (accel < 1)
		accel = g_speed / FPS;

	WeaponDef* weapon = BG_WeaponNames[ps->weapon];
	trace_t groundTrace;

	fvec3 end = ps->origin;
	fvec3 start = ps->origin;
	end.z -= 3.f;
	start.z += 3.f;

	CG_TracePoint(vec3_t{ 14,14, 12 }, &groundTrace, start, vec3_t{ -14,-14,0 }, end, ps->clientNum, MASK_PLAYERSOLID, 0, 0);

	float diff = 0;
	if (ps->groundEntityNum == 1022 && groundTrace.walkable) {
		g_speed = ps->speed * (weapon->moveSpeedScale * (groundTrace.normal[2]));


		if ((cmd->buttons & 8194) != 0) {
			g_speed = (ps->speed / (weapon->moveSpeedScale * (groundTrace.normal[2])) * 1.5f) / 1.245060f;
		}

		//diff = acos((ps->speed - accel) / speed) * 180.f / PI;

	}
	diff = acos((g_speed - accel) / speed) * 180.f / PI;

	const float velocitydirection = atan2(ps->velocity[1], ps->velocity[0]) * 180.f / PI;
	const float accelerationAng = atan2(-rightmove, forwardmove) * 180.f / PI;

	float yaw = ps->viewangles[YAW];

	if (std::isnan(diff))
		return std::nullopt;

	if (rightmove > 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection - diff));
	}
	else if (rightmove < 0) {
		return -AngleDelta(yaw + accelerationAng, (velocitydirection + diff));
	}

	return std::nullopt;
}
float AngularDistance(float value1, float value2) {
	float diff = fmod(value2 - value1 + 180, 360) - 180;
	if (diff < -180) {
		diff += 360;
	}
	return std::abs(diff);
}

bool PointWithinLine(const fvec3& start, const fvec3& end, const fvec3& point, float radius)
{
	float distance = start.dist(end);

	float u = ((point.x - start.x) * (end.x - start.x) +
		(point.y - start.y) * (end.y - start.y) +
		(point.z - start.z) * (end.z - start.z)) / (distance * distance);

	if (u < 0.0 || u > 10.0) {
		// Closest point is outside the line segment
		return false;
	}

	fvec3 closestPoint;
	closestPoint.x = start.x + u * (end.x - start.x);
	closestPoint.y = start.y + u * (end.y - start.y);
	closestPoint.z = start.z + u * (end.z - start.z);

	float closestDistance = point.dist(closestPoint);

	return closestDistance <= radius;
}
void CG_FillAngleYaw(float start, float end, float yaw, float y, float h, float fov, const vec4_t color)
{
	range_t const range = AnglesToRange(DEG2RAD(start), DEG2RAD(end), DEG2RAD(yaw), fov);

	if(!range.split)
		R_DrawRect("white", range.x1, y, range.x2 - range.x1, h, color);
	else {
		R_DrawRect("white", 0, y, range.x1, h, color);
		R_DrawRect("white", range.x2, y, cgs->refdef.width - range.x2, h, color);
	}
}