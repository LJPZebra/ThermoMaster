#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    // === DEFINITIONS =====================================================

    SetupName = "ThermoMaster";
    Version = "1.0.1";

    // --- Pathes
    filesep = QString(QDir::separator());
    progPath = QDir::currentPath() + filesep;
    projPath = QDir::currentPath();
    projPath = projPath.mid(0, projPath.toStdString().find_last_of(filesep.toStdString()));
    projPath = projPath.mid(0, projPath.toStdString().find_last_of(filesep.toStdString()));
    projPath = projPath.mid(0, projPath.toStdString().find_last_of(filesep.toStdString())) + filesep;

    // Run
    SaveRate = 10;              // Image saving rate (Hz)
    nRun = 0;
    nFrame = 0;

    // Image writer
    ImgWriter = new QImageWriter;
    ImgWriter->setFormat("pgm");

    // === USER INTERFACE ==================================================

    // --- Main window

    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowTitle(SetupName);

    // --- Set data path ------------------------

    ui->ProjectPath->setText(projPath);
    updatePath();

    // --- Set date -----------------------------

    ui->SpawningDate->setDate(QDate::currentDate());

    // --- Shortcuts

    // Esc: Close
    s_Close = new QShortcut(Qt::Key_Escape, this);
    connect(s_Close, SIGNAL(activated()), QApplication::instance(), SLOT(quit()));

    // --- Messages -----------------------------

    // Style
    QFile File("output.css");
    File.open(QFile::ReadOnly);
    QTextDocument *OutDoc = new QTextDocument;
    OutDoc->setDefaultStyleSheet(File.readAll());
    OutDoc->setDefaultFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    ui->Output->setDocument(OutDoc);

    // Timer
    QTimer *t_Msg = new QTimer();
    connect(t_Msg, SIGNAL(timeout()), this, SLOT(UpdateMessage()));
    t_Msg->start(50);

    qInfo() << TITLE_1 << "Initialization";

    // Thread info
    qInfo().nospace() << THREAD << "Mainwindow lives in thread: " << QThread::currentThreadId();

    // --- Plots --------------------------------

    QColor Col = Qt::darkCyan;

    // Define graphs
    ui->PlotLeft->addGraph();
    ui->PlotLeft->graph(0)->setPen(QPen(Col));
    ui->PlotLeft->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Col,Col, 3));
    ui->PlotLeft->addGraph();
    ui->PlotLeft->graph(1)->setPen(QPen(Qt::red));
    ui->PlotLeft->graph(1)->setBrush(QBrush(QColor(255, 0, 0, 20)));

    ui->PlotRight->addGraph();
    ui->PlotRight->graph(0)->setPen(QPen(Col));
    ui->PlotRight->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Col,Col, 3));
    ui->PlotRight->addGraph();
    ui->PlotRight->graph(1)->setPen(QPen(Qt::red));
    ui->PlotRight->graph(1)->setBrush(QBrush(QColor(255, 0, 0, 20)));

    // Configure right and top axis to show ticks but no labels
    ui->PlotLeft->xAxis2->setVisible(true);
    ui->PlotLeft->xAxis2->setTickLabels(false);
    ui->PlotLeft->yAxis2->setVisible(true);
    ui->PlotLeft->yAxis2->setTickLabels(false);

    ui->PlotRight->xAxis2->setVisible(true);
    ui->PlotRight->xAxis2->setTickLabels(false);
    ui->PlotRight->yAxis2->setVisible(true);
    ui->PlotRight->yAxis2->setTickLabels(false);

    // Make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->PlotLeft->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotLeft->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->PlotLeft->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotLeft->yAxis2, SLOT(setRange(QCPRange)));

    connect(ui->PlotRight->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotRight->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->PlotRight->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->PlotRight->yAxis2, SLOT(setRange(QCPRange)));

    // Default target values
    TargetLeftValue = ui->TargetLeft->text().toDouble();
    TargetRightValue = ui->TargetRight->text().toDouble();

    // === Camera ==========================================================

    qInfo() << TITLE_2 << "Camera";

    // Initialize Camera
    InitCamera();

    // === Connections =====================================================

    connect(ui->CheckSerial, SIGNAL(released()), this, SLOT(checkSerial()));

    connect(ui->ProjectPathButton, SIGNAL(clicked()), this, SLOT(BrowseProject()));
    connect(ui->Autoset, SIGNAL(clicked()), this, SLOT(autoset()));
    connect(ui->ProtocolPathButton, SIGNAL(clicked()), this, SLOT(BrowseProtocol()));
    connect(ui->ProtocolRun, SIGNAL(toggled(bool)), this, SLOT(toggleProtocol(bool)));

    connect(ui->Snapshot, SIGNAL(clicked()), this, SLOT(snapshot()));
    connect(ui->SpawningDate, SIGNAL(dateChanged(QDate)), this, SLOT(updateAge(QDate)));

    connect(ui->Regulation, SIGNAL(released()), this, SLOT(setRegulation()));
    connect(ui->setTargets, SIGNAL(released()), this, SLOT(setTargets()));

    connect(ui->LightValue, SIGNAL(editingFinished()), this, SLOT(setLight()));
    connect(ui->Pcoeff, SIGNAL(editingFinished()), this, SLOT(setP()));
    connect(ui->Icoeff, SIGNAL(editingFinished()), this, SLOT(setI()));
    connect(ui->Dcoeff, SIGNAL(editingFinished()), this, SLOT(setD()));

    // === Timers ==========================================================

    // --- Protocol timer
    timerProtocol = new QTimer(this);
    connect(timerProtocol, SIGNAL(timeout()), this, SLOT(ProtocolLoop()));

    // === Startup =========================================================

    //skipSerial = true;
    QTimer::singleShot(400, this, SLOT(checkSerial()));

}

