#include "configdialog.h"
#include "ui_configdialog.h"
#include <QMessageBox>
#include <iostream>

using namespace std;

ConfigDialog::ConfigDialog(QWidget *parent, int participantId) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    this->ui->partIdSpinBox->setValue(participantId);
    this->participantId = participantId;
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}


int ConfigDialog::getParticipantId(){
    return this->participantId;
}

void ConfigDialog::on_buttonBox_accepted()
{
    this->participantId = ui->partIdSpinBox->value();
}

void ConfigDialog::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Restart log confirmation"), tr("Are you sure that you want to restart loging data?"),
                                QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
      cout << "Reestarting logs";
      GUILogger::getInstance().clearLogs();
    }
}



void ConfigDialog::on_generateLog_button_clicked()
{
    ostringstream filename;
    filename << "GuiLog_Participant_" <<  this->participantId << ".log";
    string strfile = filename.str();
    GUILogger::getInstance().dumpEvents(strfile,true);
}


