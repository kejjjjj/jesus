#pragma once

#include "pch.hpp"


struct clientInput_t
{
	char forwardmove = 0;
	char rightmove = 0;
};


clientInput_t CL_GetInputsFromAngle(const float yaw);