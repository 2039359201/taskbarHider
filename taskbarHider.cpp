#include <Windows.h>
#include <shellapi.h>
// 定义标识符
#define IDM_EXIT 1001
// 函数声明
void HideTaskbarCompletely();
void RestoreTaskbar();
void HideTaskbar();
void ShowTaskbar();

void AddTrayIcon(HWND hwnd, HICON icon, LPCWSTR tooltip);
void RemoveTrayIcon(HWND hwnd);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main()
{
    WNDCLASSW windowClass = {0};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.lpszClassName = L"MyTrayAppClass";
    // 注册窗口类
    if (RegisterClassW(&windowClass))
    {
        // 创建窗口并隐藏
        HWND hwnd = CreateWindowExW(0, L"MyTrayAppClass", L"MyTrayApp", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
        ShowWindow(hwnd, SW_HIDE);

        // 添加托盘图标
        HICON trayIcon = LoadIcon(NULL, IDI_APPLICATION); // 替换为你的图标
        AddTrayIcon(hwnd, trayIcon, L"MyTrayApp");

        bool result = false;
        // 隐藏任务栏
        HideTaskbarCompletely();
        // 注册热键，使用Ctrl+Alt+T来显示任务栏
        if (RegisterHotKey(NULL, 1, MOD_CONTROL | MOD_ALT, 'T'))
        {
            // 在这里执行你的程序逻辑
            MSG msg = {0};
            while (GetMessage(&msg, NULL, 0, 0) != 0)
            {
                if (msg.message == WM_HOTKEY)
                {
                    result = !result;
                    if (result)
                    {
                        // 收到热键消息，显示任务栏
                        RestoreTaskbar();
                        ShowTaskbar();
                    }
                    else
                    {
                        HideTaskbarCompletely();
                    }
                }
                else if (msg.message == WM_COMMAND && LOWORD(msg.wParam) == IDM_EXIT)
                {
                    // 处理退出菜单
                    RestoreTaskbar();
                    ShowTaskbar();
                    break; // 退出主循环
                }
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            UnregisterHotKey(NULL, 1);
        }
        else
        {
            // 处理注册热键失败的情况
        }
        // 在这里执行你的程序逻辑
        // RestoreTaskbar();
        // HideTaskbar();
        // ShowTaskbar();

        // 移除托盘图标
        RemoveTrayIcon(hwnd);
    }

    return 0;
}

// 完全隐藏任务栏
void HideTaskbarCompletely()
{
    const wchar_t *windowClassName = L"Shell_TrayWnd";
    HWND taskbar = FindWindowW(windowClassName, NULL);
    SetWindowPos(taskbar, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ShowWindow(taskbar, SW_HIDE);
}

// 恢复任务栏为普通窗口
void RestoreTaskbar()
{
    const wchar_t *windowClassName = L"Shell_TrayWnd";
    HWND taskbar = FindWindowW(windowClassName, NULL);
    SetWindowPos(taskbar, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

// 隐藏任务栏
void HideTaskbar()
{
    const wchar_t *windowClassName = L"Shell_TrayWnd";
    HWND taskbar = FindWindowW(windowClassName, NULL);
    SetWindowPos(taskbar, HWND_TOP, 0, 0, 0, 0, 0);
    ShowWindow(taskbar, SW_HIDE);
}

// 显示任务栏
void ShowTaskbar()
{
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", NULL);
    ShowWindow(taskbar, SW_SHOW);
}

// 在系统托盘中添加图标
void AddTrayIcon(HWND hwnd, HICON icon, LPCWSTR tooltip)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = WM_USER + 1; // 定义一个用于托盘消息的自定义消息
    nid.hIcon = icon;

    Shell_NotifyIcon(NIM_ADD, &nid);
}

// 从系统托盘中移除图标
void RemoveTrayIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {0};
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

// 显示托盘上下文菜单
void ShowContextMenu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);

    HMENU hMenu = CreatePopupMenu();
    AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"Exit");

    SetForegroundWindow(hwnd);
    TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);

    DestroyMenu(hMenu);
}

// 窗口过程
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_USER + 1: // 自定义消息，处理托盘图标的点击等事件
        switch (LOWORD(lParam))
        {
        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU: // 鼠标右键点击
            // 处理右键点击的菜单
            ShowContextMenu(hwnd);
            break;
            // 添加其他事件处理
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_EXIT:
            // 处理退出菜单
            PostQuitMessage(0);
            break;
            // 添加其他命令处理
        }
        break;
        // 添加其他窗口消息处理

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}