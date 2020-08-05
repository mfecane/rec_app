#include "lib.h"

#include <string.h>
#include <stdio.h>
#include <psapi.h>

#include <QFile>
#include <QDateTime>
#include <QFileInfo>

QString int00(int i)
{
    QString s;
    s = QString::number(i);
    if(s.length() < 2) {
        s = QString("0") + s.right(1);
        return s;
    } else {
        return s;
    }
}

QString format_time(qint64 t)
{
    int sec, min, hour;
    sec = floor(t / 1000);
    min = floor(sec / 60);
    sec = sec % 60;
    hour = floor(min / 60);
    min = min % 60;
    return  QString("%1:%2:%3").arg(int00(hour)).arg(int00(min)).arg(int00(sec));
}

const char* C_string(QString string) {
    QByteArray ba;
    ba = string.toLocal8Bit();
    return ba.constData();
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    break;
    }
    QFile outFile("C:/Temp/log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << " ";
    ts << txt << '\n';
    ts.flush();
    outFile.close();
}

BOOL getWindowInfo(HWND hwnd, QString& title, QString& exeName, bool& is64)
{
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];
    int res1 = ::GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);
    if(res1 != 0)
    {
        title = QString::fromWCharArray(windowTitle);
    } else {
        title = QStringLiteral("");
        return FALSE;
    }

    title = QString::fromWCharArray(windowTitle);

    WCHAR fileName[MAX_PATH];
    DWORD dwPID;
    ::GetWindowThreadProcessId(hwnd, &dwPID);
    HANDLE hproc = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
    if(!hproc) {
        return FALSE;
    }
    //    throw std::exception("[ TASK FAILED SUCCESSFULLY ] Unable to read process.");

    DWORD res2 = ::GetModuleFileNameEx(hproc, 0, fileName, MAX_PATH);
    if(res2 == 0)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Unable to read process name.");

    exeName = QFileInfo(QString::fromWCharArray(fileName)).fileName();

    BOOL isWow64;
    BOOL res3 = IsWow64Process(hproc, &isWow64);
    if(!res3)
        throw std::exception("[ TASK FAILED SUCCESSFULLY ] Could not get determie process bitness");

    is64 = !isWow64;

    return TRUE;
}
