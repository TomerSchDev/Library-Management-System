//
// Created by Tomer on 02/09/2025.
//

#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H

#include <QDialog>

#include "abstractWindow.h"

namespace Ui {
    class AddBookDialog;
}

class AddBookDialog final :public AbstractWindow
{
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    ~AddBookDialog() override;
    void handleEvent(EventType event) override;
    QString getTitle() const;
    QString getAuthor() const;
    int getYear() const;
    int getCopies() const;

private:
    Ui::AddBookDialog *ui;
};


#endif //ADDBOOKDIALOG_H
