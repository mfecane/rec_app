#include "capturethread.h"
#include "VideoCapture.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QWindow>
#include <QDebug>
#include <QPainter>

#include "backend.h"
#include "mousehook.h"

extern QWindow* windowRef;

CaptureThread::CaptureThread(int shots_per_second):
    m_stop (false),
    m_pause (false),
    m_shots_per_second (shots_per_second)
{
    screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();

    m_shots_per_second = 12;

    m_recMode = BackEnd::getInstance()->recordMode();
    m_hwnd = BackEnd::getInstance()->getHwnd();

    if(m_recMode == RECORD_MODE::Window) {
        qDebug() << "Installing mouse hook";
        InstallMouseHook((HWND)windowRef->winId(), m_hwnd);
    }

    m_timer.start();
}

CaptureThread::~CaptureThread()
{
    RemoveHooks();
}

void CaptureThread::run() {

    int w, h, fps;
    const char* fpussy;

    w = BackEnd::getInstance()->outWidth();
    h = BackEnd::getInstance()->outHeight();
    fps = BackEnd::getInstance()->framesPerSecond();
    QByteArray ba = BackEnd::getInstance()->outFileName().toLatin1();
    fpussy = ba.data();

    VideoCapture vc;
    vc.Init(w, h, fps, 2500, fpussy);
    while (true) {
        while(m_pause) {
            QThread::msleep(100);
        }
        //        qDebug() << "record mode:" << BackEnd::getInstance()->recordMode() << endl;

        if(m_recMode == RECORD_MODE::Window) {
            if (m_timer.elapsed() < 2000) {
                vc.AddFrame(CaptureWindow());
            }
            else {
                qDebug() << "Sleeping...." ;
            }
        }
        else {
            vc.AddFrame(CaptureScreen());
        }
        if(m_stop) {
            break;
        }
        ulong sleeptimeout = static_cast<ulong>(1000/m_shots_per_second);
        QThread::msleep(sleeptimeout);
    }
    vc.Finish();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen() {
    QPixmap pixmap = screen->grabWindow(0);
    QPixmap pixmap_cursor(":/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    QImage image (pixmap.toImage());
    return image;
}

QImage CaptureThread::CaptureWindow() {
    QPixmap pixmap = screen->grabWindow((WId)m_hwnd);
    QPixmap pixmap_cursor(":/cursor.png");
    QPainter painter(&pixmap);
    QPoint p = QCursor::pos();
    painter.drawPixmap(p,pixmap_cursor);
    QImage image (pixmap.toImage());
    return image;
}
