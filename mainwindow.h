#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QLineEdit>
#include <QSqlRelationalTableModel>
#include <QComboBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    void setMenu();
    void setWidgets();
    int getCityId(const QString &cityName);


    QSqlDatabase db;
    QSqlRelationalTableModel *modelWeather, *modelCities;
    QComboBox *citiesList;
    QLineEdit *statEdits[9];

private slots:
    void slot_About();
    void slot_tablesList();
    void slot_CreateTables();
    void slot_clearTables();
    void slot_DeleteTables();
    void slot_LoadDataFromFile();
    void slot_fillCitiesComboBox();
    void slot_showStat();
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void dataChanged();
};

#endif // MAINWINDOW_H
