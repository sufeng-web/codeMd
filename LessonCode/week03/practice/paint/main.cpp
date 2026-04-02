#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <string>

struct Stroke {
    std::vector<POINT> pts;
};

static std::vector<Stroke> g_strokes;   // 已提交笔画（持久化，用于重绘）
static Stroke g_current;                // 正在绘制的笔画
static Stroke g_pending;                // 待提交笔画（用于双击过滤）
static bool g_drawing = false;
static bool g_hasPending = false;

static std::wstring g_hint = L"左键拖动绘制 | C清空 | ESC退出";

static const UINT_PTR TIMER_COMMIT = 1; // 定时器：双击时间窗结束后提交 pending

static int StrokeCountForHint() {
    return (int)g_strokes.size() + (g_hasPending ? 1 : 0);
}

static void UpdateHintDefaultOrCount() {
    int n = StrokeCountForHint();
    if (n <= 0) {
        g_hint = L"左键拖动绘制 | C清空 | ESC退出";
    } else {
        g_hint = L"已绘制 " + std::to_wstring(n) + L" 笔 | 左键拖动绘制 | C清空 | ESC退出";
    }
}

static void UpdateHintDrawing() {
    g_hint = L"绘制中... 松开左键结束";
}

static void UpdateHintCleared() {
    g_hint = L"已清空 | 左键拖动绘制 | C清空 | ESC退出";
}

static void CancelPending(HWND hwnd) {
    if (g_hasPending) {
        g_hasPending = false;
        g_pending.pts.clear();
        KillTimer(hwnd, TIMER_COMMIT);
    }
}

static void StartPendingCommitTimer(HWND hwnd) {
    KillTimer(hwnd, TIMER_COMMIT);
    SetTimer(hwnd, TIMER_COMMIT, GetDoubleClickTime(), NULL);
}

static void CommitPendingIfAny(HWND hwnd) {
    if (g_hasPending) {
        g_strokes.push_back(g_pending);
        g_pending.pts.clear();
        g_hasPending = false;
        KillTimer(hwnd, TIMER_COMMIT);
    }
}

static void EndCurrentStrokeToPending(HWND hwnd) {
    // 结束当前笔：若点数>=2，进入 pending（延迟提交以过滤双击）
    // 若点数<2，丢弃并恢复提示，避免停留在“绘制中”
    g_drawing = false;

    if (g_current.pts.size() >= 2) {
        // 先把上一次 pending 先提交（避免连续快速画导致 pending 覆盖）
        // 也可以选择直接覆盖；这里提交更直观
        CommitPendingIfAny(hwnd);

        g_pending = g_current;
        g_hasPending = true;
        StartPendingCommitTimer(hwnd);

        // 提示立即显示“已绘制N笔”（含待提交），避免感知延迟
        UpdateHintDefaultOrCount();
    } else {
        // 无效笔画（仅点击未移动）
        UpdateHintDefaultOrCount();
    }

    g_current.pts.clear();
}

static void DrawStroke(HDC hdc, const Stroke& s) {
    if (s.pts.size() < 2) return;
    Polyline(hdc, s.pts.data(), (int)s.pts.size());
}

static void DrawAll(HWND hwnd, HDC hdc) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    // 背景
    HBRUSH bg = (HBRUSH)(COLOR_WINDOW + 1);
    FillRect(hdc, &rc, bg);

    // 画笔（简单黑色）
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HGDIOBJ oldPen = SelectObject(hdc, pen);

    // 已提交笔画
    for (const auto& s : g_strokes) {
        DrawStroke(hdc, s);
    }

    // 待提交笔画（为了让“已绘制N笔（含待提交）”视觉一致）
    if (g_hasPending) {
        DrawStroke(hdc, g_pending);
    }

    // 当前正在绘制的笔画
    if (g_drawing) {
        DrawStroke(hdc, g_current);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    // 提示文字（左上角）
    SetBkMode(hdc, TRANSPARENT);
    RECT textRc = rc;
    textRc.left += 8;
    textRc.top += 6;

    // 给提示文字加一个浅底（可选）
    // 这里做最小实现：直接 DrawText
    DrawTextW(hdc, g_hint.c_str(), -1, &textRc, DT_LEFT | DT_TOP | DT_SINGLELINE);
}

