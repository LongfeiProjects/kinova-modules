#include "savedialog.h"
#include "ui_savedialog.h"
#include <unistd.h>

using namespace std;

SaveDialog::SaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialog)
{

    ui->setupUi(this);
}

SaveDialog::~SaveDialog()
{
    delete ui;
}
