#pragma once

#include <QtWidgets/QMainWindow>
#include<qchart.h>
#include<qchartview.h>
//QT_CHARTS_USE_NAMESPACE //Qt5要使用QChart需要添加命名空间，Qt6则不用
#include "ui_mainwindow.h"
#include"parameter_ui.h"
#include"motion_ui.h"
#include"motioncontrol.h"
#include"servocontrol.h"
#include<qthread.h>
#include<qtimer.h>
#include<windef.h>//加上这个头文件，可以使用WORD型变量
#include"pa22scontrol.h"
#include"Line_rule_struct.h"
#include "dataprocess.h"
#include "datasave.h"
#include<qvalueaxis.h>
#include<qsplineseries.h>
#include<QProgressBar>
#include<QStatusBar>

//解决中文输出乱码
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    //初始化各控件位置
    void set_ui_position();
    //连接运动控制窗口各控件
    void connectmotion_ui();
    //连接扫查设置窗口各控件
    void connectparatemer_ui();

    //相关定时器
    QTimer* refreshtimer;//定时刷新A扫，B扫，C扫图
    QTimer* TimeCscantimer;//时基C扫定时器

    //显示闸门
    QRect BScanDoor;
    QPainter BScanPainter;
    QPixmap BScanDoorPixmap;
    int ys = 0;
    int top_door = 200;
    int bot_door = 400;
    int left_door = 0;
    int right_door = 800;

    //延时，闸门改变时，相应的操作
    void ys_door_changed(int ys, int top_door, int bot_door, int left_door, int right_door);

    //A扫数据容器
    QVector<int>AscanData;
    //实时A扫
    void showLiveAscan();
    //A波显示
    QChart* chart;
    QValueAxis* Xaixs;
    QValueAxis* Yaixs;


    QImage BImage;//接收从子线程传递的QImage,显示放大的B扫图片
    QImage CImage;//接收从子线程传递的QImage,显示放大的C扫图片
    int showCImagedex;//C扫图片滚动，显示Image初始坐标
    QImage CScaledImage;//存放进行放大的图像
    //查看C扫结束后的C扫图像（可以用滚轮前后移动查看）
    void showCImage_usingwheel(int value);
    bool TimeCscanstatus = false;//是否处于时基C扫的状态

    //显示保存进度
    void show_savedata_progress(int);


    //扫查模式，短边扫查，长边扫查，R角扫查
    unsigned int scanmodel_SLR = 1;//1,代表短边，2代表长边，3代表R角,默认为短边
    //扫查模式，0为编码器，1为时基
    int scanmodel = 0;//默认为编码器扫查

    //数据后处理相关
    QString filePath;    //读取数据的二进制文件路径，可以是临时数据路径，也可以是自己选择的路径
    int data_signal_num;
    int data_Clength;
    int data_TimeCscandex;
    int data_CscanStartdex;//第几条数据开始C扫成像
    int data_CscanStopdex;
    int data_CImageHeight;
    int data_startdex;//数据的第几个点进行成像
    int data_signallength;//长度
    bool data_status = false;//判断是否处于读入数据的状态
    void read_data();//读取数据
    int CImagelength;//C扫图像的长度 

    void showCImage();//读取二进制数据，并将其C扫成像
    void showBImage();//读取二进制数据，并将其B扫成像
    void showAWave();//读取二进制数据，并显示其A扫波形

    int data_BScandex;//读取数据，显示第几条数据的B扫
    int data_AWavedex = 0;//读取数据，显示第几条数据的A扫波形
    int LiveAscandex = 0;//实时显示第几条波形

    //B、CLabel鼠标事件是否可用,防止访问空对象
    bool B_LabelMouseStatus = false;
    bool C_LabelMouseStatus = false;

    //扫查路径
    QVector<QVector<double>>pos_Vector;
    int pathmodel_SLR = 1;

    //运动控制相关
    //伺服相关
    // 螺距（电机转一圈10000unit,模组运行多少距离）
    double X_pitch = 10000.0 / 17.35;
    double Y_pitch = 10000.0 / 10.0;
    double Z1_pitch = 10000.0 / 10.0;
    double Z2_pitch = 10000.0 / 10.0;

    //运行速度 设定初始运动速度
    double X_speed = 1 * X_pitch;//初始运动速度为：1mm/s
    double Y_speed = 1000;
    double Z1_speed = 1000;
    double Z2_speed = 1000;

    //运行距离（自行加正负号区分正向运动与反向运动）,初始运动距离为0，即无运动距离限制
    double X_distance = 0;
    double Y_distance = 0;
    double Z1_distance = 0;
    double Z2_distance = 0;

    //限位标志，0：低电平，限位未遮挡，1：高电平，限位被遮挡
    WORD IO_0;//X轴西
    WORD IO_1;//X轴东
    WORD IO_2;//Y轴右
    WORD IO_3;//Y轴左
    WORD IO_4;//Z1轴上
    WORD IO_5;//Z1轴下
    WORD IO_6;//Z2轴上
    WORD IO_7;//Z2轴下

    //轴停止标志，true：碰到限位停止，false：碰到限位不停止，遇到限位了，就往反方向多点几次运动，就能够移动出限位区
    bool X_stopstatus = true;
    bool Y_stopstatus = true;
    bool Z1_stopstatus = true;
    bool Z2_stopstatus = true;

    //读取限位定时器
    QTimer* read_axis_inport_timer;

    //读取限位状态与设置停止标志，伺服使能时启动，伺服失能时关闭
    void read_axis_inport();

    //临时，读取舵机位置定时器
    QTimer* read_servo_timer;

private:
    Ui::MainWindowClass ui;

    //两个子窗口
    Motion_ui* motion_ui;
    Parameter_ui* parameter_ui;

    //运动控制线程
    Motioncontrol* Motionthread;
    //数据处理线程
    DataProcess* Dataprocessthread;
    //数据保存线程
    DataSave* Datasavethread;

    //舵机控制模块
    Servocontrol* Servo_model;

    //22s控制模块
    Pa22scontrol* Pa22s_model;

signals:
    void startshowBscan();//在子线程中启动B扫
    void startshowCscan();//在子线程中启动时基C扫
    void startsave(int signal_num, int top, int bot, int left, int right, int ys);//存入信号数量（a扫信号条数）及创建临时文件,保存线程
    void stopsave(int Cscandex, int signal_num);//存入C扫数据长度，保存线程
    void savedata(QString);//在子线程中保存数据
    void start_auto_scan(QVector<QVector<double>>, int scan_model);//开始自动扫查

};
