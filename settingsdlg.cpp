#include "settingsdlg.h"
#include "clock.h"

#include <QDebug>
#include <QPushButton>
#include <QColorDialog>


SettingsDlg::SettingsDlg(QWidget * parent, Qt::WindowFlags f) : QDialog(parent, f)
{
    setupUi(this);

    QIcon i;
    QListWidgetItem *pItem = new QListWidgetItem(QIcon(":/images/colors.png"), QString("Colors"));
    contentsListWidget->insertItem(E_SETTINGS_COLORS, pItem);
    pItem = new QListWidgetItem(QIcon(":/images/view.png"), QString("View"));
    contentsListWidget->insertItem(E_SETTINGS_APPEARANCE, pItem);
    contentsListWidget->setCurrentRow(E_SETTINGS_COLORS);
    contentsListWidget->setIconSize(QSize(64, 64));

    connect(contentsListWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(onCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    initData();
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::initData()
{
    Clock* clock = static_cast <Clock*> (this->parent());
    // colors
    penColor = clock->penColor();
    foreBrushColor = clock->foreBrushColor();
    backBrushColor = clock->backBrushColor();

    setButtonBackColor(chooseColorGridToolButton, penColor);
    setButtonBackColor(chooseColorActiveToolButton, foreBrushColor);
    setButtonBackColor(chooseColorInactiveToolButton, backBrushColor);

    if (clock->isSysForeBrushColor()) {
        activeSysColRadioButton->setChecked(true);
        chooseColorActiveToolButton->setEnabled(false);
    } else {
        activeCusColRadioButton->setChecked(true);
        chooseColorActiveToolButton->setEnabled(true);
    }
    ActiveLedsOpacityHS->setValue(foreBrushColor.alpha());
    if (clock->isSysBackBrushColor()) {
        inactiveSysColRadioButton->setChecked(true);
        chooseColorInactiveToolButton->setEnabled(false);
    } else {
        inactiveCusColRadioButton->setChecked(true);
        chooseColorInactiveToolButton->setEnabled(true);
    }
    InactiveLedsOpacityHS->setValue(backBrushColor.alpha());
    if (clock->isSysPenColor()) {
        gridSysColRadioButton->setChecked(true);
        chooseColorGridToolButton->setEnabled(false);
    } else {
        gridCusColRadioButton->setChecked(true);
        chooseColorGridToolButton->setEnabled(true);
    }

    showGridCheckBox->setChecked(clock->isShowGrid());

    // appearance
    widthSpinBox->setValue(clock->bitSize().width());
    heightSpinBox->setValue(clock->bitSize().height());
    timeGroupBox->setChecked(clock->isShowTime());
    dateGroupBox->setChecked(clock->isShowDate());

    hourCheckBox->setChecked(clock->getTimeSections() & Clock::HOURS);
    minutesCheckBox->setChecked(clock->getTimeSections() & Clock::MINUTES);
    secondsCheckBox->setChecked(clock->getTimeSections() & Clock::SECONDS);

    yearCheckBox->setChecked(clock->getDateSections() & Clock::YEAR);
    monthCheckBox->setChecked(clock->getDateSections() & Clock::MONTH);
    dayCheckBox->setChecked(clock->getDateSections() & Clock::DAY);
}

void SettingsDlg::saveData()
{
    // colors
    if (activeSysColRadioButton->isChecked()) {
        foreBrushColor = QApplication::palette().color(QPalette::Base);
    } else {
        foreBrushColor = getButtonBackColor(chooseColorActiveToolButton);
    }
    foreBrushColor.setAlpha(ActiveLedsOpacityHS->value());
    if (inactiveSysColRadioButton->isChecked()) {
        backBrushColor = QApplication::palette().color(QPalette::WindowText);
    } else {
        backBrushColor = getButtonBackColor(chooseColorInactiveToolButton);
    }
    backBrushColor.setAlpha(InactiveLedsOpacityHS->value());
    if (gridSysColRadioButton->isChecked()) {
        penColor = QApplication::palette().color(QPalette::Mid);
    } else {
        penColor = getButtonBackColor(chooseColorGridToolButton);
    }
    Clock* clock = static_cast <Clock*> (this->parent());
    clock->setForeBrushColor(foreBrushColor);
    clock->setBackBrushColor(backBrushColor);
    clock->setPenColor(penColor);

    clock->setIsShowGrid(showGridCheckBox->isChecked());
    clock->setSysForeBrushColor(activeSysColRadioButton->isChecked());
    clock->setSysBackBrushColor(inactiveSysColRadioButton->isChecked());
    clock->setSysPenColor(gridSysColRadioButton->isChecked());

    // appearance
    clock->setBitSize(QSize(widthSpinBox->value(), heightSpinBox->value()));
    clock->setIsShowDateTime(dateGroupBox->isCheckable() ? dateGroupBox->isChecked() : true
                             , timeGroupBox->isCheckable() ? timeGroupBox->isChecked() : true);

    // Clock::TimeSections timeSections;
    int timeSections = 0;
    hourCheckBox->isChecked() ? timeSections |= Clock::HOURS : timeSections;
    minutesCheckBox->isChecked() ? timeSections |= Clock::MINUTES : timeSections;
    secondsCheckBox->isChecked() ? timeSections |= Clock::SECONDS : timeSections;

    // Clock::DateSections dateSections;
    int dateSections = 0;
    yearCheckBox->isChecked() ? dateSections |= Clock::YEAR : dateSections;
    monthCheckBox->isChecked() ? dateSections |= Clock::MONTH : dateSections;
    dayCheckBox->isChecked() ? dateSections |= Clock::DAY : dateSections;

    clock->setDateTimeSections(dateSections, timeSections);
}

void SettingsDlg::onCurrentItemChanged(QListWidgetItem *, QListWidgetItem *)
{
    stackedWidget->setCurrentIndex(this->contentsListWidget->currentRow());
}

// appearance

void SettingsDlg::on_timeGroupBox_toggled(bool value)
{
    hourCheckBox->setEnabled(value);
    minutesCheckBox->setEnabled(value);
    secondsCheckBox->setEnabled(value);
    dateGroupBox->setCheckable(value);
}

void SettingsDlg::on_hourCheckBox_toggled(bool val)
{
    if (!minutesCheckBox->isChecked() && !secondsCheckBox->isChecked() && val == false)
    {
        hourCheckBox->setChecked(true);
    }
}

void SettingsDlg::on_minutesCheckBox_toggled(bool val)
{
    if (!hourCheckBox->isChecked() && !secondsCheckBox->isChecked() && val == false)
    {
        minutesCheckBox->setChecked(true);
    }
}

void SettingsDlg::on_secondsCheckBox_toggled(bool val)
{
    if (!hourCheckBox->isChecked() && !minutesCheckBox->isChecked() && val == false)
    {
        secondsCheckBox->setChecked(true);
    }
}

void SettingsDlg::on_dateGroupBox_toggled(bool value)
{
    yearCheckBox->setEnabled(value);
    monthCheckBox->setEnabled(value);
    dayCheckBox->setEnabled(value);
    timeGroupBox->setCheckable(value);
}

void SettingsDlg::on_yearCheckBox_toggled(bool val)
{
    if (!dayCheckBox->isChecked() && !monthCheckBox->isChecked() && val == false)
    {
        yearCheckBox->setChecked(true);
    }
}

void SettingsDlg::on_monthCheckBox_toggled(bool val)
{
    if (!dayCheckBox->isChecked() && !yearCheckBox->isChecked() && val == false)
    {
        monthCheckBox->setChecked(true);
    }
}

void SettingsDlg::on_dayCheckBox_toggled(bool val)
{
    if (!yearCheckBox->isChecked() && !monthCheckBox->isChecked() && val == false)
    {
        dayCheckBox->setChecked(true);
    }
}

// colors

void SettingsDlg::on_activeSysColRadioButton_toggled(bool)
{
    chooseColorActiveToolButton->setEnabled(activeCusColRadioButton->isChecked());
}
void SettingsDlg::on_activeCusColRadioButton_toggled(bool)
{
    chooseColorActiveToolButton->setEnabled(activeCusColRadioButton->isChecked());
}

void SettingsDlg::on_inactiveSysColRadioButton_toggled(bool)
{
    chooseColorInactiveToolButton->setEnabled(inactiveCusColRadioButton->isChecked());
}
void SettingsDlg::on_inactiveCusColRadioButton_toggled(bool)
{
    chooseColorInactiveToolButton->setEnabled(inactiveCusColRadioButton->isChecked());
}

void SettingsDlg::on_gridSysColRadioButton_toggled(bool)
{
    chooseColorGridToolButton->setEnabled(gridCusColRadioButton->isChecked());
}
void SettingsDlg::on_gridCusColRadioButton_toggled(bool)
{
    chooseColorGridToolButton->setEnabled(gridCusColRadioButton->isChecked());
}
void SettingsDlg::on_showGridCheckBox_toggled(bool)
{
    bool val = showGridCheckBox->isChecked();
    gridSysColRadioButton->setEnabled(val);
    gridCusColRadioButton->setEnabled(val);
    chooseColorGridToolButton->setEnabled(val);
}

// tool buttons
void SettingsDlg::on_chooseColorActiveToolButton_clicked()
{
    foreBrushColor = QColorDialog::getColor(penColor, static_cast <QWidget*> (this->parent()));
    setButtonBackColor(chooseColorActiveToolButton, foreBrushColor);
}
void SettingsDlg::on_chooseColorInactiveToolButton_clicked()
{
    backBrushColor = QColorDialog::getColor(foreBrushColor, static_cast <QWidget*> (this->parent()));
    setButtonBackColor(chooseColorInactiveToolButton, backBrushColor);
}
void SettingsDlg::on_chooseColorGridToolButton_clicked()
{
    penColor = QColorDialog::getColor(backBrushColor, static_cast <QWidget*> (this->parent()));
    setButtonBackColor(chooseColorGridToolButton, penColor);
}

void SettingsDlg::on_buttonBox_clicked(QAbstractButton *btn)
{
    if (buttonBox->buttonRole(btn) == QDialogButtonBox::AcceptRole)
    {
        saveData();
    }
}

// methods

void SettingsDlg::setButtonBackColor(QAbstractButton* btn, const QColor& color)
{
    QPalette p(btn->palette());
    p.setColor(QPalette::Button, color);
    btn->setPalette(p);
}

const QColor SettingsDlg::getButtonBackColor(QAbstractButton* btn)
{
    QPalette p(btn->palette());
    return p.color(QPalette::Button);
}