/* ====================================================================== *\
|    MESSAGES                                                              |
\* ====================================================================== */

void MainWindow::UpdateMessage() {

    while (Messages.length()) {

        Message MSG = Messages.takeFirst();
        QString S;

        switch (MSG.type) {
        case QtDebugMsg:
            cout << MSG.text.toStdString() << endl;
            break;
        case QtInfoMsg:
            S = "<" + MSG.css + ">" + MSG.text + "</p>" ;
            break;
        case QtWarningMsg:
            S = "<p class='warning'>" + MSG.text + "</p>";
            break;
        case QtCriticalMsg:
            S= "<p class='critical'>" + MSG.text + "</p>";
            break;
        case QtFatalMsg:
            S = "<p class='fatal'>" + MSG.text + "</p>";
            break;
        }

        ui->Output->setHtml(ui->Output->toHtml().append(S));
        ui->Output->verticalScrollBar()->setValue(ui->Output->verticalScrollBar()->maximum());
    }
}

/* ====================================================================== *\
|    DIRECTORIES                                                           |
\* ====================================================================== */

void MainWindow::BrowseProject() {

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly);
    if (dir.length()) {
        ui->ProjectPath->setText(dir + filesep);
        updatePath();
    }

}

void MainWindow::updatePath() {

    QDateTime now = QDateTime::currentDateTime();
    ui->DataPath->setText(ui->ProjectPath->text() + "Data" + filesep + now.toString("yyyy-MM-dd") + filesep);
    autoset();

}

void MainWindow::autoset() {

    // Create data folder?
    if (!QDir(ui->DataPath->text()).exists()) { QDir().mkdir(ui->DataPath->text()); }

    // Find last run
    nRun = 0;
    QDir DataDir(ui->DataPath->text());
    QFileInfoList FIL = DataDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::DirsFirst);
    foreach(const QFileInfo &elm, FIL) {
        if(QDir(elm.absoluteFilePath()).exists()) {

            QStringList tmp = elm.fileName().split(" ", QString::SkipEmptyParts);
            if (tmp.count()==2 || !tmp.at(0).compare("Run")) {
                nRun = tmp.at(1).toInt();
            }
        }
    }

    if (nRun) {
        ui->statusBar->showMessage(QString("Last run: %1").arg(nRun, 2, 10, QLatin1Char('0')));
    } else {
        ui->statusBar->showMessage(QString("No run for today"));
    }
}

void MainWindow::updateAge(QDate) {

    ui->Age->setText(QString::number(ui->SpawningDate->date().daysTo(QDate::currentDate())));

}

/* ====================================================================== *\
|    SERIAL COMMUNICATION                                                  |
\* ====================================================================== */

