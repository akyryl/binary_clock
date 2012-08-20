#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QMessageBox>

#include "clock.h"
#include "settingsdlg.h"


Clock::Clock(QWidget* parent, Qt::WindowFlags f) : QWidget(parent, f)
        , flags(f)
{
    flags |= Qt::WindowStaysOnTopHint;
    flags |= Qt::X11BypassWindowManagerHint;
    setAttribute(Qt::WA_TranslucentBackground);
    init();
}

Clock::~Clock()
{
    m_globalPos.setX(geometry().x());
    m_globalPos.setY(geometry().y());
    saveSettings();
}

void Clock::init()
{
    setWindowIcon(QIcon(":/images/colors.png"));

    readSettings();
    m_backBrush.setStyle(Qt::SolidPattern);
    m_foreBrush.setStyle(Qt::SolidPattern);
    for (int i = 0; i < 10; ++i)
    {
        binaryNumbers[i] = intToBin(i);
    }
    setDateTimeSections(m_dateSections, m_timeSections);

    timer.setInterval(m_updateInterval);
    timerEvent();
    timer.start();
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

    setMouseTracking(true);

    initMenu();
    settingsDlg = 0;

    alwaysOnTopAct->setChecked(m_isAlwaysOnTop);
    setAlwaysOnTop();
}

void Clock::readSettings()
{
    QSettings settings(QString("QtApps"), QString("BinaryClock"));
    settings.beginGroup("Geometry");
    m_pen.setWidth(settings.value("PenWidth", 1).toInt());
    m_bitsCount = (settings.value("BitsCount", 4).toDouble());
    m_bitSize = settings.value("BitSize", QSize(15, 12)).toSize();
    m_globalPos = settings.value("ScreenPos", QPoint(100, 100)).toPoint();
    settings.endGroup();

    settings.beginGroup("Appearance");
    m_isShowGrid = settings.value("IsShowGrid", true).toBool();
    m_isSysForeBrushColor = settings.value("IsSysForeColor", true).toBool();
    m_isSysBackBrushColor = settings.value("IsSysBackColor", true).toBool();
    m_isSysPenColor = settings.value("IsSysGridColor", true).toBool();
    m_updateInterval = settings.value("UpdateInterval", 1000).toInt();
    m_isShowTime = settings.value("IsShowTime", true).toBool();
    m_isShowDate = settings.value("IsShowDate", false).toBool();
    m_timeSections = settings.value("TimeSections", HOURS | MINUTES | SECONDS).toInt();
    m_dateSections = settings.value("DateSections", YEAR | MONTH | DAY).toInt();
    m_isAlwaysOnTop = settings.value("IsAlwaysOnTop", false).toBool();
    settings.endGroup();

    settings.beginGroup("Colors");
    QColor color = QApplication::palette().color(QPalette::WindowText);
    color.setAlpha(0);
    m_backBrush.setColor(settings.value("BackBrushColor", color).value<QColor>());
    m_foreBrush.setColor(settings.value("ForeBrushColor", QApplication::palette().color(QPalette::Base)).value<QColor>());
    m_pen.setColor(settings.value("PenColor", QApplication::palette().color(QPalette::Mid)).value<QColor>());

    settings.endGroup();
}

void Clock::saveSettings()
{
    QSettings settings(QString("QtApps"), QString("BinaryClock"));
    settings.beginGroup("Geometry");
    settings.setValue("PenWidth", m_pen.width());
    settings.setValue("BitsCount", m_bitsCount);
    settings.setValue("BitSize", m_bitSize);
    settings.setValue("ScreenPos", m_globalPos);
    settings.endGroup();

    settings.beginGroup("Appearance");
    settings.setValue("IsShowGrid", m_isShowGrid);
    settings.setValue("IsSysForeColor", m_isSysForeBrushColor);
    settings.setValue("IsSysBackColor", m_isSysBackBrushColor);
    settings.setValue("IsSysGridColor", m_isSysPenColor);
    settings.setValue("UpdateInterval", m_updateInterval);
    settings.setValue("IsShowTime", m_isShowTime);
    settings.setValue("IsShowDate", m_isShowDate);
    settings.setValue("TimeSections", static_cast <int> (m_timeSections));
    settings.setValue("DateSections", static_cast <int> (m_dateSections));
    settings.setValue("IsAlwaysOnTop", m_isAlwaysOnTop);
    settings.endGroup();

    settings.beginGroup("Colors");
    settings.setValue("BackBrushColor", m_backBrush);
    settings.setValue("ForeBrushColor", m_foreBrush);
    settings.setValue("PenColor", m_pen.color());
    settings.endGroup();
}