static void OnClear(HWND hwnd) {
    g_strokes.clear();
    g_current.pts.clear();
    CancelPending(hwnd);

    UpdateHintCleared();
    InvalidateRect(hwnd, NULL, TRUE);
}

static void OnEsc(HWND hwnd) {
    int ret = MessageBoxW(hwnd, L"是否退出？", L"提示", MB_YESNO | MB_ICONQUESTION);
    if (ret == IDYES) {
        DestroyWindow(hwnd);
    }
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // 允许双击消息（不写也通常能收到，但保险起见）
        // 也可在注册窗口类时设置 CS_DBLCLKS，这里不再重复
        return 0;

    case WM_LBUTTONDOWN: {
        // 【修复】：如果在此刻还有 pending 的笔画，说明用户是在快速绘制新笔画
        // 遇到新的普通按下，应该立刻将其正式提交，而不是丢弃。
        CommitPendingIfAny(hwnd);

        SetCapture(hwnd);
        g_drawing = true;
        g_current.pts.clear();

        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        g_current.pts.push_back(pt);

        UpdateHintDrawing();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (g_drawing && (wParam & MK_LBUTTON)) {
            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

            // 简单去重：与上一个点相同就不加
            if (g_current.pts.empty() ||
                g_current.pts.back().x != pt.x ||
                g_current.pts.back().y != pt.y) {
                g_current.pts.push_back(pt);
                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;
    }

    case WM_LBUTTONUP: {
        if (GetCapture() == hwnd) ReleaseCapture();
        if (g_drawing) {
            EndCurrentStrokeToPending(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_NCLBUTTONUP:
        // 非客户区松开：也要结束当前笔，避免一直“绘制中”
        if (GetCapture() == hwnd) ReleaseCapture();
        if (g_drawing) {
            EndCurrentStrokeToPending(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_CAPTURECHANGED:
        // 任何原因导致 capture 丢失，也要收尾
        if (g_drawing) {
            EndCurrentStrokeToPending(hwnd);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_LBUTTONDBLCLK:
        // 双击：丢弃 pending（如果已经产生），并丢弃当前笔
        CancelPending(hwnd);
        g_current.pts.clear();
        g_drawing = false;

        UpdateHintDefaultOrCount();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_COMMIT) {
            // 双击时间窗过了：正式提交 pending
            CommitPendingIfAny(hwnd);
            UpdateHintDefaultOrCount();
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        break;

    case WM_KEYDOWN:
        if (wParam == 'C' || wParam == 'c') {
            OnClear(hwnd);
            return 0;
        }
        if (wParam == VK_ESCAPE) {
            OnEsc(hwnd);
            return 0;
        }
        return 0;

    case WM_SIZE:
        // Resize 策略：保留原有笔画，按新客户区重绘（超出部分自然裁剪）
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);

        // 获取当前窗口客户区尺寸
        RECT rc;
        GetClientRect(hwnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        // 1. 创建内存DC和兼容位图（双缓冲核心）
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

        // 2. 将所有绘制操作输出到内存DC（后台绘制）
        DrawAll(hwnd, memDC);

        // 3. 一次性将完整的内存图像拷贝到屏幕DC（前台显示）
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // 4. 释放资源
        SelectObject(memDC, oldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        CancelPending(hwnd);
        PostQuitMessage(0);
        return 0;

    case WM_ERASEBKGND:
        return 1; // 告诉系统不需要默认擦除背景，避免闪屏
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t* kClassName = L"WPS_DRAW_DEMO";

    WNDCLASSW wc{};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // CS_DBLCLKS: 产生 WM_LBUTTONDBLCLK
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = kClassName;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(
        0,
        kClassName,
        L"WPSC++绘图实践",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 600,
        NULL, NULL, hInst, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}