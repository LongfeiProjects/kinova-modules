#include "dialogtaskfinished.h"
#include "ui_dialogtaskfinished.h"

DialogTaskFinished::DialogTaskFinished(QWidget *parent,string taskName) :
    QDialog(parent),
    ui(new Ui::DialogTaskFinished){
     ui->setupUi(this);
    this->ui->taskNameLabel->setText(QString::fromStdString(taskName));

}

DialogTaskFinished::DialogTaskFinished(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTaskFinished)
{
    ui->setupUi(this);
}

DialogTaskFinished::~DialogTaskFinished()
{
    delete ui;
}

void DialogTaskFinished::on_comboBox_currentIndexChanged(const QString &arg1)
{
    this->giveUpStr = arg1.toStdString();
}

void DialogTaskFinished::on_buttonBox_accepted()
{
    this->giveUpStr = this->ui->comboBox->currentText().toStdString();
    this->comments = this->ui->comments->toPlainText().toStdString();
}

string DialogTaskFinished::getComments(){
    replace(this->comments.begin(),this->comments.end(),'\n', ' ');
    replace(this->comments.begin(),this->comments.end(),'\r', ' ');
    return this->comments;
}


string DialogTaskFinished::getGiveUpStr(){
    return this->giveUpStr;
}
