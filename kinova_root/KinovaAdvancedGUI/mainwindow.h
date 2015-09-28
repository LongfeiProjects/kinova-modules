#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "kinovalib.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    Kinovalib* klib ;

    int opType;
    void initGUI();
public slots:
    void clickedSlot();
private slots:
    void on_homeButton_clicked();
    void on_typeComboBox_currentIndexChanged(int index);

};

#endif // MAINWINDOW_H
