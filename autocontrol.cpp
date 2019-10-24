/*
 * @Author: xingzhang.Wu
 * @Date: 2019-10-21 16:07:31
 * @Last Modified by: xingzhang.Wu
 * @Last Modified time: 2019-10-21 16:17:26
 */
#include "autocontrol.h"
#include "globaldata.h"
#include "package.h"
#include "debug.h"
#include <cmath>
#include <QElapsedTimer>
#include <QTimer>

#define PI 57.3

AutoControl::AutoControl(QObject *parent) : QObject(parent),
                                            period(20), l1(0), l2(0.304), l3(0.277),
                                            flag(0), leg(0), v(0), mode(0)
{
    timer = new QTimer();
    timer->setTimerType(Qt::PreciseTimer);
    connect(timer, &QTimer::timeout, this, &AutoControl::moveLeg);
    timer->start(period);
}

int AutoControl::run()
{
    return 0;
}

void AutoControl::reset()
{
    flag = 0;
}

void AutoControl::stop()
{
    mode = 0;
}

/**
 * @brief 单独移动一条腿
 *
 * @param leg 第几条腿
 * @param changePos 腿部末端位置
 * @param v 运动速度
 * @return int
 */
int AutoControl::moveLegSet(int leg, double changePos[], double v, int mode)
{
    this->leg = leg;
    for(int i=0;i<3;i++) {
        this->changePos[i]=changePos[i];
    }
    this->v = v;
    this->mode=mode;
}

int AutoControl::moveLeg()
{
    static double c1 = 0;
    static double c2 = 0;
    static double c3 = 0;
    static double x = 0;
    static double y = 0;
    static double z = 0;
    static double tp = 0;
    static QElapsedTimer t;
    static double currentPos[NODE_NUM];

    if(flag==0){
        if (NODE_NUM != 12)
            return -1;
        flag = 1;
        c1 = (global->currentCanAnalyticalData[leg].position - global->refValue[leg]) / PI;
        c2 = (global->currentCanAnalyticalData[leg + 1].position - global->refValue[leg + 1]) / PI;
        c3 = (global->currentCanAnalyticalData[leg + 2].position - global->refValue[leg + 2]) / PI;

        for(int i=0;i<NODE_NUM;i++)
            currentPos[i]=global->currentCanAnalyticalData[i].position;

        x = l2 * sin(c2) + l3 * sin(c2 + c3);
        y = l2 * sin(c1) * cos(c2) + l3 * sin(c1) * cos(c2 + c3);
        z = -l2 * cos(c1) * cos(c2) - l3 * cos(c1) * cos(c2 + c3);
        tp = 0;

        t.start();
    }

    double d = sqrt((changePos[0] * changePos[0]) + (changePos[1] * changePos[1]) + (changePos[2] * changePos[2]));
    double T1 = d / (v * 0.1);
    tp += (t.elapsed() / 1000.0) * 1 / T1;
    if (tp < 1) {
        double px = 0, py = 0, pz = 0;
        if (leg == 0 || leg == 1)
            changePos[0] = -changePos[0];

        if (leg == 1 || leg == 3)
            changePos[1] = -changePos[1];

        if (leg == 0 || leg == 3)
            px = x + changePos[0] * tp;
        else
            px = -x + changePos[0] * tp;

        py = y + changePos[1] * tp;
        pz = z + changePos[2] * tp;
        c1 = atan(py / pz);
        double f = acos((l2 * l2 + px * px + (py - l1 * sin(c1)) * (py - l1 * sin(c1)) + (pz + l1 * cos(c1)) * (pz + l1 * cos(c1)) - l3 * l3) / (2 * l2 * sqrt(px * px + (py - l1 * sin(c1)) * (py - l1 * sin(c1)) + (pz + l1 * cos(c1)) * (pz + l1 * cos(c1)))));
        c2 = atan(px / (sqrt(py * py + pz * pz)) - l1) - f;
        c3 = (l2 + l3) / l3 * f;

        double cc1=0,cc2=0,cc3=0;

        if (leg == 0 || leg == 3)
        {
            cc1 = c1 * PI + global->refValue[leg];
            cc2 = c2 * PI + global->refValue[leg + 1];
            cc3 = c3 * PI + global->refValue[leg + 2];
        }
        else
        {
            cc1 = -c1 * PI - global->refValue[leg];
            cc2 = -c2 * PI - global->refValue[leg + 1];
            cc3 = -c3 * PI - global->refValue[leg + 2];
        }
#if 0
        if(!isnanl(static_cast<long double>(cc1)))
            global->currentCanAnalyticalData[leg].position = cc1;
        if(!isnanl(static_cast<long double>(cc2)))
            global->currentCanAnalyticalData[leg + 1].position = cc2;
        if(!isnanl(static_cast<long double>(cc3)))
            global->currentCanAnalyticalData[leg + 2].position = cc3;

        qDebug() << global->currentCanAnalyticalData[leg].position << global->currentCanAnalyticalData[leg + 1].position
                 << global->currentCanAnalyticalData[leg + 2].position;

        for(int i=0;i<3;i++)
            Package::packOperate(global->sendId[leg+i], global->currentCanAnalyticalData[leg+i].position, PROTOCOL_TYPE_POS);
#else
        if(!isnanl(static_cast<long double>(cc1)))
            currentPos[leg] = cc1;
        if(!isnanl(static_cast<long double>(cc2)))
            currentPos[leg+1] = cc2;
        if(!isnanl(static_cast<long double>(cc3)))
            currentPos[leg+2] = cc3;

        qDebug() << currentPos[leg] << currentPos[leg+1]
                 << currentPos[leg+2];

        Package::packOperateMulti(global->sendId, currentPos, NODE_NUM, PROTOCOL_TYPE_POS);
#endif
    } else {
        if(mode)
            flag = 0;
    }

    return 0;
}

