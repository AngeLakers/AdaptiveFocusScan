#pragma once

//线扫法则结构体
struct Line_rule
{
    int elm_num;//阵元数量
    double pitch;//阵元间距
    int aperture_size;//孔径大小
    double focus_angle;//聚焦角度
    double focus_distance;//聚焦距离
    double sound_velocity;//工件声速
    int start_elm;//设置起始阵元
    int end_elm;//设置结束阵元
};