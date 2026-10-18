#ifndef FIRECONTROL_START_H
#define FIRECONTROL_START_H

//这里放置真正的代码位置

int cpp_start();

// ---------------------------------------------------------------------------
// 应用骨架: 四个阶段 + **一个**主循环
//
// 主循环只有一处 —— library/start.cpp 的 cpp_start() 里:
//
//     device_init();                       // 硬件 / 配置, 只调一次
//     gui_build();                         // 建界面, 只调一次
//     while (!g_quit) { status(); render(); }   // 每轮: 跑模块 + 刷一帧
//     gui_teardown();                      // 收尾
//
//   status()  —— **各模块的采集都挂这里**(传感器、状态…); 循环里反复调,
//                内部自带节流(见 start.cpp), 不会每毫秒读一次硬件。
//   render()  —— 刷一帧界面; 循环里反复调。
//   gui_build / render / gui_teardown 在控制台版里是空实现(见 appGui/gui_stub.cpp),
//   所以同一套 source/main.cpp + cpp_start() 能同时服务两种产物, 不用 #ifdef。
// ---------------------------------------------------------------------------
int device_init();
int status();
int gui_build();
int render();
void gui_teardown();

// ---------------------------------------------------------------------------
// 程序级全局状态 —— **定义在 main.cpp**, 其它文件通过这里的 extern 引用。
//
// 只放"整个程序共享"的东西。GUI 内部句柄(g_disp / g_font_* / g_footer_bar /
// g_page / g_page_dirty / g_win_w / …)留在 gui.cpp —— 那是界面的实现细节,
// 露到这里只会让 main.cpp 变成第二个 GUI 文件。
// ---------------------------------------------------------------------------
extern bool g_quit;      // 主循环退出标志: 按钮 / 按键 / 关窗都置它
extern int g_argc;       // 命令行参数, 只有 gui_build() 用(--windowed / --font=)
extern char **g_argv;

// ---------------------------------------------------------------------------
// 板载硬件控制 (正点原子 ATK-DLMP135 主板上原生的 LED 和蜂鸣器)
//
// 这些是**动作**: 调用一次执行一次, 不是"渲染"。按钮回调里直接调用。
// 命名带 _onboard_ 是为了和将来外接的传感器/执行器区分开。
//
// 返回值: 0 成功, -1 失败。
// 开发机上这两个设备路径都不存在, 返回 -1 属正常 —— 拷到板上才是 0。
// ---------------------------------------------------------------------------

// LED: 写 "1" 常亮, "0" 熄灭。
int led_onboard_on();
int led_onboard_off();

// 蜂鸣器 (beep): 写 "1" 响, "0" 停。
int buzzer_onboard_on();
int buzzer_onboard_off();

// 心跳: 让内核以 heartbeat 节奏自动闪烁, 用来确认程序在跑。
//
// 注意: 内核 LED 的 trigger 是**独立的 sysfs 文件**(/sys/class/leds/<名字>/trigger),
// 不是 brightness。下面两个函数往 trigger 文件写 "heartbeat"。
// 路径见 hardware_path.h 的 Led_trigger_Path / Buzzer_trigger_Path。
//
// 恢复常亮/熄灭要先写 trigger="none", 再写 brightness —— 否则 trigger 会一直
// 覆盖 brightness 的值。
int led_onboard_set_heartbeat();
int buzzer_onboard_set_heartbeat();

// 关掉心跳, 交还给 brightness 控制 (trigger 写 "none")。
int led_onboard_clear_heartbeat();
int buzzer_onboard_clear_heartbeat();

// 外部无源蜂鸣器 (接在 GPIOA:6) 开 / 关。
//
// 实现要点: **发声在专用线程里**按固定频率翻转电平, 不依赖主循环 —— 挂在主循环上
// 时音调会被循环周期锁死在几百 Hz(板上实测"音调很低")。频率默认 2 kHz, 可用环境变量
// FIRECONTROL_BUZZER_HZ 覆盖(100..8000), 方便在板上试出这个蜂鸣器最响的音。
// 返回值: 0 = 请求已接受; -1 = 发声线程起不来(具体原因打印到 stderr)。
// 注意: 打不开 GPIO 属于**线程内的异步失败**, 会打一行 stderr 并把状态退回"停",
// 不在这里返回 —— 所以控制台那行才是权威, 界面上显示"已开"只是"请求已发"。
int buzzer_out_set(bool on);

// 收尾: 停声、把 PA6 还给系统、并结束发声线程(join —— 不是丢一个 detached 野线程)。
// 界面退出前调一次; 之后再 buzzer_out_set(true) 仍可重新起来。
void buzzer_out_shutdown();

#endif
