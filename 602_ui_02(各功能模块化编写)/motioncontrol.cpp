#include "motioncontrol.h"

Motioncontrol::Motioncontrol(QObject* parent)
    : QObject(parent)
{
    //控制卡初始化
    dmc_board_init();
    //停止状态，当到限位，或者按了停止键，轴停止运动，true:停止运动，false:可以运动
    axis_stopstatus = false;
}

//热复位
void Motioncontrol::axis_reset()
{
    //控制卡初始化
    dmc_board_init();
    //热复位完成提示
    dmc_soft_reset(0);
    Sleep(6e3);//系统休眠

    //热复位完成提示
    QMessageBox* cpt = new QMessageBox;
    cpt->setWindowTitle(QString::fromLocal8Bit("提示"));
    cpt->setText(QString::fromLocal8Bit("热复位完成！"));
    cpt->setIcon(QMessageBox::Information);
    cpt->addButton(QMessageBox::Ok);
    // 设置提示窗口总在顶层显示
    cpt->setWindowFlags(cpt->windowFlags() | Qt::WindowStaysOnTopHint);
    cpt->exec();

}

//所有轴使能
void Motioncontrol::axis_enable()
{
    DWORD TotalAxis;//轴数量
    nmc_get_total_axes(0, &TotalAxis);
    for (int i = 0; i < TotalAxis; i++)
        nmc_set_axis_enable(0, i);
}

//所有轴失能
void Motioncontrol::axis_disable()
{
    DWORD TotalAxis;//轴数量
    nmc_get_total_axes(0, &TotalAxis);
    for (int i = 0; i < TotalAxis; i++)
        nmc_set_axis_disable(0, i);
}

//x轴定长运动
void Motioncontrol::X_axis_pmove(double speed, double len)
{
    //设定速度
    double Min_Vel = 0.25 * speed;        //起始速度
    double Max_Vel = speed;             //最大速度
    double Stop_Vel = 0.25 * speed;       //停止速度
    double Tacc = 1;                //加速时间
    double Tdec = 1;                //减速时间
    dmc_set_profile_unit(0, 0, Min_Vel, Max_Vel, Tacc, Tdec, Stop_Vel);

    //设定s段时间
    WORD s_mode = 0;                  //保留参数，固定值为0
    double s_para = 0.4;              //S段时间
    dmc_set_s_profile(0, 0, s_mode, s_para);

    //设置龙门模式，1号轴跟随0号轴运动
    WORD Master_axis = 0;             //主轴
    WORD Slave_axis = 1;              //跟随轴
    WORD CardNo = 0;                  //运动卡卡号
    WORD Enable = 1;                  //处于使能状态
    double ratio = 1;                 //保留参数
    dmc_set_gear_follow_profile(CardNo, Slave_axis, Enable, Master_axis, ratio);

    //设置主从轴跟随误差停止阈值
    double dstp_error = 50;           //减速停止误差阈值，单位：unit
    double emg_error = 100;           //立即停止误差阈值，单位：unit
    dmc_set_grant_error_protect_unit(CardNo, Slave_axis, Enable, dstp_error, emg_error);//轴号写主轴从轴都可以，只要两轴运动误差大于阈值即报警

    //执行定长运动
    WORD posi_mode = 0;               //相对运动模式：0，绝对运动模式：1
    dmc_pmove_unit(0, 0, len, posi_mode);//相对坐标运动模式
}

//x轴连续运动
void Motioncontrol::X_axis_vmove(double speed, int dir)
{
    //设定速度
    double Min_Vel = 0.25 * speed;        //起始速度
    double Max_Vel = speed;               //最大速度
    double Stop_Vel = 0.25 * speed;       //停止速度
    double Tacc = 1;                    //加速时间
    double Tdec = 1;                    //减速时间
    dmc_set_profile_unit(0, 0, Min_Vel, Max_Vel, Tacc, Tdec, Stop_Vel);

    //设定s段时间
    WORD s_mode = 0;                  //保留参数，固定值为0
    double s_para = 0.4;              //S段时间
    dmc_set_s_profile(0, 0, s_mode, s_para);

    //设置龙门模式，1号轴跟随0号轴运动
    WORD Master_axis = 0;             //主轴
    WORD Slave_axis = 1;              //跟随轴
    WORD CardNo = 0;                  //运动卡卡号
    WORD Enable = 1;                  //处于使能状态
    double ratio = 1;                 //保留参数
    dmc_set_gear_follow_profile(CardNo, Slave_axis, Enable, Master_axis, ratio);

    //设置主从轴跟随误差停止阈值
    double dstp_error = 50;           //减速停止误差阈值，单位：unit 
    double emg_error = 100;           //立即停止误差阈值，单位：unit
    dmc_set_grant_error_protect_unit(CardNo, Slave_axis, Enable, dstp_error, emg_error);//轴号写主轴从轴都可以，只要两轴运动误差大于阈值即报警

    //执行连续运动
    dmc_vmove(0, 0, dir);//dir:运动方向，0，1
}

