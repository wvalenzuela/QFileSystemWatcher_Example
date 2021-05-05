#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QFileDialog>
#include <QFileSystemWatcher>
#include <QString>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent *event);
private slots:
    void on_btnBrowse_clicked();

    // our slot to emit
    void NotifyChanges(const QString &path);

private:
    Ui::MainWindow *ui;

    QFileSystemWatcher *watcher;
    QString Path;
};

#endif // MAINWINDOW_H
