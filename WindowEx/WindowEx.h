#ifndef WINDOWEX_H
#define WINDOWEX_H

#include <QMainWindow>
#include "Windows.h"
#include "WinUser.h"
#include "windowsx.h"
#include "dwmapi.h"
#include <Uxtheme.h>

namespace Ui {
class WindowEx;
}

class WindowEx : public QMainWindow
{
    Q_OBJECT

public:
    explicit WindowEx(QWidget *parent = 0);
    ~WindowEx();
private:
    LRESULT HitTestNCA(HWND hWnd, WPARAM wParam, LPARAM lParam);
    LRESULT CustomCaptionProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool* pfCallDWP);
    void PaintCustomCaption(HWND hWnd, HDC hdc);

protected:
    bool winEvent(MSG *msg, long *result);
    bool nativeEvent(const QByteArray &, void *message, long *result);

private:
    Ui::WindowEx *ui;
};

#endif // WINDOWEX_H
