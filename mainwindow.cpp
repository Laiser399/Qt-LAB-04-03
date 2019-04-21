#include "mainwindow.h"
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>
#include <QGridLayout>
#include <QTableView>
#include <QSqlRelation>
#include <QTabWidget>
#include <QSqlRelationalDelegate>
#include <QLabel>
#include <QPushButton>

//-------------|
//   private   |
//-------------|
void MainWindow::setMenu() {
    QMenu *mFile = new QMenu("&Файл");
    mFile->addAction("Считать данные из файла", this, SLOT(slot_LoadDataFromFile()));
    mFile->addSeparator();
    mFile->addAction("&Выход", this, SLOT(close()));

    QMenu *mTables = new QMenu("&Таблицы");
    mTables->addAction("Список таблиц", this, SLOT(slot_tablesList()));
    mTables->addAction("Создать таблицы", this, SLOT(slot_CreateTables()));
    mTables->addAction("Очистить таблицы", this, SLOT(slot_clearTables()));
    mTables->addAction("Удалить таблицы", this, SLOT(slot_DeleteTables()));

    QMenu *mHelp = new QMenu("&?");
    mHelp->addAction("&Автор", this, SLOT(slot_About()));


    menuBar()->addMenu(mFile);
    menuBar()->addMenu(mTables);
    menuBar()->addMenu(mHelp);
}

void MainWindow::setWidgets() {
    QWidget *w = new QWidget;
    QGridLayout *lay = new QGridLayout;
        QTabWidget *tabWidget = new QTabWidget;
            QTableView *viewWeather = new QTableView;
                modelWeather = new QSqlRelationalTableModel(nullptr, db);
            QTableView *viewCities = new QTableView;
                modelCities = new QSqlRelationalTableModel;
            QWidget *WStat = new QWidget;
                QGridLayout *statLay = new QGridLayout;
                    QLabel *pLabel0 = new QLabel("Город");
                    citiesList = new QComboBox;
                    QPushButton *calcStat = new QPushButton("Показать статистику");
                    QLabel *pLabel1 = new QLabel("Наим. температура");
                    QLabel *pLabel2 = new QLabel("Наиб. температура");
                    QLabel *pLabel3 = new QLabel("Средняя температура");
                    QLabel *pLabel4 = new QLabel("Наим. давление");
                    QLabel *pLabel5 = new QLabel("Наиб. давление");
                    QLabel *pLabel6 = new QLabel("Среднее давление");
                    QLabel *pLabel7 = new QLabel("Наим. скорость ветра");
                    QLabel *pLabel8 = new QLabel("Наиб. скорость ветра");
                    QLabel *pLabel9 = new QLabel("Средняя скорость ветра");
                    for (int i = 0; i < 9; i++)
                        statEdits[i] = new QLineEdit;


    setCentralWidget(w);
    w->setLayout(lay);
        lay->addWidget(tabWidget, 0, 0);
            tabWidget->addTab(viewWeather, "Погода");
                viewWeather->setModel(modelWeather);
            tabWidget->addTab(viewCities, "Города");
                viewCities->setModel(modelCities);
            tabWidget->addTab(WStat, "Статистика");
                WStat->setLayout(statLay);
                    statLay->addWidget(pLabel0, 0, 0);
                    statLay->addWidget(citiesList, 0, 1);
                    statLay->addWidget(calcStat, 0, 2);
                    statLay->addWidget(pLabel1, 1, 0, 1, 2);
                    statLay->addWidget(pLabel2, 2, 0, 1, 2);
                    statLay->addWidget(pLabel3, 3, 0, 1, 2);
                    statLay->addWidget(pLabel4, 4, 0, 1, 2);
                    statLay->addWidget(pLabel5, 5, 0, 1, 2);
                    statLay->addWidget(pLabel6, 6, 0, 1, 2);
                    statLay->addWidget(pLabel7, 7, 0, 1, 2);
                    statLay->addWidget(pLabel8, 8, 0, 1, 2);
                    statLay->addWidget(pLabel9, 9, 0, 1, 2);
                    for (int i = 0; i < 9; i++)
                        statLay->addWidget(statEdits[i], i + 1, 2);



    for (int i = 0; i < 9; i++)
        statEdits[i]->setReadOnly(true);
    statLay->setColumnStretch(0, 0);
    statLay->setColumnStretch(1, 1);
    statLay->setColumnStretch(2, 1);
    connect(calcStat, SIGNAL(pressed()), this, SLOT(slot_showStat()));

    viewWeather->setItemDelegate(new QSqlRelationalDelegate);
    viewWeather->setEditTriggers(QAbstractItemView::NoEditTriggers);
    modelWeather->setTable("weather");
    modelWeather->setRelation(1, QSqlRelation("cities", "id", "name"));
    modelWeather->setHeaderData(0, Qt::Horizontal, "id");
    modelWeather->setHeaderData(1, Qt::Horizontal, "Город");
    modelWeather->setHeaderData(2, Qt::Horizontal, "Дата");
    modelWeather->setHeaderData(3, Qt::Horizontal, "Температура");
    modelWeather->setHeaderData(4, Qt::Horizontal, "Давление");
    modelWeather->setHeaderData(5, Qt::Horizontal, "Скорость ветра");

    viewCities->setEditTriggers(QAbstractItemView::NoEditTriggers);
    modelCities->setTable("cities");
    modelCities->setHeaderData(0, Qt::Horizontal, "id");
    modelCities->setHeaderData(1, Qt::Horizontal, "Город");

    connect(this, SIGNAL(dataChanged()), modelWeather, SLOT(select()));
    connect(this, SIGNAL(dataChanged()), modelCities, SLOT(select()));
    connect(this, SIGNAL(dataChanged()), this, SLOT(slot_fillCitiesComboBox()));

    emit dataChanged();

    //view->resizeColumnsToContents();
}

