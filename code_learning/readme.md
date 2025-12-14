# 任务一：MotorController 电机控制程序

## 1. 项目简介
这是一个基于 C++ (或 Python) 编写的电机控制模拟程序。通过定义 `MotorController` 类，实现了对电机状态（开关）、速度和旋转方向的封装与控制。程序具备完整的逻辑判断，确保在电机未开启的情况下无法进行参数修改。

## 2. 编译与运行方式

### 环境要求
- 操作系统：Windows / Linux / MacOS
- 编译器：G++ (C++) 或 Python 3.x

### 启动指令


```bash
# 1. 编译代码
g++ main.cpp -o motor_app

# 2. 运行程序
# Windows:
./motor_app.exe
# Linux/Mac:
./motor_app