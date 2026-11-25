#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <conio.h>   // 可选，仅用于等待按键退出

// 全局标志
bool g_isClicking = false;          // 是否正在连点
int g_intervalMs = 50;              // 点击间隔 (毫秒)
bool g_running = true;              // 程序是否运行

// 热键上次按下状态 (用于防止重复触发)
bool f1_prev = false;
bool f2_prev = false;
bool f3_prev = false;
bool f4_prev = false;
bool esc_prev = false;

// 模拟鼠标左键单击 (使用 SendInput，更现代)
void MouseClick()
{
    // 获取当前光标位置
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // 构造 INPUT 结构，用于鼠标按下和释放
    INPUT inputDown = {0};
    inputDown.type = INPUT_MOUSE;
    inputDown.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    inputDown.mi.dx = cursorPos.x;
    inputDown.mi.dy = cursorPos.y;

    INPUT inputUp = {0};
    inputUp.type = INPUT_MOUSE;
    inputUp.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    inputUp.mi.dx = cursorPos.x;
    inputUp.mi.dy = cursorPos.y;

    // 发送按下和释放事件
    SendInput(1, &inputDown, sizeof(INPUT));
    SendInput(1, &inputUp, sizeof(INPUT));
}

// 延迟函数，在等待期间每隔 10ms 检查一次停止/退出热键
// 如果检测到停止或退出请求，则提前返回 false；否则等待满 interval 后返回 true
bool DelayWithHotkeyCheck(int intervalMs)
{
    const int step = 10;            // 每次检查间隔 10ms
    int elapsed = 0;
    while (elapsed < intervalMs)
    {
        if (!g_running) return false;       // 退出程序
        if (!g_isClicking) return false;    // 用户按 F1 停止连点

        // 检查退出热键 (ESC)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            return false;

        Sleep(10);
    }
    return true;
}

// 显示当前状态 (控制台标题或直接打印)
void UpdateStatus()
{
    system("cls");   // 清屏
    std::cout << "========== 鼠标连点器 (C++) ==========\n";
    std::cout << "当前状态: " << (g_isClicking ? "【连点中】" : "【已停止】") << "\n";
    std::cout << "点击间隔: " << g_intervalMs << " ms\n";
    std::cout << "----------------------------------------\n";
    std::cout << "热键说明:\n";
    std::cout << "  F1 - 开始/停止连点\n";
    std::cout << "  F2 - 增加间隔 (+10ms)\n";
    std::cout << "  F3 - 减少间隔 (-10ms, 最低1ms)\n";
    std::cout << "  F4 - 手动设置间隔\n";
    std::cout << "  ESC - 退出程序\n";
    std::cout << "----------------------------------------\n";
    std::cout << "提示: 连点会在当前鼠标位置模拟左键单击\n";
}

int main()
{
    std::cout << "鼠标连点器启动...\n";
    UpdateStatus();

    // 主循环
    while (g_running)
    {
        // 检测热键 F1 (开始/停止)
        bool f1_now = (GetAsyncKeyState(VK_F1) & 0x8000) != 0;
        if (f1_now && !f1_prev)
        {
            g_isClicking = !g_isClicking;
            UpdateStatus();
            // 如果停止连点，可能正处于 DelayWithHotkeyCheck 中，但循环会立即处理
        }
        f1_prev = f1_now;

        // 检测热键 F2 (增加间隔)
        bool f2_now = (GetAsyncKeyState(VK_F2) & 0x8000) != 0;
        if (f2_now && !f2_prev)
        {
            g_intervalMs += 10;
            if (g_intervalMs > 1000) g_intervalMs = 1000; // 上限 1 秒
            UpdateStatus();
        }
        f2_prev = f2_now;

        // 检测热键 F3 (减少间隔)
        bool f3_now = (GetAsyncKeyState(VK_F3) & 0x8000) != 0;
        if (f3_now && !f3_prev)
        {
            g_intervalMs -= 10;
            if (g_intervalMs < 1) g_intervalMs = 1;
            UpdateStatus();
        }
        f3_prev = f3_now;

        bool f4_now = (GetAsyncKeyState(VK_F4) & 0x8000) != 0;
        if (f4_now && !f4_prev)
        {
          std::cout << "当前点击间隔为: " << g_intervalMs << " ms" << std::endl;
          std::cout << "修改为：" << std::endl;
          std::cin >> g_intervalMs;
          UpdateStatus();
        }
        f4_prev = f4_now;

        // 检测退出热键 ESC
        bool esc_now = (GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0;
        if (esc_now && !esc_prev)
        {
            g_running = false;
            break;
        }
        esc_prev = esc_now;

        // 如果正在连点，则执行一次点击并等待间隔
        if (g_isClicking)
        {
            MouseClick();
            // 在等待间隔期间，如果用户按下了停止或退出，则会提前退出 DelayWithHotkeyCheck
            if (!DelayWithHotkeyCheck(g_intervalMs))
            {
                // 若返回 false，说明用户停止了连点或退出了程序
                // 如果是停止，g_isClicking 已经被主循环设置为 false，不需要额外动作
                // 更新一下界面（可能因停止而改变状态）
                UpdateStatus();
                // 避免循环过于紧凑，稍微让出 CPU
                Sleep(10);
            }
        }
        else
        {
            // 未连点时，降低 CPU 占用
            Sleep(10);
        }
    }

    std::cout << "\n程序已退出。\n";
    return 0;
}