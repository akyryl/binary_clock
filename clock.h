#ifndef CLOCK_H
#define CLOCK_H


#include <QWidget>
#include <QDateTime>
#include <QBrush>
#include <QPen>
#include <QPaintEvent>
#include <QBitArray>
#include <QTimer>
#include <QMenu>
#include <QSettings>


class SettingsDlg;

class Clock : public QWidget
{
	Q_OBJECT
public:
        Clock(QWidget* parent = 0, Qt::WindowFlags = Qt::FramelessWindowHint);
	~Clock();

        enum TimeSection {NONE_TIME = 0, HOURS = 1, MINUTES = 1 << 1, SECONDS = 1 << 2};
        enum DateSection {NONE_DATE = 0, YEAR = 1, MONTH = 1 << 1, DAY = 1 << 2};

protected:

        // drawing
        QBrush m_backBrush;
        QBrush m_foreBrush;
        QPen m_pen;
        bool m_isShowGrid;
        bool m_isSysPenColor;
        bool m_isSysForeBrushColor;
        bool m_isSysBackBrushColor;

        bool m_isShowTime;
        bool m_isShowDate;

        bool m_isAlwaysOnTop;

        // binary info
        int m_timeSections;
        int m_dateSections;
	QBitArray binaryNumbers[10];
        QDateTime m_currentDateTime;
	QTimer timer;
        QVector <int> timeNumbers;
        QVector <int> dateNumbers;
        int m_updateInterval;
        float m_bitsCount;

        // size/pos
        QSize m_bitSize;
        QPoint m_globalPos;
        // needs for moving
        QPoint m_leftButtonPressPoint;

        // menu
	QMenu *contextMenu;
	QAction* settingsAct;
        QAction* aboutAct;
	QAction* exitAct;
	QAction* alwaysOnTopAct;

        Qt::WindowFlags flags;

        QSettings m_settings;
	SettingsDlg *settingsDlg;

        // methods
        void init();
        void initMenu();
        QBitArray intToBin(unsigned int i);
        void drawGrid(QPainter &painter, const QRectF &rect);
        void resizeClock();

        void readSettings();
        void saveSettings();

        int getTimeDateItemsCount();

        // events
        void paintEvent(QPaintEvent * event);
        void contextMenuEvent(QContextMenuEvent * event);
        void mouseMoveEvent(QMouseEvent * event);
        void mousePressEvent(QMouseEvent * event);
        void mouseDoubleClickEvent(QMouseEvent * event);

protected slots:
	void timerEvent();
	void openSettingsDialog();
        void setAlwaysOnTopSlot();
        void showAbout();

public:
	QColor penColor();
	QColor backBrushColor();
	QColor foreBrushColor();

        void setPenColor(const QColor &);
        void setBackBrushColor(const QColor &);
        void setForeBrushColor(const QColor &);
        QIcon icon();

        void setAlwaysOnTop();

	bool isSysPenColor();
	bool isSysBackBrushColor();
	bool isSysForeBrushColor();
        bool isShowGrid();
        bool isShowTime();
        bool isShowDate();
        int getTimeSections();
        int getDateSections();
        const QSize bitSize();

	void setSysPenColor(bool);
	void setSysBackBrushColor(bool);
	void setSysForeBrushColor(bool);
        void setIsShowGrid(bool);
        void setIsShowDateTime(bool isDateVisible, bool isTimeVisible);
        void setDateTimeSections(int dateSections, int timeSections);
        void setBitSize(const QSize&);
};


#endif // CLOCK_H
