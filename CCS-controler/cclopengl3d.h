#ifndef CCLOPENGL3D_H
#define CCLOPENGL3D_H

#include <QOpenGLWidget>

class cclOpenGL3D:public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit cclOpenGL3D(QWidget *parent);
    void setPulleyAngle(GLfloat _angle);
    void setVehiclePosition(GLfloat _position);
    void animate();//该函数作为刷新的外部接口，最终调用的是下面的paintGL函数来刷新
    void followEnable(bool _follow);

//以下槽函数用以调整观察3D模型的视角
public slots:
    void leftRotate();
    void rightRotate();
    void upRotate();
    void downRotate();
    void resetRotate();

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();//刷新3D模型的绘制，以实现动态

private:
    void drawFilledCircle(float z0, float radius);//画圆
    void drawCuboid(float x0, float y0, float z0);//画长方体
    void drawCylinder(float z1, float z2, float radius);//画圆筒
    void drawWheels(float z1, float z2, float radius);//用圆和圆筒组合成轮子

    GLfloat angle=0.0;
    GLfloat horizontalRotateAngle=0.0;//水平旋转角，以实现3D模型在水平方向上的旋转观察
    GLfloat verticalRotateAngle=0.0;//垂直旋转角，以实现3D模型在垂直方向上的旋转观察
    GLfloat wheelRotateAngle=0.0;//设置轮子每次刷新的旋转角度，以实现轮子的旋转
    GLfloat wheelRadius;
    GLfloat pulleyRadius;//倒立摆转轮半径
    GLfloat pulleyAngle;//倒立摆倾斜角
    GLfloat vehiclePosition;
    GLfloat finalPulleyAngle=0.0;

    bool follow;
};

#endif // CCLOPENGL3D_H
