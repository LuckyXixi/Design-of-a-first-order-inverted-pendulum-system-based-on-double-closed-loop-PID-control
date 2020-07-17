#ifndef CYHPIDCONTROLLER_H
#define CYHPIDCONTROLLER_H

class cyhPIDController
{
public:
    cyhPIDController(float _Kp, float _Kd, int _type);
    float PIDOutput(float targetValue, float actualValue);

private:
    float error; // 目标值与实际值的误差
    float errorLast; //上一次的误差
    float Kp, Kd;
    int type;
};

#endif // CYHPIDCONTROLLER_H