void MainWindow::checkSerial() {

    qInfo() << TITLE_2 << "Serial connections";

    // --- Get available ports

    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    qInfo() << infos.length() << "connections detected";

    // --- Assign port

    for (int i=0; i<infos.length(); i++) {

        // --- Checks

        // Skip non-Arduino connections
        if (infos[i].description().left(7)!="Arduino") { continue; }

        // Is device busy ?
        if (infos[i].isBusy()) {
            qInfo().nospace() << "[" << infos[i].portName() << "] is busy ...";
            continue;
        }

        // --- Open connection

        qInfo() << "Opening" << infos[i].portName();

        Serial = new QSerialPort(this);

        Serial->setPortName(infos[i].portName());
        Serial->setBaudRate(115200);
        Serial->setDataBits(QSerialPort::Data8);
        Serial->setParity(QSerialPort::NoParity);
        Serial->setStopBits(QSerialPort::OneStop);
        Serial->setFlowControl(QSerialPort::NoFlowControl);

        if (!Serial->open(QIODevice::ReadWrite)) {
            qWarning() << "Failed to open port" << Serial->portName();
            return;
        }

        qInfo() << "Init. serial connection";

        // Connect serial read output
        connect(Serial, SIGNAL(readyRead()), this, SLOT(readSerial()));

    }
}

void MainWindow::send(QString cmd) {
    Serial->write(cmd.toStdString().c_str());
    Serial->flush();
    QThread::msleep(5);
}

void MainWindow::readSerial() {

    // --- Read response
    QByteArray readData = Serial->readAll();
    while (Serial->waitForReadyRead(10)) {
        readData.append(Serial->readAll());
    }

    if (skipSerial) {
        skipSerial = false;
        return;
    }

    QStringList res = QString(readData).split("\r\n", QString::SkipEmptyParts);

    // --- Filter responses
    for (int i=0; i<res.size(); i++) {

        if (res[i].left(4)=="Data") {

            QStringList Data = res[i].mid(5).split(" ", QString::SkipEmptyParts);
            setTemperatures(Data);

        } else {

            // --- Display
            qDebug() << res[i].toStdString().c_str();

        }

    }

}

/* ====================================================================== *\
|    CAMERA                                                                |
\* ====================================================================== */

void MainWindow::InitCamera() {

    Camera = new Camera_FLIR(0);

    // --- Connections
    connect(ui->UpdateCamera, SIGNAL(released()), this, SLOT(UpdateCamera()));
    connect(Camera, SIGNAL(newImageForDisplay(QPixmap)), this, SLOT(updateDisplay(QPixmap)));
    connect(ui->CstAvg, SIGNAL(toggled(bool)), this, SLOT(SetAvgVal(bool)));

    this->ArmCamera();

}

void MainWindow::ArmCamera() {

    // Update Settings
    Camera->Exposure = ui->Exposure->text().toFloat();
    Camera->X1 = ui->X1->text().toFloat();
    Camera->X2 = ui->X2->text().toFloat();
    Camera->Y1 = ui->Y1->text().toFloat();
    Camera->Y2 = ui->Y2->text().toFloat();

    // Create new camera
    Camera->newCamera();

}

void MainWindow::UpdateCamera() {

    Camera->stopCamera();
    this->ArmCamera();

}

void MainWindow::updateDisplay(QPixmap pix) {

    ui->Image->setPixmap(pix);
    ui->AvgValue->setText(QString("%1").arg(Camera->avgval));

    if (ui->Record->isChecked()) {

        // Save Image
        ImgWriter->setFileName(QString(RunPath + filesep + "Frame_%1.pgm").arg(nFrame, 6, 10, QLatin1Char('0')));
        ImgWriter->write(ui->Image->pixmap()->toImage());

        // Append metadata
        QFile ImgFile(ImgWriter->fileName());
        if (ImgFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QTextStream tmp(&ImgFile);
            tmp << endl << "#" + SetupName + " " + Version;
            tmp << endl << "#" + QString("Timestamp:%1;TempLeft:%2;TempRight:%3").arg(Camera->timestamp).arg(TempLeft.last()).arg(TempRight.last());
        }

        // --- Update

        // Status bar
        ui->statusBar->showMessage(QString("Run %1 - Frame %2").arg(nRun, 2, 10, QLatin1Char('0')).arg(nFrame, 6, 10, QLatin1Char('0')));

        // Frame number
        nFrame++;

    }
}

