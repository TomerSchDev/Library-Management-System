//
// Created by Tomer on 02/09/2025.
//

#ifndef ADDBOOKDIALOG_H
#define ADDBOOKDIALOG_H

#include <QDialog>

namespace Ui {
    class AddBookDialog;
}

class AddBookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddBookDialog(QWidget *parent = nullptr);
    ~AddBookDialog() override;

    QString getTitle() const;
    QString getAuthor() const;
    int getYear() const;
    int getCopies() const;

private:
    Ui::AddBookDialog *ui;
};


#endif //ADDBOOKDIALOG_H
