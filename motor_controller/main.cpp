#include <iostream>
#include <string>
#include <limits>
#include <cstdlib> // <--- 记得加这个头文件
using namespace std;

// 定义电机旋转方向的枚举
enum class Direction {
    CLOCKWISE,       // 顺时针
    COUNTER_CLOCKWISE // 逆时针
};

class MotorController {
private:
    bool isMotorOn;
    int speed;
    Direction currentDirection;

    // 辅助函数：获取方向的字符串表示
    string getDirectionString() const {
        return (currentDirection == Direction::CLOCKWISE) ? "顺时针 (Clockwise)" : "逆时针 (Counter-Clockwise)";
    }

public:
    // 构造函数
    MotorController() : isMotorOn(false), speed(0), currentDirection(Direction::CLOCKWISE) {}

    // 1. 开启电机
    void turnOn() {
        if (isMotorOn) {
            cout << ">> 提示：电机已经是开启状态。" << endl;
        } else {
            isMotorOn = true;
            speed = 0; // 默认开启时速度为0
            cout << ">> 成功：电机已启动。" << endl;
        }
    }

    // 0. 关闭电机
    void turnOff() {
        if (!isMotorOn) {
            cout << ">> 提示：电机已经是关闭状态。" << endl;
        } else {
            isMotorOn = false;
            speed = 0;
            cout << ">> 成功：电机已关闭。" << endl;
        }
    }

    // 2. 修改电机速度
    void setSpeed() {
        if (!isMotorOn) {
            cout << ">> 错误：请先开启电机！" << endl;
            return;
        }
        int newSpeed;
        cout << "请输入新的速度 (0-100): ";
        while (!(cin >> newSpeed) || newSpeed < 0 || newSpeed > 100) {
            cout << "输入无效，请输入 0 到 100 之间的整数: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        speed = newSpeed;
        cout << ">> 设置成功：当前速度为 " << speed << endl;
    }

    // 3. 查看当前电机速度
    void viewSpeed() const {
        if (!isMotorOn) {
            cout << ">> 错误：电机未开启，无法读取数值。" << endl;
            return;
        }
        cout << ">> 当前速度: " << speed << endl;
    }

    // 4. 改变电机旋转方向
    void changeDirection() {
        if (!isMotorOn) {
            cout << ">> 错误：请先开启电机！" << endl;
            return;
        }
        cout << "选择方向 (0: 顺时针, 1: 逆时针): ";
        int dirChoice;
        cin >> dirChoice;
        if (dirChoice == 0) {
            currentDirection = Direction::CLOCKWISE;
        } else if (dirChoice == 1) {
            currentDirection = Direction::COUNTER_CLOCKWISE;
        } else {
            cout << ">> 错误：无效的方向选择。" << endl;
            return;
        }
        cout << ">> 设置成功：当前方向为 " << getDirectionString() << endl;
    }

    // 5. 查看当前电机旋转方向
    void viewDirection() const {
        if (!isMotorOn) {
            cout << ">> 错误：电机未开启，无法读取数值。" << endl;
            return;
        }
        cout << ">> 当前方向: " << getDirectionString() << endl;
    }

    // 检查电机是否运行（用于主循环判断是否退出）
    bool isOn() const {
        return isMotorOn;
    }
};

void showMenu() {
    cout << "\n----------------------------------------" << endl;
    cout << "请输入数字来选择你的模式:" << endl;
    cout << "0. 关闭电机 (并退出程序)" << endl;
    cout << "1. 开启电机" << endl;
    cout << "2. 修改电机速度" << endl;
    cout << "3. 查看当前电机速度" << endl;
    cout << "4. 改变电机旋转方向 (顺时针 || 逆时针)" << endl;
    cout << "5. 查看当前电机旋转方向 (顺时针 || 逆时针)" << endl;
    cout << "----------------------------------------" << endl;
    cout << "请输入选项: ";
}

int main() {
    system("chcp 65001");
    MotorController motor;
    int choice;
    bool keepRunning = true;

    cout << "=== MotorController 系统初始化 ===" << endl;

    while (keepRunning) {
        showMenu();
        
        // 输入验证
        if (!(cin >> choice)) {
            cout << ">> 错误：请输入数字！" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 0:
                motor.turnOff();
                cout << "程序即将退出..." << endl;
                keepRunning = false;
                break;
            case 1:
                motor.turnOn();
                break;
            case 2:
                motor.setSpeed();
                break;
            case 3:
                motor.viewSpeed();
                break;
            case 4:
                motor.changeDirection();
                break;
            case 5:
                motor.viewDirection();
                break;
            default:
                cout << ">> 错误：无效的选项，请重新输入。" << endl;
                break;
        }
    }
    system("pause");
    return 0;
}