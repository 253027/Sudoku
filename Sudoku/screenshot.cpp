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
    painter.setOpacity(0.5);    //设置透明度
    painter.fillRect(rect(), QColor(0, 0, 0));    //填充整个窗口界面
    painter.setCompositionMode(QPainter::CompositionMode_Clear);    //设置清除模式以便后序选择的截屏区域能够显示原图
    painter.fillRect(QRect(start_pos, end_pos), Qt::transparent);    //设置选中区域
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);    //恢复原绘图模式
    painter.setPen(Qt::green);    //设置选中区域颜色
    painter.drawRect(QRect(start_pos, end_pos));  //设置选中区域  
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
