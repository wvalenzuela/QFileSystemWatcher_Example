#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
// execute when user click on the Browse button
void MainWindow::on_btnBrowse_clicked()
{
    Path = QFileDialog::getExistingDirectory(this,tr("Open Directory"),
                                             ".",QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    ui->txtPath->setText(Path);
    ui->m_folderList->clear();
    watcher = new QFileSystemWatcher(this);
    watcher->addPath(Path);

    disconnect(watcher,&QFileSystemWatcher::directoryChanged,
               this,&MainWindow::NotifyChanges);
    disconnect(watcher,&QFileSystemWatcher::fileChanged,
               this,&MainWindow::NotifyChanges);

    QDir dir(Path);

    // for more inforation please refer to Qt Documentation
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size|QDir::Reversed);

    // now lets get the files and directories information in the folder
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QListWidgetItem *wi = new QListWidgetItem;
        wi->setText(list.at(i).fileName());
        if(QFileInfo(list.at(i).filePath()).isDir())
            wi->setForeground(Qt::blue);
        ui->m_folderList->addItem(wi);
    }

    // now we are connecting the watcherr object to it's signals
    connect(watcher,&QFileSystemWatcher::directoryChanged,
            this,&MainWindow::NotifyChanges);
    connect(watcher,&QFileSystemWatcher::fileChanged,
            this,&MainWindow::NotifyChanges);
}
// implementation of our function
void MainWindow::NotifyChanges(const QString &path)
{
    qDebug() << "Signal Received !" << path;

    QStringList oldList, newList;

    //getting the old list from QListView widget and
    // the new list which is the current files in the folder
    //then compare them together
    QMap<QString,QString> map_old;

    for (int i = 0; i < ui->m_folderList->count(); ++i) {
        QListWidgetItem *item = ui->m_folderList->item(i);
        oldList << item->text();
        map_old[item->text()] = Path + "/" + item->text();
    }

    qSort(oldList);

    // now getting new status of the folder

    QDir dir(path);

    // for more inforation please refer to Qt Documentation
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size|QDir::Reversed);

    // now lets get the files and directories information in the folder
    QMap<QString,QString> map;
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        newList << list.at(i).fileName();
        map[list.at(i).fileName()] = list.at(i).filePath();
    }
    qSort(newList);

    //Differences between two lists;

    QSet<QString> intersection;

    if(oldList.count() > newList.count())
    {
        ui->m_folderList->clear();

        intersection = oldList.toSet().intersect(newList.toSet());

        foreach (const QString &value, intersection)
        {
            QListWidgetItem *wi = new QListWidgetItem;
            wi->setText(value);
            if(QFileInfo(map[value]).isDir())
                wi->setForeground(Qt::blue);
            ui->m_folderList->addItem(wi);
        }
        this->statusBar()->showMessage("[!] File Removed");
    }
    else if(oldList.count() < newList.count())
    {
        intersection = newList.toSet().subtract(oldList.toSet());

        foreach (const QString &value, intersection)
        {
            QListWidgetItem *wi = new QListWidgetItem;
            wi->setText(value);
            if(QFileInfo(map[value]).isDir())
                wi->setForeground(Qt::red);
            else
                wi->setForeground(Qt::green);
            ui->m_folderList->addItem(wi);
        }
        ui->m_folderList->sortItems();
        qDebug() << intersection.toList();
        this->statusBar()->showMessage("[!] File Added!");
    }
    else
    {
        ui->m_folderList->clear();

        intersection = oldList.toSet().subtract(newList.toSet());
        foreach (const QString &value, newList)
        {
            QListWidgetItem *wi = new QListWidgetItem;
            wi->setText(value);
            if(map_old.find(value) == map_old.end())
            {
                if(QFileInfo(map[value]).isDir())
                    wi->setForeground(Qt::red);
                else
                    wi->setForeground(Qt::green);
            }
            else {
                if(QFileInfo(map[value]).isDir())
                    wi->setForeground(Qt::blue);
            }
            ui->m_folderList->addItem(wi);
        }
        this->statusBar()->showMessage("[!] File Modified!");
    }
}
