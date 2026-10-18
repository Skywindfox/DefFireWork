// gui_stub.cpp — 控制台程序(deffire-dev)的"没有界面"实现。
//
// 同一套骨架(main.cpp → cpp_start() → 四个阶段 + 一个循环)要服务两种产物:
//
//     deffire-gui-dev  → 链接 appGui/gui.cpp   (真界面)
//     deffire-dev      → 链接本文件         (没有界面)
//
// 这样 main.cpp 和 cpp_start() 只有一份, 不用 #ifdef, 也不用两套入口。
// 控制台版的行为与重构前一致: device_init() → status() 一次 → 退出。
#include "start.h"

int gui_build()
{
    return 0;  // 没有界面要建
}

int render()
{
    // 控制台版没有"帧"可刷 —— 跑完一轮就请求退出, 于是 cpp_start() 里那个
    // while 循环只会转一圈: status() 采一次, 然后退出。
    g_quit = true;
    return 0;
}

void gui_teardown()
{
    // 没有界面要收。
}
