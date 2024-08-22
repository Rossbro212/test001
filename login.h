#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "Packdef.h"
#include "kernel/IKernel.h"
#include <QMessageBox>
namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
public slots:
    void slot_register(STRU_REGISTER_RS* psrr);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

public:
    void setKernel(IKernel *pKernel){
        m_pKernel = pKernel;
    }
private:
    Ui::login *ui;
    IKernel *m_pKernel;
};

#endif // LOGIN_H
