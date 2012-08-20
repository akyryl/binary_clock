#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H


#include "ui_settings.h"
#include <QDialog>


class Clock;


class SettingsDlg : public QDialog , private Ui::SettingsDlg
{
Q_OBJECT
    enum E_SETTINGS_GROUP_INDEX {E_SETTINGS_COLORS, E_SETTINGS_APPEARANCE};
public:
    SettingsDlg( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    ~SettingsDlg();

protected:
    void initData();
    void saveData();

    QColor penColor;
    QColor backBrushColor;
    QColor foreBrushColor;

    Clock* clockPreview;

    void setButtonBackColor(QAbstractButton* btn, const QColor& color);
    const QColor getButtonBackColor(QAbstractButton* btn);

protected slots:
    void on_timeGroupBox_toggled(bool);
    void on_hourCheckBox_toggled(bool);
    void on_minutesCheckBox_toggled(bool);
    void on_secondsCheckBox_toggled(bool);
    void on_dateGroupBox_toggled(bool);
    void on_yearCheckBox_toggled(bool);
    void on_monthCheckBox_toggled(bool);
    void on_dayCheckBox_toggled(bool);
    void on_activeSysColRadioButton_toggled(bool);
    void on_activeCusColRadioButton_toggled(bool);
    void on_chooseColorActiveToolButton_clicked();
    void on_inactiveSysColRadioButton_toggled(bool);
    void on_inactiveCusColRadioButton_toggled(bool);
    void on_chooseColorInactiveToolButton_clicked();
    void on_showGridCheckBox_toggled(bool);
    void on_gridSysColRadioButton_toggled(bool);
    void on_gridCusColRadioButton_toggled(bool);
    void on_chooseColorGridToolButton_clicked();
    void on_buttonBox_clicked(QAbstractButton*);
    void onCurrentItemChanged(QListWidgetItem *, QListWidgetItem *);
};


#endif //SETTINGSDLG_H
