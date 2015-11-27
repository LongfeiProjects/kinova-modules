#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include "guilogger.h"

namespace Ui {
class ConfigDialog;
}

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

private:
    Ui::ConfigDialog *ui;
    int participantId;
};

#endif // CONFIGDIALOG_H
