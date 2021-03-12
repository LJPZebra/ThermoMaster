#ifndef CAMERA_FLIR_H
#define CAMERA_FLIR_H

#include <QObject>
#include <QThread>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QDebug>

#include <QTime>
#include <QTimer>

#include "Spinnaker.h"
#include "SpinGenApi/SpinnakerGenApi.h"

#include "MsgHandler.h"

using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
using namespace std;

struct Image_FLIR {

    QString CameraName;
    qint64 frameId;
    qint64 timestamp;
    qint64 gain;
    QImage Img;

};

Q_DECLARE_METATYPE(Image_FLIR)

/* =================================================================== *\
|    LowLevel_FLIR Class                                                |
\* =================================================================== */

class LowLevel_FLIR : public QObject {

    Q_OBJECT

public:

    // Constructor and destructor
    LowLevel_FLIR(int);
    ~LowLevel_FLIR();

    int CamId;
    QString CamName;
    int Exposure;
    bool grabState;

public slots:

    void display_info();
    void grab();

signals:

    void newImage(Image_FLIR);
    // void finished();
    void closed();

private:

    // --- Internal FLIR properties
    SystemPtr FLIR_system;
    CameraList FLIR_camList;
    CameraPtr pCam;

};


/* =================================================================== *\
|    Camera_FLIR Class                                                  |
\* =================================================================== */

class Camera_FLIR : public QObject {

    Q_OBJECT

public:

    // Constructor and destructor
    Camera_FLIR(int);
    ~Camera_FLIR();

    float DisplayRate;
    float SaveRate;
    QRect ROI;

    int CamId;
    QString CamName;

public slots:

    void display_info();
    void newImage(Image_FLIR);

signals:

    void newImageForDisplay(QPixmap);

private:

    LowLevel_FLIR *Camera;
    QThread *t_Cam;

    Image_FLIR Image;
    qint64 tRefDisp;
    qint64 tRefSave;

};

#endif
