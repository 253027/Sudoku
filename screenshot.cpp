#include "screenshot.h"
#include <QDebug>

ScreenShot::ScreenShot(QWidget *parent) : QWidget(parent)
{
    iscapturing = false;
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setWindowOpacity(0.3);
}

void ScreenShot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if(!iscapturing)
        return;
    QPainter painter(this);
    painter.setOpacity(0.5);
    painter.fillRect(rect(), QColor(0, 0, 0));
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(QRect(start_pos, end_pos), Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setPen(Qt::green);
    painter.drawRect(QRect(start_pos, end_pos));
}

void ScreenShot::mousePressEvent(QMouseEvent *event)
{
    if(iscapturing || event->button() == Qt::RightButton)
        return;
    iscapturing = true;
    start_pos = end_pos = event->globalPos();
    update();
}

void ScreenShot::mouseMoveEvent(QMouseEvent *event)
{
    if(!iscapturing)
        return;
    end_pos = event->globalPos();
    update();
}

void ScreenShot::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        close();
        return;
    }
    if(!iscapturing)
        return;
    end_pos = event->globalPos();
    close(), iscapturing = false;
}

void ScreenShot::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
        close();
}

void ScreenShot::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    setWindowOpacity(0);
    QRect area(start_pos, end_pos);
    QImage temp = QGuiApplication::primaryScreen()->grabWindow(0, area.x(), area.y(), area.width(), area.height()).toImage();
    if(temp.isNull())
        return;
    emit emitScreenShot(temp);
}

void ScreenShot::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    setWindowOpacity(0.3);
}
