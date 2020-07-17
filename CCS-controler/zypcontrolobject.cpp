#include "zypcontrolobject.h"

zypControlObject::zypControlObject(float uk, float position_temp, float angle_temp)
{
    for (int i = 0; i < 3; i++) {
        u[i] = uk;
        angle[i] = angle_temp;
        position[i] = position_temp;
    }
    k=2;
}

void zypControlObject::objectWork(float controllerOutput)
{
    u[k] = controllerOutput;

    angle[k] = 1.881 * angle[k - 1] - angle[k - 2] + 0.6336 * (u[k] + u[k - 1]);
    position[k] = 0.05 * angle[k] + 0.05 * angle[k - 1] + 2 * position[k - 1] - position[k - 2];

    angle[k - 2] = angle[k - 1];
    angle[k - 1] = angle[k];
    u[k - 2] = u[k - 1];
    u[k - 1] = u[k];
    position[k - 2] = position[k - 1];
    position[k - 1] = position[k];
}

float zypControlObject::getAngle()
{
    return angle[k];
}

float zypControlObject::getPosition()
{
    return position[k];
}