int MainWindow::getCityId(const QString &cityName) {
    QSqlQuery q(db);
    QString cmd = "SELECT id FROM cities WHERE lower(name)='%1';";
    cmd = cmd.arg(cityName.toLower());

    if (!q.exec(cmd)) {
        return -1;
    }

    if (q.next()) {
//        qDebug() << "here1";
//        qDebug() << q.value(0).toInt();
//        qDebug() << "here2";
        return q.value(0).toInt();
    }

    q.prepare("INSERT INTO cities (name) VALUES(?);");
    q.bindValue(0, cityName);
    if (!q.exec()) {
        return -1;
    }
    return q.lastInsertId().toInt();
}


//-------------------|
//   private slots   |
//-------------------|
void MainWindow::slot_About() {
    QMessageBox::information(this, "Автор", "Студент МАИ\nГруппы М8О-213Б-17\nСеменов Сергей Дмитриевич");
}

void MainWindow::slot_tablesList() {
    QStringList tables = db.tables();
    QString res;
    QTextStream stream(&res);
    for (int i = 0; i < tables.size(); i++) {
        stream << "\"" << tables[i] << "\"";
        if (i < tables.size() - 1)
            stream << ", ";
    }
    QMessageBox::information(this, "Список таблиц", QString("Созданные таблицы: ") + res);
}

void MainWindow::slot_CreateTables() {
    bool cr_w, cr_c;
    QSqlQuery q(db);
    cr_w = q.exec("CREATE TABLE IF NOT EXISTS weather ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "id_city INTEGER NOT NULL,"
                       "date DATE NOT NULL,"
                       "temperature INTEGER NOT NULL,"
                       "pressure INTEGER NOT NULL,"
                       "wind_speed INTEGER NOT NULL"
                       ");");
    cr_c = q.exec("CREATE TABLE IF NOT EXISTS cities ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "name VARCHAR NOT NULL"
                       ");");
    if (!cr_w || !cr_c) {
        QMessageBox::information(this, "Ошибка", "Ошибка при создании таблиц!");
    }

    modelCities->setTable("cities");
    emit dataChanged();
}

void MainWindow::slot_clearTables() {
    QSqlQuery q(db);
    if (!q.exec("DELETE FROM cities;") || !q.exec("DELETE FROM weather;"))
        QMessageBox::information(this, "Ошибка", "Ошибка очистки таблиц!");

    dataChanged();
}

void MainWindow::slot_DeleteTables() {
    QSqlQuery q(db);
    if (!q.exec("DROP TABLE weather;") || !q.exec("DROP TABLE cities;")) {
        QMessageBox::information(this, "Ошибка", "Ошибка при удалении таблиц!");
    }

    dataChanged();
}

