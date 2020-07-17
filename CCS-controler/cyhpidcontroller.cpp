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
    float result = Kp * error + Kd * (error - errorLast);
    return result;
}

void cyhPIDController::setKp(float _Kp)
{
    Kp = _Kp;
}

void cyhPIDController::setKd(float _Kd)
{
    Kd = _Kd;
}