void MainWindow::snapshot() {

    // Create snapshot directory?
    QString SnapPath(ui->DataPath->text() + "Snapshots" + filesep);
    if (!QDir(ui->DataPath->text()).exists()) { QDir().mkdir(ui->DataPath->text()); }
    if (!QDir(SnapPath).exists()) { QDir().mkdir(SnapPath); }

    // Get last image number
    int nSnap = 0;
    QDir SnapDir(SnapPath);
    QStringList flist = SnapDir.entryList(QDir::NoDotAndDotDot | QDir::Files);
    foreach(QString file, flist) {
        QStringList tmp = file.split("_", QString::SkipEmptyParts);
        tmp = tmp.at(1).split(".");
        if (tmp.at(0).toInt()>nSnap) { nSnap = tmp.at(0).toInt(); }
    }
    nSnap++;



    // Update status message
    ui->statusBar->showMessage(QString("Last image: %1").arg(nSnap, 6, 10, QLatin1Char('0')));

}


void MainWindow::SetAvgVal(bool b) {

    if (b) { Camera->setCstAvg(ui->AvgValue->text().toDouble()); }
    else { Camera->setCstAvg(-1); }

}


/* ====================================================================== *\
|    LIGHT                                                                 |
\* ====================================================================== */

void MainWindow::setLight() {

    // Update light
    send("light " + QString("%1").arg(round(ui->LightValue->value()*2.55)));
}

/* ====================================================================== *\
|    TEMPERATURES                                                          |
\* ====================================================================== */

void MainWindow::setTargets() {

    QString cmd = "set " + ui->TargetLeft->text() + " " + ui->TargetRight->text();
    send(cmd);

    TargetLeftValue = ui->TargetLeft->text().toDouble();
    TargetRightValue = ui->TargetRight->text().toDouble();

}

void MainWindow::setRegulation() {

    if (ui->Regulation->isChecked()) { send(QString("start")); }
    else { send(QString("stop")); }

}

void MainWindow::setTemperatures(QStringList Data) {

    // --- Update text displays
    ui->TempLeft->setText(Data[1]);
    ui->TempRight->setText(Data[2]);

    // --- Update plot

    // Update vectors
    Time.append(Data[0].toDouble()/1e6);
    TempLeft.append(Data[1].toDouble());
    TempRight.append(Data[2].toDouble());
    if (ui->Regulation->isChecked()) {
        TargetLeft.append(TargetLeftValue);
        TargetRight.append(TargetRightValue);
    } else {
        TargetLeft.append(0);
        TargetRight.append(0);
    }

    while (Time.count()>200) {
        Time.removeFirst();
        TempLeft.removeFirst();
        TempRight.removeFirst();
        TargetLeft.removeFirst();
        TargetRight.removeFirst();
    }

    // Display plot
    ui->PlotLeft->graph(0)->setData(Time, TempLeft);
    ui->PlotLeft->graph(1)->setData(Time, TargetLeft);
    ui->PlotRight->graph(0)->setData(Time, TempRight);
    ui->PlotRight->graph(1)->setData(Time, TargetRight);

    ui->PlotLeft->xAxis->setRange(Time.first()-0.5, Time.last()+0.5);
    ui->PlotRight->xAxis->setRange(Time.first()-0.5, Time.last()+1);
    ui->PlotLeft->yAxis->setRange(15,40);
    ui->PlotRight->yAxis->setRange(15,40);

    // ui->PlotLeft->rescaleAxes();

    ui->PlotLeft->replot();
    ui->PlotRight->replot();

}

/* ====================================================================== *\
|    PID COEFFICIENTS                                                      |
\* ====================================================================== */

void MainWindow::setP() { send("P " + QString("%1").arg(ui->Pcoeff->value())); }
void MainWindow::setI() { send("I " + QString("%1").arg(ui->Icoeff->value())); }
void MainWindow::setD() { send("D " + QString("%1").arg(ui->Dcoeff->value())); }

/* ====================================================================== *\
|    PROTOCOLS                                                             |
\* ====================================================================== */

void MainWindow::BrowseProtocol() {

    QString pfile = QFileDialog::getOpenFileName(this, tr("Open protocol file"), ui->ProjectPath->text() + "Data" + filesep + "Protocols", "Protocol files (*.protocol);; All files (*.*)");
    if (pfile.length()) { ui->ProtocolPath->setText(pfile); }

}

