/*----------------------------------------
   COLORS1.C -- Colors Using Scroll Bars
                (c) Charles Petzold, 1998
  ----------------------------------------*/

#include <windows.h>
#include <tchar.h>

static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScrollProc(HWND, UINT, WPARAM, LPARAM);

int     idFocus;
WNDPROC OldScroll[3];

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    static TCHAR szAppName[] = _T("Colors1");
    HWND         hWnd;
    MSG          msg;
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //CreateSolidBrush(0L); //(HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
        return 1;
    }

    hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1085, 660, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static COLORREF crPrim[3] = { RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255) };
    static HBRUSH  hBrush[3], hBrushStatic;
    static HWND    hwndScroll[3], hwndLabel[3], hwndValue[3], hwndRect;
    static int     color[3], cyChar;
    static RECT    rcColor;
    const TCHAR* szColorLabel[] = { TEXT("Red"), TEXT("Green"), TEXT("Blue") };
    HINSTANCE      hInstance;
    int            i, cxClient, cyClient;
    TCHAR          szBuffer[10];

    switch (message)
    {
    case WM_CREATE:
        hInstance = (HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE);

        // Create the white-rectangle window against which the
        // scroll bars will be positioned. The child window ID is 9.

        hwndRect = CreateWindow(_T("static"), NULL,
            WS_CHILD | WS_VISIBLE | SS_WHITERECT,
            0, 0, 0, 0,
            hWnd, (HMENU)9, hInstance, NULL);

        for (i = 0; i < 3; i++)
        {
            // The three scroll bars have IDs 0, 1, and 2, with
            // scroll bar ranges from 0 through 255.

            hwndScroll[i] = CreateWindow(_T("scrollbar"), NULL,
                WS_CHILD | WS_VISIBLE |
                WS_TABSTOP | SBS_VERT,
                0, 0, 0, 0,
                hWnd, (HMENU)i, hInstance, NULL);

            SetScrollRange(hwndScroll[i], SB_CTL, 0, 255, FALSE);
            SetScrollPos(hwndScroll[i], SB_CTL, 0, FALSE);

            // The three color-name labels have IDs 3, 4, and 5,
            // and text strings "Red", "Green", and "Blue".

            hwndLabel[i] = CreateWindow(_T("static"), szColorLabel[i],
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                0, 0, 0, 0,
                hWnd, (HMENU)(i + 3),
                hInstance, NULL);

            // The three color-value text fields have IDs 6, 7,
            // and 8, and initial text strings of "0".

            hwndValue[i] = CreateWindow(_T("static"), _T("0"),
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                0, 0, 0, 0,
                hWnd, (HMENU)(i + 6),
                hInstance, NULL);

            OldScroll[i] = (WNDPROC)SetWindowLong(hwndScroll[i],
                GWLP_WNDPROC, (LONG)ScrollProc);

            hBrush[i] = CreateSolidBrush(crPrim[i]);
        }

        hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));    //COLOR_BTNHIGHLIGHT <------------------------

        cyChar = HIWORD(GetDialogBaseUnits());
        return 0;

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);
        SetRect(&rcColor, cxClient / 2, 0, cxClient, cyClient);

        MoveWindow(hwndRect, 0, 0, cxClient / 2, cyClient, TRUE);

        for (i = 0; i < 3; i++)
        {
            MoveWindow(hwndScroll[i],
                (2 * i + 1) * cxClient / 14, 2 * cyChar,
                cxClient / 14, cyClient - 4 * cyChar, TRUE);

            MoveWindow(hwndLabel[i],
                (4 * i + 1) * cxClient / 28, cyChar / 2,
                cxClient / 7, cyChar, TRUE);

            MoveWindow(hwndValue[i],
                (4 * i + 1) * cxClient / 28,
                cyClient - 3 * cyChar / 2,
                cxClient / 7, cyChar, TRUE);
        }
        SetFocus(hWnd);
        break;

    case WM_SETFOCUS:
        SetFocus(hwndScroll[idFocus]);
        break;

    case WM_VSCROLL:
        i = GetWindowLong((HWND)lParam, GWLP_ID);

        switch (LOWORD(wParam))
        {
        case SB_PAGEDOWN:
            color[i] += 15;
            // fall through
        case SB_LINEDOWN:
            color[i] = min(255, color[i] + 1);
            break;

        case SB_PAGEUP:
            color[i] -= 15;
            // fall through
        case SB_LINEUP:
            color[i] = max(0, color[i] - 1);
            break;

        case SB_TOP:
            color[i] = 0;
            break;

        case SB_BOTTOM:
            color[i] = 255;
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            color[i] = HIWORD(wParam);
            break;

        default:
            break;
        }
        SetScrollPos(hwndScroll[i], SB_CTL, color[i], TRUE);
        wsprintf(szBuffer, _T("%i"), color[i]);
        SetWindowText(hwndValue[i], szBuffer);

        DeleteObject((HBRUSH)
            SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(color[0], color[1], color[2]))));
            InvalidateRect(hWnd, &rcColor, TRUE);
        return 0;

    case WM_CTLCOLORSCROLLBAR:
        i = GetWindowLong((HWND)lParam, GWLP_ID);
        return (LRESULT)hBrush[i];

    case WM_CTLCOLORSTATIC:
        i = GetWindowLong((HWND)lParam, GWLP_ID);

        if (i >= 3 && i <= 8)    // static text controls
        {
            SetTextColor((HDC)wParam, crPrim[i % 3]);
            SetBkColor((HDC)wParam, GetSysColor(COLOR_BTNHIGHLIGHT));   //COLOR_BTNHIGHLIGHT  <--------------
            return (LRESULT)hBrushStatic;
        }
        break;

    case WM_SYSCOLORCHANGE:
        DeleteObject(hBrushStatic);
        hBrushStatic = CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT));  //COLOR_BTNHIGHLIGHT   <-----------------
        break;

    case WM_DESTROY:
        DeleteObject((HBRUSH)SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG)GetStockObject(WHITE_BRUSH)));

        for (i = 0; i < 3; i++)
            DeleteObject(hBrush[i]);

        DeleteObject(hBrushStatic);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
}

LRESULT CALLBACK ScrollProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int id = GetWindowLong(hWnd, GWLP_ID);

    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == VK_TAB)
            SetFocus(GetDlgItem(GetParent(hWnd),
                (id + (GetKeyState(VK_SHIFT) < 0 ? 2 : 1)) % 3));
        break;

    case WM_SETFOCUS:
        idFocus = id;
        break;
    }
    return CallWindowProc(OldScroll[id], hWnd, message, wParam, lParam);
}
