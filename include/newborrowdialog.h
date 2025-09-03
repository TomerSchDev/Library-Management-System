#ifndef NEWBORROWDIALOG_H
#define NEWBORROWDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "book.h"

namespace Ui {
    class NewBorrowDialog;
}

class NewBorrowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewBorrowDialog(const QList<Book>& availableBooks, QWidget *parent = nullptr);
    ~NewBorrowDialog() override;

    Book getSelectedBook() const;
    QDate getReturnDate() const;

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::NewBorrowDialog *ui;
    QList<Book> m_availableBooks;
};

#endif // NEWBORROWDIALOG_H