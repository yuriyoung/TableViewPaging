#include "WindowEx.h"
#include "ui_WindowEx.h"

#define RECTWIDTH(rc)           (rc.right - rc.left)
#define RECTHEIGHT(rc)          (rc.bottom - rc.top)
#define BIT_COUNT               32
#define TMT_CAPTIONFONT         1
const wchar_t szWindowClass[] = L"ViewWinClass";
const wchar_t szTitle[] = L"Application";
#define LEFTEXTENDWIDTH         8
#define RIGHTEXTENDWIDTH        8
#define BOTTOMEXTENDWIDTH       20
#define TOPEXTENDWIDTH          33
#define TOPEXTENDWIDTH_ZOOM     33

WindowEx::WindowEx(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowEx)
{
    ui->setupUi(this);
}

WindowEx::~WindowEx()
{
    delete ui;
}

void WindowEx::PaintCustomCaption(HWND hWnd, HDC hdc)
{
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    HTHEME hTheme = OpenThemeData(NULL, L"CompositedWindow::Window");
    if (hTheme)
    {
        HDC hdcPaint = CreateCompatibleDC(hdc);
        if (hdcPaint)
        {
            int cx = RECTWIDTH(rcClient);
            int cy = RECTHEIGHT(rcClient);

            // Define the BITMAPINFO structure used to draw text.
            // Note that biHeight is negative. This is done because
            // DrawThemeTextEx() needs the bitmap to be in top-to-bottom
            // order.
            BITMAPINFO dib = { 0 };
            dib.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
            dib.bmiHeader.biWidth           = cx;
            dib.bmiHeader.biHeight          = -cy;
            dib.bmiHeader.biPlanes          = 1;
            dib.bmiHeader.biBitCount        = BIT_COUNT;
            dib.bmiHeader.biCompression     = BI_RGB;

            HBITMAP hbm = CreateDIBSection(hdc, &dib, DIB_RGB_COLORS, NULL, NULL, 0);
            if (hbm)
            {
                HBITMAP hbmOld = (HBITMAP)SelectObject(hdcPaint, hbm);

                // Setup the theme drawing options.
                DTTOPTS DttOpts = {sizeof(DTTOPTS)};
                DttOpts.dwFlags = DTT_COMPOSITED | DTT_GLOWSIZE;
                DttOpts.iGlowSize = 15;

                // Select a font.
                LOGFONT lgFont;
                HFONT hFontOld = NULL;
                if (SUCCEEDED(GetThemeSysFont(hTheme, TMT_CAPTIONFONT, &lgFont)))
                {
                    HFONT hFont = CreateFontIndirect(&lgFont);
                    hFontOld = (HFONT) SelectObject(hdcPaint, hFont);
                }

                // Draw the title.
                RECT rcPaint = rcClient;
                rcPaint.top += 8;
                rcPaint.right -= 125;
                rcPaint.left += 8;
                rcPaint.bottom = 50;
                DrawThemeTextEx(hTheme,
                                hdcPaint,
                                0, 0,
                                szTitle,
                                -1,
                                DT_LEFT | DT_WORD_ELLIPSIS,
                                &rcPaint,
                                &DttOpts);

                // Blit text to the frame.
                BitBlt(hdc, 0, 0, cx, cy, hdcPaint, 0, 0, SRCCOPY);

                SelectObject(hdcPaint, hbmOld);
                if (hFontOld)
                {
                    SelectObject(hdcPaint, hFontOld);
                }
                DeleteObject(hbm);
            }
            DeleteDC(hdcPaint);
        }
        CloseThemeData(hTheme);
    }
}

LRESULT WindowEx::HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // Get the point coordinates for the hit test.
    POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    // Get the window rectangle.
    RECT rcWindow;
    GetWindowRect(hWnd, &rcWindow);

    // Get the frame rectangle, adjusted for the style without a caption.
    RECT rcFrame = { 0 };
    AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

    // Determine if the hit test is for resizing. Default middle (1,1).
    USHORT uRow = 1;
    USHORT uCol = 1;
    bool fOnResizeBorder = false;

    // Determine if the point is at the top or bottom of the window.
    if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + TOPEXTENDWIDTH)
    {
        fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
        uRow = 0;
    }
    else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - BOTTOMEXTENDWIDTH)
    {
        uRow = 2;
    }

    // Determine if the point is at the left or right of the window.
    if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + LEFTEXTENDWIDTH)
    {
        uCol = 0; // left side
    }
    else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - RIGHTEXTENDWIDTH)
    {
        uCol = 2; // right side
    }

    // Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
    LRESULT hitTests[3][3] =
    {
        { HTTOPLEFT,    fOnResizeBorder ? HTTOP : HTCAPTION,    HTTOPRIGHT },
        { HTLEFT,       HTNOWHERE,     HTRIGHT },
        { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
    };

    return hitTests[uRow][uCol];
}