//x轴停止
void Motioncontrol::X_axis_stop()
{
    WORD stop_mode = 0;//制动方式，0：减速停止，1：紧急停止
    dmc_stop(0, 0, stop_mode);
}

//其余轴定长运动
void Motioncontrol::other_axis_pmove(int axis, double speed, double len)
{
    //如果是x轴，则停止运动
    if (axis == 0 || axis == 1)
        return;

    //设定速度
    double Min_Vel = 0.25 * speed;        //起始速度
    double Max_Vel = speed;               //最大速度
    double Stop_Vel = 0.25 * speed;       //停止速度
    double Tacc = 0.5;                    //加速时间
    double Tdec = 0.5;                    //减速时间
    dmc_set_profile_unit(0, axis, Min_Vel, Max_Vel, Tacc, Tdec, Stop_Vel);

    //设定s段时间
    WORD s_mode = 0;                  //保留参数，固定值为0
    double s_para = 0.2;              //S段时间
    dmc_set_s_profile(0, axis, s_mode, s_para);

    //执行定长运动
    WORD posi_mode = 0;//相对运动模式：0，绝对运动模式：1
    dmc_pmove_unit(0, axis, len, posi_mode);//相对坐标运动模式
}

//其余轴连续运动
void Motioncontrol::other_axis_vmove(int axis, double speed, int dir)
{
    //如果是x轴，则停止运动
    if (axis == 0 || axis == 1)
        return;

    //设定速度
    double Min_Vel = 0.25 * speed;        //起始速度
    double Max_Vel = speed;               //最大速度
    double Stop_Vel = 0.25 * speed;       //停止速度
    double Tacc = 0.5;                    //加速时间
    double Tdec = 0.5;                    //减速时间
    dmc_set_profile_unit(0, axis, Min_Vel, Max_Vel, Tacc, Tdec, Stop_Vel);

    //设定s段时间
    WORD s_mode = 0;                  //保留参数，固定值为0
    double s_para = 0.2;              //S段时间
    dmc_set_s_profile(0, axis, s_mode, s_para);
    qDebug() << "zhixl";

    //执行连续运动
    dmc_vmove(0, axis, dir);//dir:运动方向：0，1
}

//其余轴停止
void Motioncontrol::other_axis_stop(int axis)
{
    WORD stop_mode = 0;//制动方式，0：减速停止，1：紧急停止
    dmc_stop(0, axis, stop_mode);
}

//紧急停止
void Motioncontrol::stop_all_axis()
{
    DWORD TotalAxis;//轴数量
    nmc_get_total_axes(0, &TotalAxis);

    WORD stop_mode = 0;//制动方式，0：减速停止，1：紧急停止
    for (int i = 0; i < TotalAxis; i++)
        dmc_stop(0, i, stop_mode);
}

//读取编码器位置
double Motioncontrol::get_axis_position(WORD axis)
{
    double axis_pos;
    dmc_get_encoder_unit(0, axis, &axis_pos);
    return axis_pos;
}

//所有轴编码器位置归零
void Motioncontrol::set_axis_pos_zero()
{
    DWORD TotalAxis;//轴数量
    nmc_get_total_axes(0, &TotalAxis);
    for (int i = 0; i < TotalAxis; i++)
    {
        dmc_set_encoder_unit(0, i, 0);
    }
}

