#include "cyhpidcontroller.h"

cyhPIDController::cyhPIDController(float _Kp, float _Kd, int _type)
{
    Kp = _Kp;
    Kd = _Kd;
    type = _type;
    error = 0.0;
    errorLast = 0.0;
}

float cyhPIDController::PIDOutput(float targetValue, float actualValue)
{
    errorLast = error;
    error = targetValue + type * actualValue;
    return Kp * error + Kd * (error - errorLast);
}
