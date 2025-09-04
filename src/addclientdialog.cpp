#include "windows/addclientdialog.h"
#include "../ui/ui_addclientdialog.h"

AddClientDialog::AddClientDialog(QWidget *parent)
    : AbstractWindow(parent)
    , ui(new Ui::AddClientDialog)
{
    ui->setupUi(this);
}

AddClientDialog::AddClientDialog(const QList<QString>& existingFamilies, QWidget *parent)
    : AbstractWindow(parent)
    , ui(new Ui::AddClientDialog)
{
    ui->setupUi(this);
    ui->familyComboBox->addItems(existingFamilies);
}

AddClientDialog::~AddClientDialog()
{
    delete ui;
}

QString AddClientDialog::getName() const {
    return ui->nameLineEdit->text();
}

QString AddClientDialog::getSurname() const {
    return ui->surnameLineEdit->text();
}

QString AddClientDialog::getFamily() const {
    return ui->familyComboBox->currentText();
}

void AddClientDialog::handleEvent(const EventType event)
{
    Q_UNUSED(event);
}
