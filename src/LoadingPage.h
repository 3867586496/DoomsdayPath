#ifndef LOADINGPAGE_H
#define LOADINGPAGE_H

#include <QWidget>

class QProgressBar;

class LoadingPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingPage(QWidget *parent = nullptr);

signals:
    void backClicked();

private:
    QProgressBar *m_progressBar = nullptr;
};

#endif // LOADINGPAGE_H
