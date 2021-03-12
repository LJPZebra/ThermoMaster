#include "Camera_FLIR.h"

using namespace std;

/* =================================================================== *\
|    LowLevel_FLIR Class                                                |
\* =================================================================== */

/* === Constructor =================================================== */

LowLevel_FLIR::LowLevel_FLIR(int CamIdx) {

    // --- Times
    // Exposures are specified in microseconds
    // Timestamps are in nanoseconds

    // Initialization
    CamId = CamIdx;
    Exposure = 40000;
    grabState = false;

    // Camera initialization
    FLIR_system = System::GetInstance();
    FLIR_camList = FLIR_system->GetCameras();
    unsigned int FLIR_nCam = FLIR_camList.GetSize();

    if (FLIR_nCam) {

        pCam = FLIR_camList.GetByIndex(CamId);

        // --- Get camera identifier
        INodeMap &nodeMap = pCam->GetTLDeviceNodeMap();
        CCategoryPtr category = nodeMap.GetNode("DeviceInformation");
        if (IsAvailable(category) && IsReadable(category)) {
            FeatureList_t features;
            category->GetFeatures(features);

            QRegExp rx(" (\\w+)\-");
            QString model("");
            if (rx.indexIn(QString(features.at(3)->ToString())) != -1) { model += rx.cap(1) + " "; }
            CamName = model + features.at(1)->ToString();
        }

    }

}

/* === Destructor ==================================================== */

LowLevel_FLIR::~LowLevel_FLIR() {

    pCam->DeInit();
    Q_EMIT closed();

}

/* === Information display =========================================== */

void LowLevel_FLIR::display_info() {

    // --- Device info

    INodeMap &nodeMap = pCam->GetTLDeviceNodeMap();
    CCategoryPtr category = nodeMap.GetNode("DeviceInformation");

    if (IsAvailable(category) && IsReadable(category)) {

        FeatureList_t features;
        category->GetFeatures(features);

        qInfo().nospace() << "<table class='cameraInfo'>"
                << "<tr>"
                << "<th rowspan=3>Camera " << CamId << "</th>"
                << "<td>" << features.at(1)->GetName() << "</td>"       // DeviceSerialNumber
                << "<td>" << features.at(1)->ToString() << "</td>"
                << "</tr><tr>"
                << "<td>" << features.at(3)->GetName() << "</td>"       // DeviceModelName
                << "<td>" << features.at(3)->ToString() << "</td>"
                << "</tr><tr>"
                << "<td>" << features.at(12)->GetName() << "</td>"      // DeviceCurrentSpeed
                << "<td>" << features.at(12)->ToString() << "</td>"
                << "</tr>"
                << "</table>";

    } else { qWarning() << "Device info not available"; }

}

/* === Frame grabber ================================================= */

