#include "ui_parameter_ui.h"
#include<qdebug.h>
#include"Line_rule_struct.h"
#include<QFileDialog>
#include<QMessageBox>
#include"LTDMC.h"

//解决中文输出乱码
#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

class Parameter_ui : public QWidget
{
    Q_OBJECT

public:
    Parameter_ui(QWidget* parent = nullptr);
    ~Parameter_ui();

    //初始化各控件位置
    void set_ui_position();
    Ui::Parameter_uiClass ui;

    //数字增益
    double dgain;
    //发射电压
    int volt;
    //脉冲宽度
    int pulse_width;
    //增益1,2
    int vga1;
    int vga2;
    //重复频率
    int prf;
    //压缩比
    double range_ratio;
    //数字滤波
    int dfilter;
    //视频检波
    int video_detect;
    //线扫法则相关参数
    Line_rule myline_rule;

    //闸门相关
    int ys = 0;
    int top_door = 200;
    int bot_door = 400;
    int left_door = 0;
    int right_door = 800;
    int minwidth = 40;//成像框最小宽度
    int B_Scan_label_height = 600;
    int B_Scan_label_width = 800;

    //加载工艺
    void readTechnique();

    //保存工艺
    void saveTechnique();

    //新建扫查路径
    void creat_path();

    //添加控制点
    void add_pos(double x, double y, double z, double angle);

    //删除控制点
    void delete_pos();

    //加载控制点
    void read_path();

    //保存控制点
    void save_path();




private:

signals:
    //设置数字增益
    void setdgain(double dgain);
    //设置发射电压
    void setvolt(int volt);
    //设置脉冲宽度
    void setpulsewidth(int pulse_width);
    //设置增益1,2
    void setvga(int vga1, int vga2);
    //设置重复频率
    void setprf(int prf);
    //设置压缩比
    void setrangeratio(double range_ratio);
    //设置数字滤波
    void setdfilter(int dfilter);
    //设置视频检波
    void setvideodetect(int video_detect);
    //设置线扫法则
    void setpalscan(Line_rule myline_rule);
    //编码器归零
    void resetencoder();
    //延时，闸门数值改变，将值传递出去
    void send_ys_door(int ys, int top_door, int bot_door, int left_door, int right_door);
    //发送已读取的路径至主线程
    void send_path(int pathmodel_SLR, QVector<QVector<double>>pos_Vector);
    //发送编码器精度
    void send_encoder_accuracy(double);

};