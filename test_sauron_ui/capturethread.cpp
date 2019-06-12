#include "capturethread.h"
#include "VideoCapture.h"

#include <QGuiApplication>
#include <QWindow>

extern QWindow* windowRef;

CaptureThread::CaptureThread():
    m_stop (false),
    m_pause (false)
{
    screen = QGuiApplication::primaryScreen();
    if (windowRef)
        screen = windowRef->screen();

    shots_per_second = 12;
}

void CaptureThread::run() {
    VideoCapture vc;
    vc.Init(352,288,25,2500);
    while (true) {

        while(m_pause) {
          QThread::msleep(100);
        }

        vc.AddFrame(CaptureScreen());

        if(m_stop) {
            break;
        }

        QThread::msleep((int)1000/shots_per_second);
    }
    vc.Finish();
    emit CaptureThread::resultReady();
}

QImage CaptureThread::CaptureScreen() {
    QPixmap pixmap = screen->grabWindow(0);
    QImage image (pixmap.toImage());
    return image;
}