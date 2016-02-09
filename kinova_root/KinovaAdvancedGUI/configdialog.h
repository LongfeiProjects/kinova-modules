#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "guilogger.h"
#include "sqlmanager.h"

namespace Ui {
class ConfigDialog;
}

#define APPEND_DATA_IN_LOGFILE 1

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = 0, int participantId = 0);
    ~ConfigDialog();

    int getParticipantId();

private slots:
    void on_buttonBox_accepted();

    void on_pushButton_2_clicked();

    void on_generateLog_button_clicked();

    void on_pushButton_clicked();


    void on_joystickModeButton_toggled(bool checked);

    void on_pushButton_3_clicked();

    void on_recordingOption_checkbox_clicked(bool checked);

private:
    Ui::ConfigDialog *ui;
    int participantId;
};

#endif // CONFIGDIALOG_H
