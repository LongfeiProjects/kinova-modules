#ifndef DIALOGTASKFINISHED_H
#define DIALOGTASKFINISHED_H
#include <string.h>
#include <QDialog>

using namespace std;
namespace Ui {
class DialogTaskFinished;
}

class DialogTaskFinished : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTaskFinished(QWidget *parent = 0);
    DialogTaskFinished(QWidget *parent = 0,string taskName = "");
    ~DialogTaskFinished();

    string getComments();
    string getGiveUpStr();

private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::DialogTaskFinished *ui;
    string giveUpStr;
    string comments;
};

#endif // DIALOGTASKFINISHED_H
