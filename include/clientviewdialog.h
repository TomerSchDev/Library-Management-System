#ifndef CLIENTVIEWDIALOG_H
#define CLIENTVIEWDIALOG_H

#include <QDialog>
#include "../include/client.h"
#include "../include/library.h"

namespace Ui {
    class ClientViewDialog;
}

class ClientViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ClientViewDialog(QWidget *parent = nullptr);
    ~ClientViewDialog();

    void setClient(const Client& client);
    void setLibrary(Library* library);
    bool eventFilter(QObject* obj, QEvent* event);



private:
    Ui::ClientViewDialog *ui;
};

#endif // CLIENTVIEWDIALOG_H
