#pragma once

#include "pch.hpp"

struct prediction_state
{
	pmove_t pm;
	pml_t pml;
	playerState_s ps;
};


enum class prediction_angle_enumerator : int32_t
{
	FIXED_TURN,
	STRAFEBOT,
	FIXED_ANGLE

};

class prediction_viewangle
{
public:
	prediction_viewangle() = default;
	virtual prediction_angle_enumerator get_type() const = 0;
	virtual fvec3 get_deltas(pmove_t* pm, pml_t* pml) const = 0;
 };

struct prediction_controller
{
	prediction_controller() = default;
	char forwardmove = 127;
	char rightmove = 0;
	int32_t numInputs = 100;
	int buttons = 0;
	int FPS = 125;
	//bool autofps = false;

	//these have to be the last 2
	prediction_angle_enumerator turntype_enum = prediction_angle_enumerator::FIXED_TURN;
	std::unique_ptr<prediction_viewangle> turntype;

	prediction_controller(const prediction_controller&) = delete;
	prediction_controller& operator=(const prediction_controller&) = delete;
};

class prediction_viewangle_fixed_turn : public prediction_viewangle
{
public:

	prediction_viewangle_fixed_turn(float _right, float _up) : right(_right), up(_up){}
	prediction_viewangle_fixed_turn() = default;
	prediction_angle_enumerator get_type() const override 
	{
		return prediction_angle_enumerator::FIXED_TURN;
	}
	fvec3 get_deltas(pmove_t* pm, pml_t* pml) const override
	{
		return {up, -right, 0.f};
	}

	float right = 0.f;
	float up = 0.f;
};
class prediction_viewangle_strafebot : public prediction_viewangle
{
public:
	prediction_angle_enumerator get_type() const override
	{
		return prediction_angle_enumerator::STRAFEBOT;
	}
	fvec3 get_deltas(pmove_t* pm, pml_t* pml) const override
	{
		std::optional<float> delta = CG_GetOptYawDelta(pm, pml);

		if (!delta)
			return { 0,0,0 };


		return { up, delta.value(), 0.f};
	}

	float up;
};
class prediction_viewangle_fixed_angle : public prediction_viewangle
{
public:
	prediction_viewangle_fixed_angle(float _forward, float _right, float _up) : forward(_forward), right(_right), up(_up) {}
	prediction_viewangle_fixed_angle() = default;
	prediction_angle_enumerator get_type() const override
	{
		return prediction_angle_enumerator::FIXED_ANGLE;
	}
	fvec3 get_deltas(pmove_t* pm, pml_t* pml) const override
	{
		return { forward - pm->ps->viewangles[0], right - pm->ps->viewangles[1], up - pm->ps->viewangles[2] };
	}

	float forward;
	float right;
	float up;
};
struct simulation_results
{
	ivec3 cmd_angles;
	char forwardmove;
	char rightmove;
	char weapon;
	char offhand;
	int buttons;
	fvec3 viewangles;
	fvec3 origin;
	fvec3 velocity;
	fvec3 mins, maxs;
	int FPS;
	float camera_yaw = 0;
};

simulation_results PmoveSingleSimulation(pmove_t* pm, pml_t* pml, prediction_controller* controller);