void Clock::initMenu()
{
    alwaysOnTopAct = new QAction(tr("Always On Top"), this);
    alwaysOnTopAct->setStatusTip(tr("Stay on top of all other windows"));
    alwaysOnTopAct->setCheckable(true);
    connect(alwaysOnTopAct, SIGNAL(triggered()), this, SLOT(setAlwaysOnTopSlot()));

    settingsAct = new QAction(tr("Settings"), this);
    settingsAct->setShortcut(tr("Dbl Click"));
    settingsAct->setStatusTip(tr("Binary Clock Settings"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    aboutAct = new QAction(tr("About"), this);
    aboutAct->setStatusTip(tr("About Binary Clock"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(showAbout()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(tr("Alt+F4"));
    exitAct->setStatusTip(tr("Exit the binary clock"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    contextMenu = new QMenu();
    contextMenu->addAction(alwaysOnTopAct);
    contextMenu->addSeparator();
    contextMenu->addAction(settingsAct);
    contextMenu->addAction(aboutAct);
    contextMenu->addSeparator();
    contextMenu->addAction(exitAct);
}

// events

void Clock::timerEvent()
{
    m_currentDateTime = QDateTime::currentDateTime();
    int index = 0;
    if (m_timeSections & HOURS)
    {
        timeNumbers[index] = m_currentDateTime.time().hour() / 10;
        index++;
        timeNumbers[index] = m_currentDateTime.time().hour() % 10;
        index++;
    }
    if (m_timeSections & MINUTES)
    {
        timeNumbers[index] = m_currentDateTime.time().minute() / 10;
        index++;
        timeNumbers[index] = m_currentDateTime.time().minute() % 10;
        index++;
    }
    if (m_timeSections & SECONDS)
    {
        timeNumbers[index] = m_currentDateTime.time().second() / 10;
        index++;
        timeNumbers[index] = m_currentDateTime.time().second() % 10;
        index++;
    }

    index = 0;
    if (m_dateSections & MONTH)
    {
        dateNumbers[index] = m_currentDateTime.date().month() / 10;
        index++;
        dateNumbers[index] = m_currentDateTime.date().month() % 10;
        index++;
    }
    if (m_dateSections & DAY)
    {
        dateNumbers[index] = m_currentDateTime.date().day() / 10;
        index++;
        dateNumbers[index] = m_currentDateTime.date().day() % 10;
        index++;
    }
    if (m_dateSections & YEAR)
    {
        dateNumbers[index] = m_currentDateTime.date().year() / 1000;
        index++;
        dateNumbers[index] = (m_currentDateTime.date().year() / 100) % 10;
        index++;
        dateNumbers[index] = (m_currentDateTime.date().year() / 10) % 10;
        index++;
        dateNumbers[index] = m_currentDateTime.date().year() % 10;
        index++;
    }
    this->update();
}

void Clock::mouseMoveEvent(QMouseEvent * event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        m_globalPos.setX(event->globalPos().x() - m_leftButtonPressPoint.x());
        m_globalPos.setY(event->globalPos().y() - m_leftButtonPressPoint.y());
        setGeometry(m_globalPos.x(),m_globalPos.y() , geometry().width(), geometry().height());
    }
}

void Clock::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        m_leftButtonPressPoint = event->pos();
    }
}

void Clock::mouseDoubleClickEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        openSettingsDialog();
    }
}

void Clock::contextMenuEvent (QContextMenuEvent * event)
{
    contextMenu->exec(event->globalPos());
}

void Clock::paintEvent(QPaintEvent */*event*/)
{
    if (!timer.isActive())
    {
        return;
    }

    QRectF rect(this->rect());
    QPainter painter(this);
    painter.setBrush(m_backBrush);
    painter.setPen(m_pen);

    float parts = timeNumbers.size();
    float x = 0;
    float y = 0;

    // draw time
    if (m_isShowTime)
    {
        for (int j = 0; j < parts; ++j)
        {
            for (int i = 0; i < m_bitsCount; ++i)
            {
                if (binaryNumbers[timeNumbers[j]][i] == true)
                {
                    painter.fillRect(x, rect.height() - y - m_bitSize.height(), m_bitSize.width(), m_bitSize.height(), m_foreBrush);
                }
                else
                {
                    painter.fillRect(x, rect.height() - y - m_bitSize.height(), m_bitSize.width(), m_bitSize.height(), m_backBrush);
                }
                y += m_bitSize.height();
            }
            x += m_bitSize.width();
            y = 0;
        }
    }
    if (m_isShowTime && m_isShowDate)
    {
        // space between time and date
        x += m_bitSize.width();
    }
    if (m_isShowDate)
    {
        // draw date
        float dateParts = dateNumbers.size();
        for (int j = 0; j < dateParts; ++j)
        {
            for (int i = 0; i < m_bitsCount; ++i)
            {
                if (binaryNumbers[dateNumbers[j]][i] == true)
                {
                    painter.fillRect(x, rect.height() - y - m_bitSize.height(), m_bitSize.width(), m_bitSize.height(), m_foreBrush);
                }
                else
                {
                    painter.fillRect(x, rect.height() - y - m_bitSize.height(), m_bitSize.width(), m_bitSize.height(), m_backBrush);
                }
                y += m_bitSize.height();
            }
            x += m_bitSize.width();
            y = 0;
        }
    }

    if (m_isShowGrid)
    {
        // draw grid
        drawGrid(painter, rect);
    }
}

// protected methods

void Clock::drawGrid(QPainter &painter, const QRectF &rect)
{
    float parts = getTimeDateItemsCount();
    float x = 0;
    for (int i = 0; i < parts; ++i)
    {
        painter.drawLine(QPointF(x, 0), QPointF(x, rect.height()));
        x += m_bitSize.width();
    }
    painter.drawLine(QPointF(x, 0), QPointF(x, rect.height()));
    float y = 0;
    parts = 0;
    if (m_isShowTime) // time horizontal lines
    {
        parts = timeNumbers.size();
        for (int i = 0; i < m_bitsCount; ++i)
        {
            if (i)
            {
                painter.drawLine(QPointF(0, y), QPointF(m_bitSize.width() * parts, y));
            }
            else
            {
                painter.drawLine(QPointF(0, y), QPointF(rect.width(), y));
            }
            y += m_bitSize.height();
        }
        if (m_isShowDate) {
            ++parts;
        }
    }
    if (m_isShowDate) // date horizontal lines
    {
        y = 0;
        for (int i = 0; i < m_bitsCount; ++i)
        {
            painter.drawLine(QPointF(m_bitSize.width() * parts, y), QPointF(rect.width(), y));
            y += m_bitSize.height();
        }
    }
    painter.drawLine(QPointF(0, y), QPointF(rect.width(), y));
}

QBitArray Clock::intToBin(unsigned int i)
{
    QBitArray res(m_bitsCount, false);
    int pos = 0;
    while (i)
    {
        res[pos] = i % 2;
        i /= 2;
        pos++;
    }
    return res;
}

void Clock::openSettingsDialog()
{
    if (settingsDlg == 0)
        settingsDlg = new SettingsDlg(this);
    settingsDlg->exec();
}

void Clock::setAlwaysOnTopSlot()
{
    m_isAlwaysOnTop = alwaysOnTopAct->isChecked();
    setAlwaysOnTop();
}

void Clock::setAlwaysOnTop()
{
    if (m_isAlwaysOnTop)
    {
        flags |= Qt::WindowStaysOnTopHint;
        flags |= Qt::X11BypassWindowManagerHint;
    }
    else
    {
        flags ^= Qt::WindowStaysOnTopHint;
        flags ^= Qt::X11BypassWindowManagerHint;
    }
    QWidget::setWindowFlags(flags);
    // restore position
    QPoint p = pos();
    if (p.x() < 0)
            p.setX(0);
    if (p.y() < 0)
            p.setY(0);
    move(p);
    show();
}

void Clock::showAbout()
{
    QMessageBox::about(this, "About","<b>Binary Clock</b><br />version 0.1.0<br /><br />Anatolii Kyryliuk");
}

void Clock::resizeClock()
{
    setGeometry(m_globalPos.x()
            , m_globalPos.y()
            , m_bitSize.width() * getTimeDateItemsCount() + (m_isShowGrid ? m_pen.width() : 0)
            , m_bitSize.height() * m_bitsCount + (m_isShowGrid ? m_pen.width() : 0));
}

int Clock::getTimeDateItemsCount()
{
    int timeUnitsCount = 0;
    int dateUnitsCount = 0;
    if (m_isShowTime)
    {
        timeUnitsCount = timeNumbers.size();
        if (m_isShowDate)
        {
            // space between time and date
            ++timeUnitsCount;
        }
    }
    if (m_isShowDate)
    {
        dateUnitsCount = dateNumbers.size();
    }
    return timeUnitsCount + dateUnitsCount;
}

QIcon Clock::icon()
{
    return QIcon(":/images/colors.png");
};

// public methods

void Clock::setPenColor(const QColor &color)
{
    m_pen.setColor(color);
}
void Clock::setBackBrushColor(const QColor &color)
{
    m_backBrush.setColor(color);
}
void Clock::setForeBrushColor(const QColor &color)
{
    m_foreBrush.setColor(color);
}

QColor Clock::penColor()
{
    return m_pen.color();
}
QColor Clock::backBrushColor()
{
    return m_backBrush.color();
}
QColor Clock::foreBrushColor()
{
    return m_foreBrush.color();
}

bool Clock::isSysPenColor()
{
    return this->m_isSysPenColor;
}
bool Clock::isSysBackBrushColor()
{
    return this->m_isSysBackBrushColor;
}
bool Clock::isSysForeBrushColor()
{
    return m_isSysForeBrushColor;
}
bool Clock::isShowGrid()
{
    return m_isShowGrid;
}
bool Clock::isShowTime()
{
    return m_isShowTime;
}
bool Clock::isShowDate()
{
    return m_isShowDate;
}
int Clock::getTimeSections()
{
    return m_timeSections;
}
int Clock::getDateSections()
{
    return m_dateSections;
}

void Clock::setSysPenColor(bool isSys)
{
    m_isSysPenColor = isSys;
}
void Clock::setSysBackBrushColor(bool isSys)
{
    m_isSysBackBrushColor = isSys;
}
void Clock::setSysForeBrushColor(bool isSys)
{
    m_isSysForeBrushColor = isSys;
}
void Clock::setIsShowGrid(bool isShowGrid)
{
    m_isShowGrid = isShowGrid;
}
void Clock::setIsShowDateTime(bool isDateVisible, bool isTimeVisible)
{
    m_isShowDate = isDateVisible;
    m_isShowTime = isTimeVisible;
    resizeClock();
}
void Clock::setDateTimeSections(int dateSections, int timeSections)
{
    // --TODO: add locale
    m_dateSections = dateSections;
    int count = 0;
    m_dateSections & MONTH ? count += 2 : count;
    m_dateSections & DAY ? count += 2 : count;
    m_dateSections & YEAR ? count += 4 : count;
    dateNumbers.resize(count);

    m_timeSections = timeSections;
    count = 0;
    m_timeSections & HOURS ? count += 2 : count;
    m_timeSections & MINUTES ? count += 2 : count;
    m_timeSections & SECONDS ? count += 2 : count;
    timeNumbers.resize(count);
    resizeClock();
}
const QSize Clock::bitSize()
{
    return m_bitSize;
}
void Clock::setBitSize(const QSize& size)
{
    m_bitSize = size;
    resizeClock();
}