void MainWindow::slot_LoadDataFromFile() {
    QString filename = QFileDialog::getOpenFileName(this, QString(), QString(), "Text (*.txt);;Any file (*.*)");
    if (filename.length() == 0)
        return;

    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) {
        QMessageBox::information(this, "Ошибка", "Ошибка открытия файла!");
        return;
    }
    QTextStream stream(&f);
    QString text = stream.readAll();
    f.close();

    QSqlQuery q(db);
    int count_all = 0, count_added = 0;
    QStringList rows = text.split(';');
    for (int i = 0; i < rows.size(); i++) {
        QString row = rows[i].simplified();
        if ((row.length() > 0) && (row[0] == '(')
                && (row[row.size() - 1] == ')'))
        {
            row = row.mid(1, row.length() - 2);
            QStringList cols = row.split(',');
            if (cols.length() == 5) {
                count_all++;
                int id_city = getCityId(cols[0].simplified());
                if (id_city == -1) {
                    qDebug() << "err id city";
                    continue;
                }

                q.prepare("INSERT INTO weather "
                          "(id_city, date, temperature, pressure, wind_speed) "
                          "VALUES(?, ?, ?, ?, ?);");
                //'?', '?', '?', '?'
                q.bindValue(0, id_city);
                for (int i = 1; i < 5; i++)
                    q.bindValue(i, cols[i].simplified());

                if (q.exec())
                    count_added++;
                else
                    qDebug() << q.lastError();
            }
        }
    }

    QMessageBox::information(this, "Информация", QString("Добавлено %1 из %2 записей!")
                             .arg(count_added).arg(count_all));

    dataChanged();
}

void MainWindow::slot_fillCitiesComboBox() {
    citiesList->clear();

    QSqlQuery q(db);
    q.exec("SELECT * FROM cities;");
    while (q.next()) {
        citiesList->addItem(q.value(1).toString(), q.value(0));
    }
}

void MainWindow::slot_showStat() {
    for (int i = 0; i < 9; i++) {
        statEdits[i]->setText("");
    }

    QString city = citiesList->currentText();
    int id_city = citiesList->currentData().toInt();
    if (city.isEmpty())
        return;

    QSqlQuery q(db);
    q.prepare("SELECT temperature, pressure, wind_speed FROM weather WHERE id_city=?;");
    q.bindValue(0, id_city);
    if (!q.exec())
        return;
    double temp_sum = 0, pres_sum = 0, w_speed_sum = 0;
    int counter = 0;
    int temp_min = -1, temp_max = -1, pres_min = -1, pres_max = -1,
        w_speed_min = -1, w_speed_max = -1;
    while (q.next()) {
        int temp = q.value(0).toInt();
        int pres = q.value(1).toInt();
        int w_speed = q.value(2).toInt();

        if ((temp_min == -1) || (temp < temp_min))
            temp_min = temp;
        if ((temp_max == -1) || (temp > temp_max))
            temp_max = temp;

        if ((pres_min == -1) || (pres < pres_min))
            pres_min = pres;
        if ((pres_max == -1) || (pres > pres_max))
            pres_max = pres;

        if ((w_speed_min == -1) || (w_speed < w_speed_min))
            w_speed_min = w_speed;
        if ((w_speed_max == -1) || (w_speed > w_speed_max))
            w_speed_max = w_speed;

        temp_sum += temp;
        pres_sum += pres;
        w_speed_sum += w_speed;
        counter++;
    }

    if (counter == 0)
        return;
    statEdits[0]->setText(QString::number(temp_min));
    statEdits[1]->setText(QString::number(temp_max));
    statEdits[2]->setText(QString::number(temp_sum / counter));
    statEdits[3]->setText(QString::number(pres_min));
    statEdits[4]->setText(QString::number(pres_max));
    statEdits[5]->setText(QString::number(pres_sum / counter));
    statEdits[6]->setText(QString::number(w_speed_min));
    statEdits[7]->setText(QString::number(w_speed_max));
    statEdits[8]->setText(QString::number(w_speed_sum / counter));
}

//------------|
//   public   |
//------------|
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("lab_04_03_db");
    if (!db.open()) {
        QMessageBox::information(this, "Ошибка", "Ошибка подключения к БД!");
        exit(0);
    }


    setMenu();
    setWidgets();

    resize(650, 400);

}

MainWindow::~MainWindow()
{
    db.close();
}