void MainWindow::toggleProtocol(bool b) {

    if (ui->ProtocolRun->isChecked()) {

        // --- Load protocol
        QFile *PFile = new QFile(ui->ProtocolPath->text());

        if (!PFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Unable to load \"" << PFile->fileName() << "\"";
            return;
        };

        QTextStream stream(PFile);
        Protocol.clear();
        QString line;
        while (stream.readLineInto(&line)) {

            // --- Remove empty lines and comments
            if (line.isEmpty() || line.left(1) == "#") { continue; }

            Protocol.append(line);
        }

        // --- Start protocol
        ui->ProtocolTime->setStyleSheet("QLabel { color: firebrick;}");
        ProtocolTime.start();
        timerProtocol->start(1000);
        ProtoLoop();

    } else {

        // Clear Protocol
        ui->ProtocolTime->setText("00:00:00");
        ui->ProtocolTime->setStyleSheet("QLabel { color: black;}");
        Protocol.clear();
        timerProtocol->stop();

        // Stop recording
        ui->Record->setChecked(false);

    }

}

void MainWindow::ProtocolLoop() {
// Synchronous loop (every 1s) for display

    // Ellapsed time
    int m = ProtocolTime.elapsed();
    int hours = floor(m/3600000); m -= hours*3600000;
    int minutes = floor(m/60000); m -= minutes*60000;
    int seconds = floor(m/1000);

    ui->ProtocolTime->setText(QString("%1").arg(hours, 2, 10, QChar('0')) + ":" +
                              QString("%1").arg(minutes, 2, 10, QChar('0')) + ":" +
                              QString("%1").arg(seconds, 2, 10, QChar('0')));

}

void MainWindow::ProtoLoop() {
// Asynchronous loop for processing protocol commands

    // --- End of protocol
    if (!Protocol.count()) {
        ui->ProtocolRun->setChecked(false);
        return;
    }

    // Loop by default
    bool brem = true;
    bool bcont = true;

    // --- Parse first command
    QStringList list = Protocol[0].split(":");

    if (list.at(0)=="print") {

        // --- PRINT ----------------------------

        qInfo() << qPrintable(list.at(1));

    } else if (list.at(0)=="data") {

        // --- DATA ---------------------------

        if (list.at(1)=="create directory") {

            // Reset run number
            nRun++;

            // Create run directory
            if (!QDir(ui->DataPath->text()).exists()) { QDir().mkdir(ui->DataPath->text()); }
            RunPath = QString(ui->DataPath->text() + "Run %1" + filesep).arg(nRun, 2, 10, QLatin1Char('0'));
            if (!QDir(RunPath).exists()) { QDir().mkdir(RunPath); }

            // Save protocol file
            QFile::copy(ui->ProtocolPath->text(), RunPath + filesep + "Protocol.txt");

            // Save parameters
            QFile fparam(RunPath + filesep + "Parameters.txt");
            if (fparam.open(QIODevice::ReadWrite)) {
                QTextStream stream(&fparam);
                stream << "Strain\t" << ui->Strain->text() << endl;
                stream << "Spawning_date\t" << ui->SpawningDate->date().toString("yyyy-MM-dd") << endl;
                stream << "Age\t" << ui->Age->text() << endl;
            }

        }

    } else if (list.at(0)=="camera") {

        // --- CAMERA ---------------------------

        if (list.at(1)=="start") {

            nFrame = 0;
            ui->Record->setChecked(true);

        } else if (list.at(1)=="stop") {

            ui->Record->setChecked(false);

        }

    } else if (list.at(0)=="regulation") {

        // --- REGULATION -----------------------

        // NOT WORKING. WHY ?!?

        if (list.at(1)=="start") {
            ui->Regulation->setChecked(true);
            send(QString("start"));
        } else if (list.at(1)=="stop") {
            ui->Regulation->setChecked(false);
            send(QString("stop"));
        }
        setRegulation();

    }  else if (list.at(0)=="targets") {

        // --- TARGETS --------------------------

        ui->TargetLeft->setText(list.at(1));
        ui->TargetRight->setText(list.at(2));
        setTargets();

    } else if (list.at(0)=="wait") {

        // --- WAIT -----------------------------

        QTimer::singleShot(list.at(1).toInt(), this, SLOT(ProtoLoop()));
        Protocol.removeFirst();
        return;

    } else {

        qDebug() << "Unknown command:" << Protocol[0];
        return;
    }

    // --- Remove first command
    Protocol.removeFirst();

    // --- Continue
    if (bcont) { ProtoLoop(); }

}

MainWindow::~MainWindow() {
    delete ui;
}