//自动扫查
void Motioncontrol::auto_scan(QVector<QVector<double>>pos_Vector, int scanmodel_SLR)
{
    qDebug() << "开始自动扫查";
    switch (scanmodel_SLR)
    {
    case 1://短边扫查
        for (int i = 0; i < pos_Vector.size(); i++)
        {
            if (axis_stopstatus)
                break;
            //Y轴向左提出50mm，实际第一个控制点作为起点，不调姿，但Y轴会进行相应的提出与提回
            other_axis_pmove(2, 15000, (-1) * 50000);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Y轴调姿运动
            if (i == (pos_Vector.size() - 1))//到最后一个控制点时，跳出函数，最后一个控制点只作为终点，不调姿
                return;
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(2, 15000, pos_Vector[i][1] - pos_Vector[i - 1][1]);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //舵机调姿
            if (i != 0)
                emit start_duoji(128,pos_Vector[i][3] * (4095.0 / 360.0),01);
            Sleep(1e3);
            //Z1轴调姿
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(3, 10000, pos_Vector[i][2] - pos_Vector[i - 1][2]);
            while (dmc_check_done(0, 3) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Y轴向右提回50mm
            other_axis_pmove(2, 15000, 50000);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //X轴运动
            if (axis_stopstatus)
                break;
            X_axis_pmove((10000.0 / 17.35) * 20, pos_Vector[i + 1][0] - pos_Vector[i][0]);//X轴扫查速度20mm/s
            while (dmc_check_done(0, 0) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);
        }
        break;
    case 2://长边扫查
        for (int i = 0; i < pos_Vector.size(); i++)
        {
            if (axis_stopstatus)
                break;
            //Y轴向右提出50mm，实际第一个控制点作为起点，不调姿，但Y轴会进行相应的提出与提回
            other_axis_pmove(2, 15000, 50000);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Y轴调姿运动
            if (i == (pos_Vector.size() - 1))//到最后一个控制点时，跳出函数，最后一个控制点只作为终点，不调姿
                return;
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(2, 15000, pos_Vector[i][1] - pos_Vector[i - 1][1]);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //舵机调姿
            if (i != 0)
                emit start_duoji(128, pos_Vector[i][3] * (4095.0 / 360.0), 02);
            Sleep(1e3);
            //Z1轴调姿
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(3, 10000, pos_Vector[i][2] - pos_Vector[i - 1][2]);
            while (dmc_check_done(0, 3) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Y轴向左提回50mm
            other_axis_pmove(2, 15000, (-1) * 50000);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //X轴运动
            if (axis_stopstatus)
                break;
            X_axis_pmove((10000.0 / 17.35) * 20, pos_Vector[i + 1][0] - pos_Vector[i][0]);//X轴扫查速度20mm/s
            while (dmc_check_done(0, 0) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);
        }
        break;
    case 3://R角扫查
        for (int i = 0; i < pos_Vector.size(); i++)
        {
            if (axis_stopstatus)
                break;
            //Z2轴向上提出50mm，实际第一个控制点作为起点，不调姿，但Z2轴会进行相应的提出与提回
            other_axis_pmove(4, 10000, 50000);
            while (dmc_check_done(0, 4) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //舵机调姿
            if (i != 0)
                emit start_duoji(128, pos_Vector[i][3] * (4095.0 / 360.0), 03);
            Sleep(1e3);
            //Y轴调姿
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(2, 15000, pos_Vector[i][1] - pos_Vector[i - 1][1]);
            while (dmc_check_done(0, 2) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Z2轴调姿运动
            if (i == (pos_Vector.size() - 1))//到最后一个控制点时，跳出函数，最后一个控制点只作为终点，不调姿
                return;
            if (i != 0)//设置好检测起点直接开始运动，跳过第一次控制点调姿
                other_axis_pmove(4, 10000, pos_Vector[i][2] - pos_Vector[i - 1][2]);
            while (dmc_check_done(0, 4) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            if (axis_stopstatus)
                break;
            //Z2轴向下提回50mm
            other_axis_pmove(4, 10000, (-1) * 50000);
            while (dmc_check_done(0, 4) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);

            //X轴运动
            if (axis_stopstatus)
                break;
            X_axis_pmove((10000.0 / 17.35) * 20, pos_Vector[i + 1][0] - pos_Vector[i][0]);//X轴扫查速度20mm/s
            while (dmc_check_done(0, 0) == 0)//等待轴运动完成，返回值为0，说明指定轴正在运行
            {
            }
            Sleep(1e3);
        }
        break;
    default:
        break;
    }
    axis_stopstatus = false;
}

Motioncontrol::~Motioncontrol()
{
    //关闭控制卡，释放系统资源
    dmc_board_close();
}
