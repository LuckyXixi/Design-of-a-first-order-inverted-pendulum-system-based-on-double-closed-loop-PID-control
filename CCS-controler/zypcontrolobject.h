#ifndef ZYPCONTROLOBJECT_H
#define ZYPCONTROLOBJECT_H

class zypControlObject
{
public:
    zypControlObject(float uk, float position_temp, float angle_temp);
    void objectWork(float controllerOutput);
    float getAngle();
    float getPosition();

private:
    int k;
    float angle[3];
    float u[3];
    float position[3];
};

#endif // ZYPCONTROLOBJECT_H
