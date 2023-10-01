#include "pch.hpp"

clientInput_t CL_GetInputsFromAngle(const float targetAngle)
{
    clientInput_t cl;
    cl.forwardmove = 127;

    float closestAngle = 9999.0;

    const int possibleValues[] = { -127, 0, 127 };

    for (int fm : possibleValues) {
        for (int rm : possibleValues) {
            float angle = std::atan2(-rm, fm) * 180.0 / PI;
            float angleDifference = std::abs(targetAngle - angle);

            if (angleDifference < closestAngle) {
                closestAngle = angleDifference;

                cl.forwardmove = fm;
                cl.rightmove = rm;

            }
        }
    }
    
    if (!cl.rightmove && !cl.forwardmove)
        cl.forwardmove = 127;

    return cl;
}