/**
 * @brief 四足身体移动
 *
 * @param changePos 身体的位置 c a d b
 * @param v 运动速度
 * @return int
 */
int AutoControl::moveBody(double changePos[], double v)
{
    if (NODE_NUM != 12)
        return -1;
    double ca1 = (global->currentCanAnalyticalData[3].position - global->refValue[3]) / PI;
    double ca2 = (global->currentCanAnalyticalData[4].position - global->refValue[4]) / PI;
    double ca3 = (global->currentCanAnalyticalData[5].position - global->refValue[5]) / PI;
    double cb1 = (global->currentCanAnalyticalData[9].position - global->refValue[9]) / PI;
    double cb2 = (global->currentCanAnalyticalData[10].position - global->refValue[10]) / PI;
    double cb3 = (global->currentCanAnalyticalData[11].position - global->refValue[11]) / PI;
    double cc1 = (global->currentCanAnalyticalData[0].position - global->refValue[0]) / PI;
    double cc2 = (global->currentCanAnalyticalData[1].position - global->refValue[1]) / PI;
    double cc3 = (global->currentCanAnalyticalData[2].position - global->refValue[2]) / PI;
    double cd1 = (global->currentCanAnalyticalData[6].position - global->refValue[6]) / PI;
    double cd2 = (global->currentCanAnalyticalData[7].position - global->refValue[7]) / PI;
    double cd3 = (global->currentCanAnalyticalData[8].position - global->refValue[8]) / PI;
    double xa = l2 * sin(ca2) + l3 * sin(ca2 + ca3);
    double ya = l2 * sin(ca1) * cos(ca2) + l3 * sin(ca1) * cos(ca2 + ca3);
    double za = -l2 * cos(ca1) * cos(ca2) - l3 * cos(ca1) * cos(ca2 + ca3);
    double xb = l2 * sin(cb2) + l3 * sin(cb2 + cb3);
    double yb = l2 * sin(cb1) * cos(cb2) + l3 * sin(cb1) * cos(cb2 + cb3);
    double zb = -l2 * cos(cb1) * cos(cb2) - l3 * cos(cb1) * cos(cb2 + cb3);
    double xc = l2 * sin(cc2) + l3 * sin(cc2 + cc3);
    double yc = l2 * sin(cc1) * cos(cc2) + l3 * sin(cc1) * cos(cc2 + cc3);
    double zc = -l2 * cos(cc1) * cos(cc2) - l3 * cos(cc1) * cos(cc2 + cc3);
    double xd = l2 * sin(cd2) + l3 * sin(cd2 + cd3);
    double yd = l2 * sin(cd1) * cos(cd2) + l3 * sin(cd1) * cos(cd2 + cd3);
    double zd = -l2 * cos(cd1) * cos(cd2) - l3 * cos(cd1) * cos(cd2 + cd3);
    double tp = 0;
    double f = 0;

    QElapsedTimer t;
    t.start();

    connect(timer, &QTimer::timeout, [=]() mutable {
        double d = sqrt(changePos[0] * changePos[0] + changePos[1] * changePos[1] + changePos[2] * changePos[2]);
        double T1 = d / (v * 0.1);
        tp = tp + (t.elapsed() / 1000) * 1 / T1;

        double pxa = -xa + changePos[0] * tp;
        double pya = ya + changePos[1] * tp;
        double pza = za + changePos[2] * tp;
        ca1 = atan(pya / pza);
        f = acos((l2 * l2 + pxa * pxa + (pya - l1 * sin(ca1)) * (pya - l1 * sin(ca1)) + (pza + l1 * cos(ca1)) * (pza + l1 * cos(ca1)) - l3 * l3) / (2 * l2 * sqrt(pxa * pxa + (pya - l1 * sin(ca1)) * (pya - l1 * sin(ca1)) + (pza + l1 * cos(ca1)) * (pza + l1 * cos(ca1)))));
        ca2 = atan(pxa / (sqrt(pya * pya + pza * pza)) - l1) - f;
        ca3 = (l2 + l3) / l3 * f;

        double pxb = xb - changePos[0] * tp;
        double pyb = yb + changePos[1] * tp;
        double pzb = zb + changePos[2] * tp;
        cb1 = atan(pyb / pzb);
        f = acos((l2 * l2 + pxb * pxb + (pyb - l1 * sin(cb1)) * (pyb - l1 * sin(cb1)) + (pzb + l1 * cos(cb1)) * (pzb + l1 * cos(cb1)) - l3 * l3) / (2 * l2 * sqrt(pxb * pxb + (pyb - l1 * sin(cb1)) * (pyb - l1 * sin(cb1)) + (pzb + l1 * cos(cb1)) * (pzb + l1 * cos(cb1)))));
        cb2 = atan(pxb / (sqrt(pyb * pyb + pzb * pzb)) - l1) - f;
        cb3 = (l2 + l3) / l3 * f;

        double pxc = xc + changePos[0] * tp;
        double pyc = yc - changePos[1] * tp;
        double pzc = zc + changePos[2] * tp;
        cc1 = atan(pyc / pzc);
        f = acos((l2 * l2 + pxc * pxc + (pyc - l1 * sin(cc1)) * (pyc - l1 * sin(cc1)) + (pzc + l1 * cos(cc1)) * (pzc + l1 * cos(cc1)) - l3 * l3) / (2 * l2 * sqrt(pxc * pxc + (pyc - l1 * sin(cc1)) * (pyc - l1 * sin(cc1)) + (pzc + l1 * cos(cc1)) * (pzc + l1 * cos(cc1)))));
        cc2 = atan(pxc / (sqrt(pyc * pyc + pzc * pzc)) - l1) - f;
        cc3 = (l2 + l3) / l3 * f;

        double pxd = -xd - changePos[0] * tp;
        double pyd = yd - changePos[1] * tp;
        double pzd = zd + changePos[2] * tp;
        cd1 = atan(pyd / pzd);
        f = acos((l2 * l2 + pxd * pxd + (pyd - l1 * sin(cd1)) * (pyd - l1 * sin(cd1)) +
                  (pzd + l1 * cos(cd1)) * (pzd + l1 * cos(cd1)) - l3 * l3) /
                 (2 * l2 * sqrt(pxd * pxd + (pyd - l1 * sin(cd1)) * (pyd - l1 * sin(cd1)) + (pzd + l1 * cos(cd1)) * (pzd + l1 * cos(cd1)))));
        cd2 = atan(pxd / (sqrt(pyd * pyd + pzd * pzd)) - l1) - f;
        cd3 = (l2 + l3) / l3 * f;

        global->currentCanAnalyticalData[3].position = -ca1 * PI - global->refValue[3];
        global->currentCanAnalyticalData[4].position = -ca2 * PI - global->refValue[4];
        global->currentCanAnalyticalData[5].position = -ca3 * PI - global->refValue[5];

        global->currentCanAnalyticalData[9].position = cb1 * PI + global->refValue[9];
        global->currentCanAnalyticalData[10].position = cb2 * PI + global->refValue[10];
        global->currentCanAnalyticalData[11].position = cb3 * PI + global->refValue[11];

        global->currentCanAnalyticalData[0].position = cc1 * PI + global->refValue[0];
        global->currentCanAnalyticalData[1].position = cc2 * PI + global->refValue[1];
        global->currentCanAnalyticalData[2].position = cc3 * PI + global->refValue[2];

        global->currentCanAnalyticalData[6].position = -cd1 * PI - global->refValue[6];
        global->currentCanAnalyticalData[7].position = -cd2 * PI - global->refValue[7];
        global->currentCanAnalyticalData[8].position = -cd3 * PI - global->refValue[8];

        if (tp >= 1)
            timer->stop();
    });

    if (!timer->isActive())
        timer->start(period);

    return 0;
}