LRESULT WindowEx::CustomCaptionProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pfCallDWP)
{
    LRESULT lRet = 0;
    HRESULT hr = S_OK;
    bool fCallDWP = true; // Pass on to DefWindowProc?

    fCallDWP = !DwmDefWindowProc(hWnd, message, wParam, lParam, &lRet);
    // Handle window creation.
        if (message == WM_CREATE)
        {
            RECT rcClient;
            GetWindowRect(hWnd, &rcClient);

            // Inform application of the frame change.
            SetWindowPos(hWnd,
                         NULL,
                         rcClient.left, rcClient.top,
                         RECTWIDTH(rcClient), RECTHEIGHT(rcClient),
                         SWP_FRAMECHANGED);

            fCallDWP = true;
            lRet = 0;
        }

        // Handle window activation.
        if (message == WM_ACTIVATE)
        {
            // Extend the frame into the client area.
            MARGINS margins;

            margins.cxLeftWidth = LEFTEXTENDWIDTH;      // 8
            margins.cxRightWidth = RIGHTEXTENDWIDTH;    // 8
            margins.cyBottomHeight = BOTTOMEXTENDWIDTH; // 20
            margins.cyTopHeight = TOPEXTENDWIDTH;       // 27

            hr = DwmExtendFrameIntoClientArea(hWnd, &margins);

            if (!SUCCEEDED(hr))
            {
                // Handle error.
            }

            fCallDWP = true;
            lRet = 0;
        }

        if (message == WM_PAINT)
        {
            PAINTSTRUCT ps;
            HDC hdc;
            {
                hdc = BeginPaint(hWnd, &ps);
                PaintCustomCaption(hWnd, hdc);
                EndPaint(hWnd, &ps);
            }

            fCallDWP = true;
            lRet = 0;
        }

        // Handle the non-client size message.
        if ((message == WM_NCCALCSIZE) && (wParam == TRUE))
        {
            // Calculate new NCCALCSIZE_PARAMS based on custom NCA inset.
            NCCALCSIZE_PARAMS *pncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);

            pncsp->rgrc[0].left   = pncsp->rgrc[0].left   + 0;
            pncsp->rgrc[0].top    = pncsp->rgrc[0].top    + 0;
            pncsp->rgrc[0].right  = pncsp->rgrc[0].right  - 0;
            pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;

            lRet = 0;

            // No need to pass the message on to the DefWindowProc.
            fCallDWP = false;
        }

        // Handle hit testing in the NCA if not handled by DwmDefWindowProc.
        if ((message == WM_NCHITTEST) && (lRet == 0))
        {
            lRet = HitTestNCA(hWnd, wParam, lParam);

            if (lRet != HTNOWHERE)
            {
                fCallDWP = false;
            }
        }

        *pfCallDWP = fCallDWP;

        return lRet;
}


bool WindowEx::winEvent(MSG *msg, long *result)
{
    bool fCallDWP = true;
    BOOL fDwmEnabled = FALSE;
    LRESULT lRet = 0;
    HRESULT hr = S_OK;
    PAINTSTRUCT ps;
    HDC hdc;
    HWND hWnd       = msg->hwnd;
    UINT message    = msg->message;
    WPARAM wParam   = msg->wParam;
    LPARAM lParam   = msg->lParam;

    fCallDWP = !DwmDefWindowProc(hWnd, message, wParam, lParam, &lRet);

    hr = DwmIsCompositionEnabled(&fDwmEnabled);
//    if ( SUCCEEDED(hr) )
    {
        lRet = CustomCaptionProc(hWnd, message, wParam, lParam, &fCallDWP);
    }
    hr = DwmIsCompositionEnabled(&fDwmEnabled);

    if (fCallDWP)
    {
//        lRet = AppWinProc(hWnd, message, wParam, lParam);
    }

    switch(message)
    {
        // 鼠标在窗口边缘时，缩放可用
        case WM_NCHITTEST:
        {

        };
        break;

        case WM_COMMAND:
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            PaintCustomCaption(hWnd, hdc);

            // Add any drawing code here...

            EndPaint(hWnd, &ps);
        };
        break;

        // Handle window activation.
        case WM_ACTIVATE:
        {
            MARGINS margins = { -1, 0, 0, 0 };
            hr = DwmExtendFrameIntoClientArea(hWnd, &margins);
        };
        break;

        // 缩放
        case WM_NCCALCSIZE:
        {

        };
        break;

        //最大化
        case WM_GETMINMAXINFO:
        {

        };
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return lRet;
}

bool WindowEx::nativeEvent(const QByteArray &/*eventType*/, void *message, long *result)
{
    return winEvent((MSG*)message, result);
}