void LowLevel_FLIR::grab() {

    // Thread info
    qInfo().nospace() << THREAD << qPrintable(CamName) << " lives in thread: " << QThread::currentThreadId();

    return;

    // --- Camera & nodemaps definitions -----------------------------------

    pCam->Init();
    INodeMap &nodeMap = pCam->GetNodeMap();
    pCam->GainAuto.SetValue(Spinnaker::GainAutoEnums::GainAuto_Off);

    // --- Configure ChunkData ---------------------------------------------

    // --- Activate chunk mode

    CBooleanPtr ptrChunkModeActive = nodeMap.GetNode("ChunkModeActive");

    if (!IsAvailable(ptrChunkModeActive) || !IsWritable(ptrChunkModeActive)) {
        qWarning() << "Unable to activate chunk mode. Aborting.";
        return;
    }

    ptrChunkModeActive->SetValue(true);
    qInfo() << "Chunk mode activated";

    // --- Chunk data types

    NodeList_t entries;

    // Retrieve the selector node
    CEnumerationPtr ptrChunkSelector = nodeMap.GetNode("ChunkSelector");

    if (!IsAvailable(ptrChunkSelector) || !IsReadable(ptrChunkSelector)) {
        qWarning() << "Unable to retrieve chunk selector. Aborting.";
        return;
    }

    // Retrieve entries
    ptrChunkSelector->GetEntries(entries);

    for (int i = 0; i < entries.size(); i++) {

        // Select entry to be enabled
        CEnumEntryPtr ptrChunkSelectorEntry = entries.at(i);

        // Go to next node if problem occurs
        if (!IsAvailable(ptrChunkSelectorEntry) || !IsReadable(ptrChunkSelectorEntry)) { continue; }

        ptrChunkSelector->SetIntValue(ptrChunkSelectorEntry->GetValue());

        // Retrieve corresponding boolean
        CBooleanPtr ptrChunkEnable = nodeMap.GetNode("ChunkEnable");

        // Enable the boolean, thus enabling the corresponding chunk data
        if (IsWritable(ptrChunkEnable)) {
            ptrChunkEnable->SetValue(true);
        }

    }

    // --- Acquisition parameters ------------------------------------------

    // === Continuous mode ======================

    CEnumerationPtr ptrAcquisitionMode = nodeMap.GetNode("AcquisitionMode");
    if (!IsAvailable(ptrAcquisitionMode) || !IsWritable(ptrAcquisitionMode)) {
        qWarning() << "Unable to set acquisition mode to continuous (enum retrieval)";
        return;
    }

    // Retrieve entry node from enumeration node
    CEnumEntryPtr ptrAcquisitionModeContinuous = ptrAcquisitionMode->GetEntryByName("Continuous");
    if (!IsAvailable(ptrAcquisitionModeContinuous) || !IsReadable(ptrAcquisitionModeContinuous)) {
        qWarning() << "Unable to set acquisition mode to continuous (entry retrieval)";
        return;
    }

    // Retrieve integer value from entry node
    int64_t acquisitionModeContinuous = ptrAcquisitionModeContinuous->GetValue();

    // Set integer value from entry node as new value of enumeration node
    ptrAcquisitionMode->SetIntValue(acquisitionModeContinuous);

    qInfo() << "Acquisition mode set to continuous";

    // === Exposure time ========================

    CEnumerationPtr exposureAuto = nodeMap.GetNode("ExposureAuto");
    exposureAuto->SetIntValue(exposureAuto->GetEntryByName("Off")->GetValue());

    CEnumerationPtr exposureMode = nodeMap.GetNode("ExposureMode");
    exposureMode->SetIntValue(exposureMode->GetEntryByName("Timed")->GetValue());

    CFloatPtr exposureTime = nodeMap.GetNode("ExposureTime");
    exposureTime->SetValue(Exposure);

    qInfo() << "Exposure time set to " << Exposure/1000 << "ms";

    // --- Acquire images --------------------------------------------------

    grabState = true;
    qInfo() << "Starting image acquisition";

    pCam->BeginAcquisition();

    while (grabState) {

        ImagePtr pImg = pCam->GetNextImage();

        if (pImg->IsIncomplete()) {

            qWarning() << "Image incomplete with image status " << pImg->GetImageStatus();

        } else {

            Image_FLIR FImg;

            // --- Get image and convert to QImage
            unsigned char* Raw = (unsigned char*)pImg->GetData();
            FImg.Img = QImage(Raw, pImg->GetWidth(), pImg->GetHeight(), QImage::Format_Indexed8);
            for (unsigned int i=0 ; i<=255; i++) { FImg.Img.setColor(i, qRgb(i,i,i)); }

            // --- Get ChunkData
            ChunkData chunkData = pImg->GetChunkData();
            FImg.CameraName = CamName;
            FImg.timestamp = (qint64) chunkData.GetTimestamp();
            FImg.frameId = (qint64) chunkData.GetFrameID();
            FImg.gain = (qint64) chunkData.GetGain();

            emit newImage(FImg);
            //qDebug() << FImg.timestamp;

        }

        pImg->Release();

    }

    pCam->EndAcquisition();

    // emit finished();

}

/* =================================================================== *\
|    Camera_FLIR Class                                                  |
\* =================================================================== */

/* === Constructor =================================================== */

Camera_FLIR::Camera_FLIR(int CamIdx) {

    // Initialisation

    DisplayRate = 25;
    SaveRate = 25;
    ROI = QRect(0,0,1280,1024);

    CamId = CamIdx;
    Camera = new LowLevel_FLIR(CamId);
    tRefDisp = -1;
    tRefSave = -1;

    // Change camera thread
    t_Cam = new QThread;
    Camera->moveToThread(t_Cam);

    // Custom types registation
    qRegisterMetaType<Image_FLIR>();

    // Connections
    connect(t_Cam, SIGNAL(started()), Camera, SLOT(grab()));
    // connect(Camera, SIGNAL(finished()), t_Cam, SLOT (quit()));
    connect(Camera, SIGNAL(closed()), t_Cam, SLOT(quit()));
    connect(Camera, SIGNAL(closed()), t_Cam, SLOT(deleteLater()));
    connect(Camera, SIGNAL(closed()), Camera, SLOT(deleteLater()));
    connect(Camera, SIGNAL(newImage(Image_FLIR)), this, SLOT(newImage(Image_FLIR)));

    // Start the camera
    t_Cam->start();

    // Thread info
    qInfo().nospace() << THREAD << "Camera lives in thread: " << QThread::currentThreadId();

}

/* === Destructor ==================================================== */

Camera_FLIR::~Camera_FLIR() {}

/* === Information display =========================================== */

void Camera_FLIR::display_info() { Camera->display_info(); }

/* === New image received ============================================ */

void Camera_FLIR::newImage(Image_FLIR FImg) {

    // --- Initialize time references
    if (tRefDisp==-1) { tRefDisp = FImg.timestamp; }
    if (tRefSave==-1) { tRefSave = FImg.timestamp; }

    // --- Get image
    QPixmap Pixmap = QPixmap::fromImage(FImg.Img.copy(ROI));

    // --- Save image ? -------------------------------------------------

    // --- Display image ? ----------------------------------------------

    if (FImg.timestamp-tRefDisp >= 1e9/DisplayRate) {
        emit newImageForDisplay(Pixmap);
    }

}
