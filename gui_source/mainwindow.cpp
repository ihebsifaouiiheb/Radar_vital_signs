#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtEndian>
#include <QDialog>
#include <QSerialPortInfo>
#include <QFile>
#include <QElapsedTimer>                       // This class provides a fast way to calculate elapsed times
#include "dialogsettings.h"
#include <fstream> //zidtha
using namespace std;


#define LENGTH_MAGIC_WORD_BYTES         8  // Length of Magic Word appended to the UART packet from the EVM

#define LENGTH_HEADER_BYTES               40   // Header + Magic Word
#define LENGTH_TLV_MESSAGE_HEADER_BYTES   8
#define LENGTH_DEBUG_DATA_OUT_BYTES       128   // VitalSignsDemo_OutputStats size
#define MMWDEMO_OUTPUT_MSG_SEGMENT_LEN    32   // The data sent out through the UART has Extra Padding to make it a
                                                   // multiple of MMWDEMO_OUTPUT_MSG_SEGMENT_LEN
#define LENGTH_OFFSET_BYTES               LENGTH_HEADER_BYTES  - LENGTH_MAGIC_WORD_BYTES + LENGTH_TLV_MESSAGE_HEADER_BYTES
#define LENGTH_OFFSET_NIBBLES             2*LENGTH_OFFSET_BYTES

#define  INDEX_GLOBAL_COUNT                  6
#define  INDEX_RANGE_BIN_PHASE               1
#define  INDEX_RANGE_BIN_VALUE               2
#define  INDEX_PHASE                         5
#define  INDEX_BREATHING_WAVEFORM            6
#define  INDEX_HEART_WAVEFORM                7
#define  INDEX_HEART_RATE_EST_FFT            8
#define  INDEX_HEART_RATE_EST_FFT_4Hz        9
#define  INDEX_HEART_RATE_EST_FFT_xCorr      10
#define  INDEX_HEART_RATE_EST_PEAK           11
#define  INDEX_BREATHING_RATE_FFT            12
#define  INDEX_BREATHING_RATE_xCorr          13
#define  INDEX_BREATHING_RATE_PEAK           14
#define  INDEX_CONFIDENCE_METRIC_BREATH      15
#define  INDEX_CONFIDENCE_METRIC_BREATH_xCorr 16
#define  INDEX_CONFIDENCE_METRIC_HEART       17
#define  INDEX_CONFIDENCE_METRIC_HEART_4Hz   18
#define  INDEX_CONFIDENCE_METRIC_HEART_xCorr 19
#define  INDEX_ENERGYWFM_BREATH              20
#define  INDEX_ENERGYWFM_HEART               21
#define  INDEX_MOTION_DETECTION              22
#define  INDEX_BREATHING_RATE_HARM_ENERG     23
#define  INDEX_HEART_RATE_HARM_ENERG         24
#define  INDEX_RESERVED7                     25
#define  INDEX_RESERVED8                     26
#define  INDEX_RESERVED9                     27
#define  INDEX_RESERVED10                    28
#define  INDEX_RESERVED11                    29
#define  INDEX_RESERVED12                    30
#define  INDEX_RESERVED13                    31
#define  INDEX_RESERVED14                    32


#define  INDEX_RANGE_PROFILE_START           35//(LENGTH_DEBUG_DATA_OUT_BYTES+LENGTH_TLV_MESSAGE_HEADER_BYTES)/4  + 1

#define  INDEX_IN_GLOBAL_FRAME_COUNT                    INDEX_GLOBAL_COUNT*8
#define  INDEX_IN_RANGE_BIN_INDEX                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RANGE_BIN_PHASE*8 + 4
#define  INDEX_IN_DATA_CONFIDENCE_METRIC_HEART_4Hz      LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_CONFIDENCE_METRIC_HEART_4Hz*8
#define  INDEX_IN_DATA_CONFIDENCE_METRIC_HEART_xCorr    LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_CONFIDENCE_METRIC_HEART_xCorr*8
#define  INDEX_IN_DATA_PHASE                            LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_PHASE*8
#define  INDEX_IN_DATA_BREATHING_WAVEFORM               LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_BREATHING_WAVEFORM*8
#define  INDEX_IN_DATA_HEART_WAVEFORM                   LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_HEART_WAVEFORM*8
#define  INDEX_IN_DATA_BREATHING_RATE_FFT               LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_BREATHING_RATE_FFT*8
#define  INDEX_IN_DATA_HEART_RATE_EST_FFT               LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_HEART_RATE_EST_FFT*8
#define  INDEX_IN_DATA_HEART_RATE_EST_FFT_4Hz           LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_HEART_RATE_EST_FFT_4Hz*8
#define  INDEX_IN_DATA_HEART_RATE_EST_FFT_xCorr         LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_HEART_RATE_EST_FFT_xCorr*8
#define  INDEX_IN_DATA_BREATHING_RATE_PEAK              LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_BREATHING_RATE_PEAK*8
#define  INDEX_IN_DATA_HEART_RATE_EST_PEAK              LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_HEART_RATE_EST_PEAK*8
#define  INDEX_IN_DATA_CONFIDENCE_METRIC_BREATH         LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_CONFIDENCE_METRIC_BREATH*8
#define  INDEX_IN_DATA_CONFIDENCE_METRIC_HEART          LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_CONFIDENCE_METRIC_HEART*8
#define  INDEX_IN_DATA_ENERGYWFM_BREATH                 LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_ENERGYWFM_BREATH*8
#define  INDEX_IN_DATA_ENERGYWFM_HEART                  LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_ENERGYWFM_HEART*8
#define  INDEX_IN_DATA_MOTION_DETECTION_FLAG            LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES  + INDEX_MOTION_DETECTION*8
#define  INDEX_IN_DATA_CONFIDENCE_METRIC_BREATH_xCorr   LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES  + INDEX_CONFIDENCE_METRIC_BREATH_xCorr*8
#define  INDEX_IN_DATA_BREATHING_RATE_HARM_ENERGY       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_BREATHING_RATE_HARM_ENERG*8
#define  INDEX_IN_DATA_BREATHING_RATE_xCorr             LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_BREATHING_RATE_xCorr*8
#define  INDEX_IN_DATA_RESERVED7                        LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED7*8
#define  INDEX_IN_DATA_RESERVED8                        LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED8*8
#define  INDEX_IN_DATA_RESERVED9                        LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED9*8
#define  INDEX_IN_DATA_RESERVED10                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED10*8
#define  INDEX_IN_DATA_RESERVED11                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED11*8
#define  INDEX_IN_DATA_RESERVED12                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED12*8
#define  INDEX_IN_DATA_RESERVED13                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED13*8
#define  INDEX_IN_DATA_RESERVED14                       LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RESERVED14*8

#define  INDEX_IN_DATA_RANGE_PROFILE_START          LENGTH_MAGIC_WORD_BYTES + LENGTH_OFFSET_NIBBLES + INDEX_RANGE_PROFILE_START*8


#define NUM_PTS_RRSIGNAL_PLOT        (60)
#define NUM_PTS_DISTANCE_TIME_PLOT        (256)
#define HEART_RATE_EST_MEDIAN_FLT_SIZE    (150)
#define HEART_RATE_EST_FINAL_OUT_SIZE     (200)
#define THRESH_HEART_CM                   (0.25)
#define THRESH_BREATH_CM                  (1.0)
#define BACK_THRESH_BPM                   (4)
#define BACK_THRESH_CM                    (0.20)
#define BACK_THRESH_4Hz_CM                (0.15)
#define THRESH_BACK                       (30)
#define THRESH_DIFF_EST                   (20)
#define ALPHA_HEARTRATE_CM                (0.2)
#define ALPHA_RCS                         (0.2)
#define APPLY_KALMAN_FILTER               (1.0)

float BREATHING_PLOT_MAX_YAXIS;
float HEART_PLOT_MAX_YAXIS;

QSerialPort *serialRead, *serialWrite;
bool  FileSavingFlag;                   // "True" if we want to save the data on to a text file
bool  serialPortFound_Flag ;            // "True" if Serial port found
bool  FlagSerialPort_Connected, dataPort_Connected, userPort_Connected;
int lastindb=0,lastindh=0;//zithom lel peaket
float thresh_breath, thresh_heart;
int TheFirstFrame=1;
int heartBufferSize,breathingBufferSize;
// GUI Status
enum  gui_status { gui_running, gui_stopped, gui_paused };
gui_status current_gui_status;

QSettings settings("Texas Instruments", "Vital Signs");
QFile outFile("dataOutputFromEVM.bin");     // File to save the Data
int timeOfDay=QDateTime::currentMSecsSinceEpoch() - QDateTime(QDate::currentDate()).toMSecsSinceEpoch();
std::ofstream myfile;
QElapsedTimer timer;       // For computing the time required for the operations
//int previous_time=0;
bool breathOrheart=1;
QCPTextElement *myTitle_HeartWfm;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Displays TI Logo
    myfile.open("C:/Users/ISIFAOUI/Desktop/68xx_vital_signs/Dataset/"+std::to_string(timeOfDay)+".csv");

    QPixmap pic(":/ti_logo_trimmed.PNG");
    if(pic.isNull())
    {
    qDebug()<<"logo not Found";
    }
    else
    {
    ui->pushButton_Icon->setIcon(QIcon(pic));
    }

    localCount = 0;
    currIndex  = 0;
    qDebug() <<"QT version = " <<QT_VERSION_STR;

    qint32 baudRate = 115200;//921600//115200
    FLAG_PAUSE = false;
    outFile.open(QIODevice::ReadWrite);  // Open the file by default
    AUTO_DETECT_COM_PORTS = ui->checkBox_AutoDetectPorts->isChecked();


    if(AUTO_DETECT_COM_PORTS)
      {
    /* Serial Port Settings */
     serialPortFound_Flag = serialPortFind();
     if (serialPortFound_Flag)
     {
        qDebug()<<"Serial Port Found";
        qDebug()<<"Data Port Number is" << dataPortNum;
        qDebug()<<"User Port Number is" << userPortNum;
     }
    }

     serialRead = new QSerialPort(this);
     dataPort_Connected = serialPortConfig(serialRead, baudRate, dataPortNum);
     if (dataPort_Connected)
         qDebug()<<"Data port succesfully Open";
     else
         qDebug()<<"data port did Not Open";

     serialWrite = new QSerialPort(this);
     userPort_Connected = serialPortConfig(serialWrite, 115200, userPortNum);
     if (userPort_Connected)
         qDebug()<<"User port succesfully Open";
     else
         qDebug()<<"User port did not Open ";

     connect(serialRead,SIGNAL(readyRead()),this,SLOT(serialRecieved()));

    // Plot Settings
    QFont font;
    font.setPointSize(12);
    QPen myPen;
    myPen.setWidth(1.5);    // Width greater than 1 considerably slows down the application
    myPen.setColor(Qt::blue);
    QFont titleFont;
    titleFont.setPointSize(15);
    titleFont.bold();

    ui->checkBox_LoadConfig->setChecked(true);
    statusBar()->showMessage(tr("Ready"));

    xRRsignal.resize(NUM_PTS_RRSIGNAL_PLOT);
    xDistTimePlot.resize(NUM_PTS_DISTANCE_TIME_PLOT);
    yDistTimePlot.resize(NUM_PTS_DISTANCE_TIME_PLOT);
    jjsignalBuffer.resize(NUM_PTS_RRSIGNAL_PLOT);
    breathingWfmBuffer.resize(NUM_PTS_DISTANCE_TIME_PLOT);
    heartWfmBuffer.resize(NUM_PTS_DISTANCE_TIME_PLOT);

    ui->plot_RangeProfile->addGraph(0);
    ui->plot_RangeProfile->setBackground(this->palette().background().color());
    ui->plot_RangeProfile->axisRect()->setBackground(this->palette().background().color());
    ui->plot_RangeProfile->xAxis->setLabel("Range (m)");
    ui->plot_RangeProfile->xAxis->setLabelFont(font);
    ui->plot_RangeProfile->yAxis->setLabel("Magnitude (a.u.)");
    ui->plot_RangeProfile->yAxis->setLabelFont(font);
    ui->plot_RangeProfile->axisRect()->setupFullAxesBox();
    ui->plot_RangeProfile->xAxis->setRange(0,NUM_PTS_DISTANCE_TIME_PLOT);

    QCPTextElement *myTitle = new QCPTextElement(ui->plot_RangeProfile, "Range Profile");
    myTitle->setFont(titleFont);
    ui->plot_RangeProfile->plotLayout()->insertRow(0); // inserts an empty row above the default axis rect
    ui->plot_RangeProfile->plotLayout()->addElement(0, 0, myTitle);

    ui->phaseWfmPlot->addGraph(0);
    ui->phaseWfmPlot->setBackground(this->palette().background().color());
    ui->phaseWfmPlot->axisRect()->setBackground(this->palette().background().color());
    ui->phaseWfmPlot->xAxis->setLabel("Frame (Index)");
    ui->phaseWfmPlot->xAxis->setLabelFont(font);
    ui->phaseWfmPlot->yAxis->setLabel("Displacement (a.u.)");
    ui->phaseWfmPlot->yAxis->setLabelFont(font);
    ui->phaseWfmPlot->axisRect()->setupFullAxesBox();
    ui->phaseWfmPlot->xAxis->setRange(0,NUM_PTS_DISTANCE_TIME_PLOT);
    ui->phaseWfmPlot->graph(0)->setPen(myPen);

    QCPTextElement *myTitle_ChestDisp = new QCPTextElement(ui->phaseWfmPlot, "Chest Displacement");
    myTitle_ChestDisp->setFont(titleFont);
    ui->phaseWfmPlot->plotLayout()->insertRow(0); // inserts an empty row above the default axis rect
    ui->phaseWfmPlot->plotLayout()->addElement(0, 0, myTitle_ChestDisp);

    ui->BreathingWfmPlot->addGraph(0);
    ui->BreathingWfmPlot->setBackground(this->palette().background().color());
    ui->BreathingWfmPlot->axisRect()->setBackground(this->palette().background().color());
    ui->BreathingWfmPlot->xAxis->setLabelFont(font);
    ui->BreathingWfmPlot->yAxis->setLabel("JJ interval(ms)");
    ui->BreathingWfmPlot->yAxis->setLabelFont(font);
    ui->BreathingWfmPlot->axisRect()->setupFullAxesBox();
    ui->BreathingWfmPlot->xAxis->setRange(0,NUM_PTS_DISTANCE_TIME_PLOT);
    ui->BreathingWfmPlot->graph(0)->setPen(myPen);

    QCPTextElement *myTitle_BreathWfm = new QCPTextElement(ui->BreathingWfmPlot, "JJ Signal");
    myTitle_BreathWfm->setFont(titleFont);
    ui->BreathingWfmPlot->plotLayout()->insertRow(0); // inserts an empty row above the default axis rect
    ui->BreathingWfmPlot->plotLayout()->addElement(0, 0, myTitle_BreathWfm);

    ui->heartWfmPlot->addGraph(0);
    ui->heartWfmPlot->setBackground(this->palette().background().color());
    ui->heartWfmPlot->axisRect()->setBackground(this->palette().background().color());
    ui->heartWfmPlot->xAxis->setLabelFont(font);
    ui->heartWfmPlot->yAxis->setLabel("Phase (radians)");
    ui->heartWfmPlot->yAxis->setLabelFont(font);
    ui->heartWfmPlot->axisRect()->setupFullAxesBox();
    ui->heartWfmPlot->xAxis->setRange(0,NUM_PTS_DISTANCE_TIME_PLOT);
    ui->heartWfmPlot->graph(0)->setPen(myPen);

    QPalette lcdpaletteBreathing = ui->lcdNumber_Breathingrate->palette();
    lcdpaletteBreathing.setColor(QPalette::Normal, QPalette::Window, Qt::white);

    QPalette lcdpaletteHeart = ui->lcdNumber_HeartRate->palette();
    lcdpaletteHeart.setColor(QPalette::Normal, QPalette::Window, Qt::white);

    /*QPalette lcdpaletteNotBreathing = ui->lcdNumber_Breathingrate->palette();
    lcdpaletteNotBreathing.setColor(QPalette::Normal, QPalette::Window, Qt::red);*/

    ui->lcdNumber_HeartRate->setAutoFillBackground(true);
    ui->lcdNumber_HeartRate->setPalette(lcdpaletteHeart);
    ui->lcdNumber_Breathingrate->setAutoFillBackground(true);
    ui->lcdNumber_Breathingrate->setPalette(lcdpaletteBreathing);

    myTitle_HeartWfm = new QCPTextElement(ui->heartWfmPlot, "Heart Waveform");
    myTitle_HeartWfm->setFont(titleFont);
    ui->heartWfmPlot->plotLayout()->insertRow(0); // inserts an empty row above the default axis rect
    ui->heartWfmPlot->plotLayout()->addElement(0, 0, myTitle_HeartWfm);

    ui->lineEdit_ProfileBack->setText("xwr1642_profile_VitalSigns_20fps_Back.cfg");
    ui->lineEdit_ProfileFront->setText("xwr1642_profile_VitalSigns_20fps_Front.cfg");
//  ui->lineEdit_ProfileBack->setText("xwr1443_profile_VitalSigns_20fps_Back.cfg");
//  ui->lineEdit_ProfileFront->setText("xwr1443_profile_VitalSigns_20fps_Front.cfg");
    connect(this,SIGNAL(gui_statusChanged()),this,SLOT(gui_statusUpdate()));
}


void MainWindow::serialRecieved()
{
    QByteArray dataSerial = serialRead->readAll().toHex();
    int dataSize = dataSerial.size();
    dataBuffer = dataBuffer.append(dataSerial);
    indexBuffer = indexBuffer + dataSize;
    if(dataBuffer.size()>=2500){
        processData();
    }
}


MainWindow::~MainWindow()
{
  serialWrite->write("sensorStop\n");
  serialWrite->waitForBytesWritten(10000);
  serialRead->close();
  serialWrite->close();
  delete ui;
}

void MainWindow::on_pushButton_start_clicked()
{
    localCount = 0;
    if(current_gui_status == gui_paused)
    {
        current_gui_status = gui_running;
        emit gui_statusChanged();
        return;
    }

    FLAG_PAUSE = false;
    AUTO_DETECT_COM_PORTS = ui->checkBox_AutoDetectPorts->isChecked();


   if (AUTO_DETECT_COM_PORTS)
    {
        if (!FlagSerialPort_Connected)
        {
            serialPortFound_Flag = serialPortFind();

            if( ! userPort_Connected)
            {
                userPort_Connected = serialPortConfig(serialWrite, 115200, userPortNum);
            }

            if( ! dataPort_Connected)
            {
                dataPort_Connected = serialPortConfig(serialRead, 115200, dataPortNum);
            }
        }
    }
    else
    {
        userPortNum = ui->lineEdit_UART_port->text();
        dataPortNum = ui->lineEdit_data_port->text();
        userPort_Connected = serialPortConfig(serialWrite, 115200, userPortNum);
        dataPort_Connected = serialPortConfig(serialRead, 115200, dataPortNum);
    }


    if (ui->checkBox_LoadConfig->isChecked())
    {


   // Read the Configuration file path from the GUI

       //QString profileBack =  ui->lineEdit_ProfileBack->text();
       //qDebug() << "Configuration File Name Read from the GUI is"<<profileBack<<endl;


   DialogSettings myDialogue;
   QString userComPort = myDialogue.getUserComPortNum();
   //qDebug()<<userComPort;
   QDir currDir = QCoreApplication::applicationDirPath();
   currDir.cdUp();
   QString filenameText = currDir.path();
   filenameText.append("/profiles/");        // Reads from this path

    if (ui->radioButton_BackMeasurements->isChecked())
    {
      filenameText.append(ui->lineEdit_ProfileBack->text());        // Reads from this path
      qDebug() << "\nConf Path is"<<filenameText;

      HEART_PLOT_MAX_YAXIS    = myDialogue.getHeartWfm_yAxisMax();
      BREATHING_PLOT_MAX_YAXIS= myDialogue.getBreathWfm_yAxisMax();

      thresh_breath = 0.001;
      thresh_heart  = 0.001;
    }
    else
    {
      filenameText.append(ui->lineEdit_ProfileFront->text());        // Reads from this path
      qDebug() << "Configuration File Path is "<<filenameText;
      thresh_breath = 10;//10;
      thresh_heart = 0;//0.1;
      BREATHING_PLOT_MAX_YAXIS = 1.0;
      HEART_PLOT_MAX_YAXIS = 0.5;
    }
    ui->SpinBox_TH_Breath->setValue(thresh_breath);
    ui->SpinBox_TH_Heart->setValue(thresh_heart);

    QFile infile(filenameText);
    if (infile.open(QIODevice::ReadWrite))
    {
        QTextStream inStream(&infile);
        while (!inStream.atEnd())
        {
            QString line = inStream.readLine();  // Read a line from the input Text File
            qDebug() << line;
            serialWrite->write(line.toUtf8().constData());
            serialWrite->write("\n");
            serialWrite->waitForBytesWritten(10000);
            Sleep(150);
        }
        Sleep(3000);
        infile.close();
    }

// Parse the Configuration Text File
    if (infile.open(QIODevice::ReadWrite))
    {
        QStringList listArgs;
        QString tempString;
        QTextStream inStream(&infile);
        while (!inStream.atEnd())
        {
            QString line = inStream.readLine();  // Read a line from the input Text File
            //qDebug() << line;
            if (line.contains("vitalSignsCfg", Qt::CaseInsensitive) )
            {
                  listArgs = line.split(QRegExp("\\s+"));
                  tempString = listArgs.at(1);
                  demoParams.rangeStartMeters = tempString.toFloat();
                  qDebug() << demoParams.rangeStartMeters;

                  listArgs = line.split(QRegExp("\\s+"));
                  tempString = listArgs.at(2);
                  demoParams.rangeEndMeters = tempString.toFloat();
                  qDebug() << demoParams.rangeEndMeters;

                  listArgs = line.split(QRegExp("\\s+"));
                  tempString = listArgs.at(5);
                  demoParams.AGC_thresh = tempString.toFloat();
                  qDebug() << demoParams.AGC_thresh;

                  listArgs = line.split(QRegExp("\\s+"));
                  tempString = listArgs.at(3);
                  breathingBufferSize=tempString.toFloat();

                  listArgs = line.split(QRegExp("\\s+"));
                  tempString = listArgs.at(4);
                  heartBufferSize = tempString.toFloat();
            }

            if (line.contains("profileCfg", Qt::CaseInsensitive) )
            {
                listArgs = line.split(QRegExp("\\s+"));
                tempString = listArgs.at(2);
                demoParams.stratFreq_GHz = tempString.toFloat();

                listArgs = line.split(QRegExp("\\s+"));
                tempString = listArgs.at(8);
                demoParams.freqSlope_MHZ_us = tempString.toFloat();

                listArgs = line.split(QRegExp("\\s+"));
                tempString = listArgs.at(10);
                demoParams.numSamplesChirp = tempString.toFloat();

                listArgs = line.split(QRegExp("\\s+"));
                tempString = listArgs.at(11);
                demoParams.samplingRateADC_ksps = tempString.toInt();
            }
        }
        infile.close();
        // Compute the Derived Configuration Parameters
        demoParams.chirpDuration_us = 1e3*demoParams.numSamplesChirp/demoParams.samplingRateADC_ksps;
        qDebug() << "\nChirp Duration in us is :" << demoParams.chirpDuration_us;

        demoParams.chirpBandwidth_kHz = (demoParams.freqSlope_MHZ_us)*(demoParams.chirpDuration_us);
        qDebug() << "Chirp Bandwidth in kHz is : "<<demoParams.chirpBandwidth_kHz;

        float numTemp = (demoParams.chirpDuration_us)*(demoParams.samplingRateADC_ksps)*(3e8);
        float denTemp =  2*(demoParams.chirpBandwidth_kHz);
        demoParams.rangeMaximum_meters = numTemp/(denTemp*1e9);
        qDebug() << "Maximum Range in Meters is :"<< demoParams.rangeMaximum_meters;

        demoParams.rangeFFTsize = nextPower2(demoParams.numSamplesChirp);
        qDebug() << "Range-FFT size is :"<< demoParams.rangeFFTsize;

        demoParams.rangeBinSize_meters = demoParams.rangeMaximum_meters/demoParams.rangeFFTsize;
        qDebug() << "Range-Bin size is :"<< demoParams.rangeBinSize_meters;

        demoParams.rangeBinStart_index = demoParams.rangeStartMeters/demoParams.rangeBinSize_meters;
        demoParams.rangeBinEnd_index   = demoParams.rangeEndMeters/demoParams.rangeBinSize_meters;
        qDebug() << "Range-Bin Start Index is :"<< demoParams.rangeBinStart_index;
        qDebug() << "Range-Bin End Index is :"  << demoParams.rangeBinEnd_index;

        demoParams.numRangeBinProcessed = demoParams.rangeBinEnd_index - demoParams.rangeBinStart_index + 1;

        // Calculate the Payload size

        demoParams.totalPayloadSize_bytes = LENGTH_HEADER_BYTES;
        demoParams.totalPayloadSize_bytes += LENGTH_TLV_MESSAGE_HEADER_BYTES + (4*demoParams.numRangeBinProcessed);
        demoParams.totalPayloadSize_bytes += LENGTH_TLV_MESSAGE_HEADER_BYTES +  LENGTH_DEBUG_DATA_OUT_BYTES;
        qDebug() << "Total Payload size from the UART is :"<< demoParams.totalPayloadSize_bytes;

        if ((demoParams.totalPayloadSize_bytes % MMWDEMO_OUTPUT_MSG_SEGMENT_LEN) != 0)
        {
            int paddingFactor = ceil( (float) demoParams.totalPayloadSize_bytes/ (float) MMWDEMO_OUTPUT_MSG_SEGMENT_LEN);
            qDebug() << "Padding Factor is :"<<paddingFactor;

            demoParams.totalPayloadSize_bytes = MMWDEMO_OUTPUT_MSG_SEGMENT_LEN*paddingFactor;
        }

        qDebug() << "Total Payload size from the UART is :"<< demoParams.totalPayloadSize_bytes;
        demoParams.totalPayloadSize_nibbles = 2 * demoParams.totalPayloadSize_bytes;
       }
    }
    ui->heartWfmPlot->yAxis->setRange(-HEART_PLOT_MAX_YAXIS,HEART_PLOT_MAX_YAXIS);
    for(int i=0;i<NUM_PTS_RRSIGNAL_PLOT;++i){
        xRRsignal[i]=i;
    }
    current_gui_status = gui_running;
    emit gui_statusChanged();
    timer.start();

    //myfile<<"phaseWfm_Out"<<','<<"breathWfm_Out"<<','<<"heartWfm_Out"<<','<<"BreathingRate_Out"<<','<<"heartRate_Out"<<','<<"HR_PK"<<','<<"HR_corr"<<','<<"HR_FFT_2hz"<<','<<"HR_FFT_4hz"<<','<<"HR_xCorr_CM"<<','<<"HR_2hz_CM"<<','<<"HR_4Hz_CM\n";
   //myfile<<"JJ Signal"<<','<<"accumuler\n";
}

void MainWindow::on_pushButton_stop_clicked()
{
     serialWrite->write("sensorStop\n");
     ui->checkBox_LoadConfig->setChecked(true);
     current_gui_status = gui_stopped;
     emit gui_statusChanged();
     myfile.close();
}


void MainWindow::on_pushButton_Refresh_clicked()
{
     serialWrite->write("guiMonitor 0 0 1 1\n");
     serialWrite->waitForBytesWritten(10000);
     localCount = 0;
     Sleep(1300);
     breathOrheart = 1-breathOrheart;
     if(breathOrheart){
         myTitle_HeartWfm->setText("Heart Waveform");// = new QCPTextElement(ui->heartWfmPlot, "Heart Waveform");
         ui->heartWfmPlot->plotLayout()->addElement(0, 0, myTitle_HeartWfm);
         ui->heartWfmPlot->plotLayout()->updateLayout();
     }else{
         myTitle_HeartWfm->setText("Breath Waveform");// = new QCPTextElement(ui->heartWfmPlot, "Heart Waveform");
         ui->heartWfmPlot->plotLayout()->addElement(0, 0, myTitle_HeartWfm);
         ui->heartWfmPlot->plotLayout()->updateLayout();
     }
}

void MainWindow::on_pushButton_settings_clicked()
{
    DialogSettings settingsDialogue;
    settingsDialogue.setModal(true);
    settingsDialogue.exec();
}


int MainWindow::nextPower2(int num)
{
    int power = 1;
    while(power < num)
        power*=2;
    return power;
}


float  MainWindow::parseValueFloat(QByteArray data, int valuePos, int valueSize)
{
    bool ok;
    QByteArray parseData;
    parseData = data.mid(valuePos,valueSize);
    QString strParseData = parseData;
    quint32 temp_int = strParseData.toUInt(&ok,16);
    temp_int =  qToBigEndian(temp_int);        // Convert to Big-Endian
    float parseValueOut;
    parseValueOut = *reinterpret_cast<float*>(&temp_int); // cast to Float
    return parseValueOut;
}

quint32  MainWindow::parseValueUint32(QByteArray data, int valuePos, int valueSize)
{
    bool ok;
    QByteArray parseData;
    parseData = data.mid(valuePos, valueSize);
    QString strParseData = parseData;
    quint32 tempInt32 = strParseData.toUInt(&ok,16);
    quint32 parseValueOut =  qToBigEndian(tempInt32);        // Convert to Big-Endian
    return parseValueOut;
}

quint16 MainWindow::parseValueUint16(QByteArray data, int valuePos, int valueSize)
{
    bool ok;
    QByteArray parseData;
    parseData = data.mid(valuePos, valueSize);
    QString strParseData = parseData;
    quint16 parseValueOut = strParseData.toInt(&ok,16);
    parseValueOut =  qToBigEndian(parseValueOut);        // Convert to Big-Endian
    return parseValueOut;
}

qint16  MainWindow::parseValueInt16(QByteArray data, int valuePos, int valueSize)
{
    bool ok;
    QByteArray parseData;
    parseData = data.mid(valuePos, valueSize);
    QString strParseData = parseData;
    qint16 parseValueOut = strParseData.toInt(&ok,16);
    parseValueOut =  qToBigEndian(parseValueOut);        // Convert to Big-Endian
    return parseValueOut;
}


bool MainWindow::serialPortFind()
{
    DialogSettings dialogBoxSerial;/*
    // Find available COM ports on the computer
    QString portNum;
    QString portNumt[3];
    int i=0;
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        portNum = serialPortInfo.portName();
        portNumt[i]=portNum;
        i++;
    }
    dialogBoxSerial.setDataComPortNum(portNumt[0]);
    dialogBoxSerial.setUserComPortNum(portNumt[1]);
*/
    dataPortNum = dialogBoxSerial.getDataComPortNum();
    userPortNum = dialogBoxSerial.getUserComPortNum();
    ui->lineEdit_data_port->setText(dataPortNum);
    ui->lineEdit_UART_port->setText(userPortNum);
    if (dataPortNum.isEmpty())
    return 0;
    else
    return 1;
}

bool MainWindow::serialPortConfig(QSerialPort *serial, qint32 baudRate, QString dataPortNum )
{
    serial->setPortName(dataPortNum);
    if(serial->open(QIODevice::ReadWrite) )
      {
      FlagSerialPort_Connected = 1;
      }
    else
      {
      FlagSerialPort_Connected = 0;
      return FlagSerialPort_Connected;
      }
    serial->setBaudRate(baudRate);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    return FlagSerialPort_Connected;
}

void MainWindow::processData()
{
    QByteArray dataSave;
    QByteArray data;
    static float outHeartNew_CM;
    static float maxRCS_updated;
    bool MagicOk;

    // Kalman Filter
    static float Pk = 1;
    static float xk = 0, xb = 27;   // State Variable

    extern bool FileSavingFlag;
    FileSavingFlag = ui->checkBox_SaveData->isChecked();
    while (dataBuffer.size() >= demoParams.totalPayloadSize_nibbles+300)
    {
    //myfile<<"Data size ok,";
//    QElapsedTimer timer;       // For computing the time required for the operations
//    timer.start();

    QByteArray searchStr("0201040306050807");  // Search String Array
    int dataStartIndex = dataBuffer.indexOf(searchStr);
    int indexTemp = localCount % NUM_PTS_DISTANCE_TIME_PLOT;
    if (dataStartIndex == -1)
    {
        //myfile<<"Magic word doesn't exists,";
        MagicOk = 0;
        qDebug()<<" Magic Word Not Found --- local Count:  " <<localCount << "DataBufferSize" <<dataBuffer.size();
        break;
    }
    else
    {
        data       = dataBuffer.mid(dataStartIndex, demoParams.totalPayloadSize_nibbles /*TOTAL_PAYLOAD_SIZE_NIBBLES*/);
        dataBuffer = dataBuffer.remove(dataStartIndex, demoParams.totalPayloadSize_nibbles /*TOTAL_PAYLOAD_SIZE_NIBBLES*/);

        dataBuffer = dataBuffer.remove(0, dataStartIndex);
//      qDebug()<<"dataBuffer size after removal is "<<dataBuffer.size();

        // Check if all the data has been recieved succesfully
        if (data.size() >= demoParams.totalPayloadSize_nibbles)
        {
            MagicOk = 1;
            statusBar()->showMessage(tr("Sensor Running"));
        }
        else
        {
            //myfile<<",Data size shortage\n";
            MagicOk = 0;
            qDebug()<<" Data size is not OK --- local Count:  " <<localCount << "DataBufferSize" <<dataBuffer.size();
            qDebug()<< data.size();
            statusBar()->showMessage(tr("Frames are being missed. Please stop and Start the Program"));
        }
    }

  if (MagicOk == 1)
  {
   if(FileSavingFlag)
   {
       dataSave = data;
       outFile.write(dataSave.fromHex(data), demoParams.totalPayloadSize_bytes /*TOTAL_PAYLOAD_SIZE_BYTES*/);
   }

   quint32  globalCountOut     = parseValueUint32(data,40, 8);  // Global Count
   quint16  rangeBinIndexOut   = parseValueUint16(data,INDEX_IN_RANGE_BIN_INDEX, 4);  // parseValueUint16(data, 28, 4)
   float BreathingRate_FFT     = parseValueFloat(data, INDEX_IN_DATA_BREATHING_RATE_FFT, 8);  // Breathing Rate
   float BreathingRatePK_Out   = parseValueFloat(data, INDEX_IN_DATA_BREATHING_RATE_PEAK, 8);
   float heartRate_FFT         = parseValueFloat(data, INDEX_IN_DATA_HEART_RATE_EST_FFT, 8);  // Heart Rate
   float heartRate_Pk          = parseValueFloat(data, INDEX_IN_DATA_HEART_RATE_EST_PEAK, 8);
   float heartRate_xCorr       = parseValueFloat(data, INDEX_IN_DATA_HEART_RATE_EST_FFT_xCorr, 8);   // Heart Rate - AutoCorrelation
   float heartRate_FFT_4Hz     = parseValueFloat(data, INDEX_IN_DATA_HEART_RATE_EST_FFT_4Hz, 8);   // Heart Rate - FFT-4Hz
   float phaseWfm_Out          = parseValueFloat(data, INDEX_IN_DATA_PHASE, 8); // Phase Waveforms
   float breathWfm_Out         = parseValueFloat(data, INDEX_IN_DATA_BREATHING_WAVEFORM, 8);   // Breathing Waveform
   float heartWfm_Out          = parseValueFloat(data, INDEX_IN_DATA_HEART_WAVEFORM, 8);   // Cardiac Waveform
   float breathRate_CM         = parseValueFloat(data, INDEX_IN_DATA_CONFIDENCE_METRIC_BREATH, 8);  // Confidence Metric
   float heartRate_2hz_CM         = parseValueFloat(data, INDEX_IN_DATA_CONFIDENCE_METRIC_HEART, 8);
   float heartRate_4Hz_CM         = parseValueFloat(data, INDEX_IN_DATA_CONFIDENCE_METRIC_HEART_4Hz, 8);
   float heartRate_xCorr_CM       = parseValueFloat(data, INDEX_IN_DATA_CONFIDENCE_METRIC_HEART_xCorr, 8);
   float outSumEnergyBreathWfm    = parseValueFloat(data, INDEX_IN_DATA_ENERGYWFM_BREATH, 8);  // Waveforms Energy
   float outSumEnergyHeartWfm     = parseValueFloat(data, INDEX_IN_DATA_ENERGYWFM_HEART, 8);  // Waveforms Energy
   float outMotionDetectionFlag   = parseValueFloat(data, INDEX_IN_DATA_MOTION_DETECTION_FLAG, 8);
   float BreathingRate_xCorr_CM   = parseValueFloat(data, INDEX_IN_DATA_CONFIDENCE_METRIC_BREATH_xCorr, 8);
   float BreathingRate_HarmEnergy = parseValueFloat(data, INDEX_IN_DATA_BREATHING_RATE_HARM_ENERGY, 8);
   float BreathingRate_xCorr = parseValueFloat(data, INDEX_IN_DATA_BREATHING_RATE_xCorr, 8);
   float reserved7 = parseValueFloat(data, INDEX_IN_DATA_RESERVED7, 8);
   float reserved8 = parseValueFloat(data, INDEX_IN_DATA_RESERVED8, 8);
   float reserved9 = parseValueFloat(data, INDEX_IN_DATA_RESERVED9, 8);
   float reserved10 = parseValueFloat(data, INDEX_IN_DATA_RESERVED10, 8);
   float reserved11 = parseValueFloat(data, INDEX_IN_DATA_RESERVED11, 8);
   float reserved12 = parseValueFloat(data, INDEX_IN_DATA_RESERVED12, 8);
   float reserved13 = parseValueFloat(data, INDEX_IN_DATA_RESERVED13, 8);
   float reserved14 = parseValueFloat(data, INDEX_IN_DATA_RESERVED14, 8);

   qDebug()<<reserved7<<' '<<reserved8<<' '<<reserved9<<' '<<reserved10<<' '<<reserved11<<' '<<reserved12<<' '<<reserved13<<' '<<reserved14;
   //myfile<<BreathingRate_FFT<<';'<<heartRate_Pk<<';'<<BreathingRate_xCorr_CM;//BreathingRate_xCorr<<';'<<heartRate_4Hz_CM;
   //qDebug()<<"Frame Number is " << globalCountOut;
   // Range Profile
   unsigned int numRangeBinProcessed = demoParams.rangeBinEnd_index - demoParams.rangeBinStart_index + 1;
   QVector<double> RangeProfile(2*numRangeBinProcessed);
   QVector<double> xRangePlot(numRangeBinProcessed), yRangePlot(numRangeBinProcessed);
   unsigned int indexRange = INDEX_IN_DATA_RANGE_PROFILE_START;/*168;*/

   for (unsigned int index = 0; index < 2*numRangeBinProcessed; index ++)
   {
   qint16  tempRange_int  = parseValueInt16(data, indexRange, 4);  // Range Bin Index
   RangeProfile[index] = tempRange_int;
   indexRange = indexRange + 4;
   }

   //Range Plot
   for (unsigned int indexRangeBin = 0; indexRangeBin < numRangeBinProcessed; indexRangeBin ++)
   {
      yRangePlot[indexRangeBin] = sqrt(RangeProfile[2*indexRangeBin]*RangeProfile[2*indexRangeBin]+ RangeProfile[2*indexRangeBin + 1]*RangeProfile[2*indexRangeBin + 1]);
      /*if(yRangePlot[indexRangeBin] < 750){
          yRangePlot[indexRangeBin] = 0;
      }*/
      xRangePlot[indexRangeBin] = demoParams.rangeStartMeters + demoParams.rangeBinSize_meters*indexRangeBin;
   }
   double maxRCS = *std::max_element(yRangePlot.constBegin(), yRangePlot.constEnd());
   maxRCS_updated = ALPHA_RCS*(maxRCS) + (1-ALPHA_RCS)*maxRCS_updated;
   // make Decision
   float BreathingRate_Out,heartRate_Out;
   float diffEst_heartRate, heartRateEstDisplay;

   float heartRate_OutMedian;
   static  QVector<float> heartRateBuffer;
   heartRateBuffer.resize(HEART_RATE_EST_MEDIAN_FLT_SIZE);


   static QVector<float> heartRateOutBufferFinal;
   heartRateOutBufferFinal.resize(HEART_RATE_EST_FINAL_OUT_SIZE);

//heartRate_2hz_CM 0.054199   11.6929
//heartRate_xCorr_CM 0.36195   37.0258
   // Heart-Rate Display Decision
   //float CM_combined;
   float outHeartPrev_CM = outHeartNew_CM;
   outHeartNew_CM = ALPHA_HEARTRATE_CM*(heartRate_2hz_CM) + (1-ALPHA_HEARTRATE_CM)*outHeartPrev_CM;

   diffEst_heartRate = abs(heartRate_FFT - heartRate_Pk);

   /*if(BreathingRatePK_Out*7 < heartRate_FFT)
       heartRate_FFT/=2;

   if(BreathingRatePK_Out*7 < heartRate_xCorr)
       heartRate_xCorr/=2;*/
   int tosave;
   //heartRate_2hz_CM = (heartRate_2hz_CM-0.05)*8;
   //heartRate_xCorr_CM = (heartRate_xCorr_CM-0.36)*0.27;
   if(ui->checkBox_xCorr->isChecked() && ui->checkBox_FFT->isChecked())
   {
       heartRateEstDisplay = heartRate_Pk;
       //CM_combined = 1;
   }
   else if (ui->checkBox_FFT->isChecked())
   {
       heartRateEstDisplay = heartRate_FFT;
       //CM_combined = heartRate_2hz_CM;
   }
   else if (ui->checkBox_xCorr->isChecked())
   {
       heartRateEstDisplay = heartRate_xCorr;
       //CM_combined = heartRate_xCorr_CM;
   }
   else //if ( (outHeartNew_CM > THRESH_HEART_CM) || (diffEst_heartRate < THRESH_DIFF_EST) )
   {
       if(abs(heartRate_2hz_CM-79.6)< 0.1 ){
           heartRateEstDisplay = heartRate_FFT;
          // CM_combined = heartRate_2hz_CM;
       }else if(heartRate_FFT > 115 && (heartRate_FFT > BreathingRate_Out*7.3 || abs(heartRate_FFT-xk) > abs(heartRate_FFT/2-xk)))//donc matnajem tkoun ken el harmonic el thenya //if(abs(heartRate_FFT-xk) < abs(heartRate_FFT/2-xk))
       {
           heartRateEstDisplay = heartRate_FFT/2;
          // CM_combined = heartRate_2hz_CM;
       }
       else if(abs(heartRate_FFT-heartRate_xCorr) < 3+(heartRate_FFT_4Hz-50)/20)
       {
           heartRateEstDisplay = heartRate_FFT;
          // CM_combined = heartRate_2hz_CM;
       }
       else if(abs(heartRate_FFT_4Hz-heartRate_FFT) < 3+(heartRate_FFT_4Hz-50)/20)
       {
           heartRateEstDisplay = heartRate_FFT;
          // CM_combined = heartRate_2hz_CM;
       }
       else if(abs(heartRate_FFT_4Hz-heartRate_xCorr) < 4+(heartRate_FFT_4Hz-50)/20)
       {
           heartRateEstDisplay = heartRate_xCorr;
          // CM_combined = heartRate_xCorr_CM;
       }
      /* else if(abs(heartRate_Pk-heartRate_xCorr) < 4)
       *  myfile<<  heartRateEstDisplay<<';'<<outSumEnergyHeartWfm<<';'<<
       *            heartRate_FFT<<';'<<heartRate_FFT_4Hz<<';'<<heartRate_xCorr<<
       *        ';'<<BreathingRate_Out<<';'<<xk;

       {
           heartRateEstDisplay = heartRate_xCorr;
           //CM_combined = heartRate_xCorr_CM/2;
       }*/
       else //if(abs(heartRate_FFT-heartRate_xCorr) < 5 || abs(heartRate_FFT-heartRate_Pk) < 7)
       {
           heartRateEstDisplay = heartRate_FFT;
           //CM_combined = heartRate_2hz_CM;
       }
   }

   //if (heartRateEstDisplay<48)heartRateEstDisplay = xk;

   /*if (ui->radioButton_BackMeasurements->isChecked())
   {

      #ifdef HEAURITICS_APPROACH1

       if ( abs(heartRate_xCorr-heartRate_FFT) < THRESH_BACK)
       {
           heartRateEstDisplay = heartRate_FFT;
       }
       else
       {
           heartRateEstDisplay = heartRate_xCorr;
       }

       heartRateBuffer.insert(2*(localCount % HEART_RATE_EST_MEDIAN_FLT_SIZE/2), heartRateEstDisplay);


      if (ui->checkBox_FFT)
       {
           heartRateBuffer.insert(2*(localCount % HEART_RATE_EST_MEDIAN_FLT_SIZE/2)+1, heartRateEstDisplay);
       }
       else
       {
           heartRateBuffer.insert(2*(localCount % HEART_RATE_EST_MEDIAN_FLT_SIZE/2)+1, heartRate_FFT_4Hz);
       }
      #endif

      int IsvalueSelected = 0;

      if ( abs(heartRate_xCorr - 2*BreathingRate_FFT) > BACK_THRESH_BPM)
      {
          heartRateBuffer.insert(currIndex % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRate_xCorr);
          IsvalueSelected = 1;
          currIndex++;
      }
      if ( heartRate_2hz_CM > BACK_THRESH_CM )
      {
          heartRateBuffer.insert(currIndex % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRate_FFT);
          IsvalueSelected = 1;
          currIndex++;
      }
      if (heartRate_4Hz_CM > BACK_THRESH_4Hz_CM)
      {
          heartRateBuffer.insert(currIndex % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRate_FFT_4Hz);
          IsvalueSelected = 1;
          currIndex++;
      }

      if (IsvalueSelected == 0)   // If nothing is selected from the current frame then select the Inter-peak distance Estimate
      {
          heartRateBuffer.insert(currIndex % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRate_Pk);
          currIndex++;
      }
   }
   else
   {*/
     //qDebug()<<heartRate_4Hz_CM<<" "<< BACK_THRESH_4Hz_CM;
     //heartRateBuffer.insert(localCount % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRate_FFT_4Hz);
     heartRateBuffer.insert(localCount % HEART_RATE_EST_MEDIAN_FLT_SIZE, heartRateEstDisplay);
   //}

   if (gui_paused != current_gui_status)
   {
  // if(CM_combined != -1){
   QList<float> heartRateBufferSort = QList<float>::fromVector(heartRateBuffer);
   qSort(heartRateBufferSort.begin(), heartRateBufferSort.end());
   heartRate_OutMedian = heartRateBufferSort.at(HEART_RATE_EST_MEDIAN_FLT_SIZE/2);
   //}
   float KF_Gain,CM_combined,CM2,diff=abs(xk-heartRate_OutMedian);//heartRateEstDisplay);//heartRate_OutMedian);
if (APPLY_KALMAN_FILTER)
{
   if (TheFirstFrame==1)
   {
       xk=70;//heartRate_OutMedian
       TheFirstFrame= 0;//heartRate_OutMedian;
   }
   float R;               // Measurement Variance
   float Q;               // State Variance
   if(globalCountOut<500){
       CM2=0;
   }
   else if(diff>=6){
       CM2 = -0.0532*diff+5.32;
   }else{
       CM2 = (diff/1.5-6.3)*(diff/1.5-6.3);
   }
   //printf("--%f--",(abs(xk-heartRate_OutMedian)/1.5-6.3)*(abs(xk-heartRate_OutMedian)/1.5-6.3));
   CM_combined = heartRate_2hz_CM + heartRate_4Hz_CM + 10*heartRate_xCorr_CM+CM2;
   CM_combined/=2;
   R = 1/(CM_combined + 0.0001);//9ad ma el CM increase 9ad ya3ti thi9a fel valeur eli jet//el 0.0001 bich matjich fel negative
   Q = 1e-6;//l3aks //expected result eli bich tather fel thi9a
   KF_Gain  = Pk/(Pk + R);
   xk = xk + KF_Gain*(heartRate_OutMedian - xk);//=(1-KF_gain)*xk + KF_gain*heartRate_OutMedian;heartRateEstDisplay
   //qDebug()<<CM_combined<<' '<<heartRate_OutMedian<<" displayed :"<<xk;
   Pk = (1-KF_Gain)*Pk + Q;// ki tji serie mta3 valeuret 3ando      el thi9a fihom ywali 3ado       thi9a fel valeuret el jeyin akther
   heartRate_Out = xk;     // ki tji serie mta3 valeuret ma3andouch el thi9a fihom ywali ma3andouch thi9a fel valeuret el jeyin akther
}
else{
   heartRate_Out = heartRate_OutMedian;
}
   // Further Filtering of the Final Heart-rate Estimates //ken el bufferSTD taya7 ma3neha ?

   heartRateOutBufferFinal.insert(localCount % (HEART_RATE_EST_FINAL_OUT_SIZE), heartRate_Out);
   const auto mean = std::accumulate(heartRateOutBufferFinal.begin(), heartRateOutBufferFinal.end(), .0) / heartRateOutBufferFinal.size();
   double sumMAD;
   double bufferSTD;
   sumMAD = 0;
   for (int indexTemp=0;indexTemp<heartRateOutBufferFinal.size();indexTemp++)
   {
      sumMAD += abs(heartRateOutBufferFinal.at(indexTemp) - mean);
   }
   bufferSTD = sqrt(sumMAD)/heartRateOutBufferFinal.size();
   ui->lcdNumber_ReliabilityMetric->display(rangeBinIndexOut);//bufferSTD);//STANDARD DEVIATION ama ne9sa el carré

   // Breathing-Rate decision
   float outSumEnergyBreathWfm_thresh;
   float RCS_thresh;
   outSumEnergyBreathWfm_thresh = ui->SpinBox_TH_Breath->value();
   RCS_thresh = ui->SpinBox_RCS->value();
   outSumEnergyBreathWfm=outSumEnergyBreathWfm/1000000;
   bool flag_Breathing;

   /*qDebug()<<"HRD added = "<<BreathingRate_xCorr
          <<"/nHRD removed = "<<outMotionDetectionFlag
         <<"/nadded pos = "<<outSumEnergyHeartWfm
        <<"/nremoved pos = "<<BreathingRate_FFT
       <<"/nHRM = "<<heartRate_4Hz_CM;//heartRate_4Hz_CM<<";"<<heartRate_Out;*/
   /* qDebug()<<heartRateEstDisplay;
   qDebug()<<"[0] "<<outSumEnergyHeartWfm
          <<"\n[1] "<<heartRate_4Hz_CM
         <<"\n[2] "<<BreathingRate_FFT
        <<"\n[3] "<<outMotionDetectionFlag
       <<"\n[4] "<<BreathingRate_xCorr;*/
  // qDebug()<<heartRate_4Hz_CM<<' '<<heartRate_OutMedian;
   //qDebug()<<heartWfm_Out<<' '<<rangeBinIndexOut;
   /*if(abs(heartRate_4Hz_CM-heartRate_OutMedian)>0.05)
   {
       qDebug()<<-1;
   }*/
   /*if(abs(BreathingRate_xCorr-heartRateEstDisplay)>0.05)// || abs(heartRate_4Hz_CM-heartRate_OutMedian)>0.05)
   {    qDebug()<<BreathingRate_xCorr<<' '<<heartRateEstDisplay;
       exit(-1);
   }*/
   if (((outSumEnergyBreathWfm < outSumEnergyBreathWfm_thresh) || (maxRCS_updated < RCS_thresh) || (BreathingRate_xCorr_CM <= 0.002))&& globalCountOut > 600)
   {
       flag_Breathing = 0;
       //BreathingRate_Out = 0;
       QPalette lcdpaletteNotBreathing = ui->lcdNumber_Breathingrate->palette();
       lcdpaletteNotBreathing.setColor(QPalette::Normal, QPalette::Window, Qt::red);
       ui->lcdNumber_Breathingrate->setPalette(lcdpaletteNotBreathing);
   }
   else
   {
       flag_Breathing = 1;
       lcdpaletteBreathing.setColor(QPalette::Normal, QPalette::Window, Qt::white);
       ui->lcdNumber_Breathingrate->setPalette(lcdpaletteBreathing);


       /*if (breathRate_CM > THRESH_BREATH_CM )
       {
          //BreathingRate_Out = BreathingRate_FFT;
           BreathingRate_Out = BreathingRatePK_Out;
       }
           else
       {
          BreathingRate_Out = BreathingRatePK_Out;
       }*/
       xb = 0.01*BreathingRatePK_Out+(1-0.01)*xb;
       BreathingRate_Out = xb;
  }

   // Heart-Rate Decision
   float outSumEnergyHeartWfm_thresh;
   outSumEnergyHeartWfm_thresh = ui->SpinBox_TH_Heart->value();//0.05
   if(outSumEnergyHeartWfm < 0)outSumEnergyHeartWfm*=-1;
   if ((outSumEnergyHeartWfm < outSumEnergyHeartWfm_thresh || maxRCS_updated < RCS_thresh) && globalCountOut > 600)
   {
       //heartRate_Out = 0;
       QPalette lcdpaletteNoHeartRate = ui->lcdNumber_HeartRate->palette();
       lcdpaletteNoHeartRate.setColor(QPalette::Normal, QPalette::Window, Qt::red);
       //heartWfm_Out=0;
       ui->lcdNumber_HeartRate->setPalette(lcdpaletteNoHeartRate);
   }
   else
   {
       QPalette lcdpaletteHeartRate = ui->lcdNumber_HeartRate->palette();
       lcdpaletteHeartRate.setColor(QPalette::Normal, QPalette::Window, Qt::white);
       ui->lcdNumber_HeartRate->setPalette(lcdpaletteHeartRate);
   }

   //myfile<<phaseWfm_Out<<','<<breathWfm_Out<<','<<heartWfm_Out<<','<<(int)BreathingRate_Out<<','<<(int)heartRate_Out<<','<<heartRate_Pk<<','<<heartRate_xCorr<<','<<heartRate_FFT<<','<<heartRate_FFT_4Hz<<','<<heartRate_xCorr_CM<<','<<heartRate_2hz_CM<<','<<heartRate_4Hz_CM<<','<<rangeBinIndexOut;
 if (ui->checkBox_displayPlots->isChecked())
 {
     if(reserved9 > 0){
         for(int i=0;i<NUM_PTS_RRSIGNAL_PLOT-1;++i){
             jjsignalBuffer[i]=jjsignalBuffer[i+1];
         }
         jjsignalBuffer[NUM_PTS_RRSIGNAL_PLOT-1] = BreathingRate_FFT*50;
     }
     /*int ind = breathingBufferSize-heartRate_4Hz_CM;
     //heart peak
     ind = heartBufferSize-BreathingRate_FFT;
     ind = indexTemp-ind;
     int period;
     float currPeriod=1000*60/heartRate_Out;
     if(ind!= lastindh & ind>0 & ind+1<NUM_PTS_DISTANCE_TIME_PLOT){
         heartWfmBuffer[ind]=heartWfmBuffer[ind]+0.2;
         if(ind>lastindh){
             period = ind-lastindh;
         }else
             period = 250-lastindh+ind;//+-1
         if(currPeriod-151 < period*50 && period*50 < currPeriod+151){
         for(int i=0;i<NUM_PTS_RRSIGNAL_PLOT-1;++i){
             jjsignalBuffer[i]=jjsignalBuffer[i+1];
         }
         jjsignalBuffer[NUM_PTS_RRSIGNAL_PLOT-1] = period*50;
         static int summ=0;
         static int lastperiod =0;
         if(lastperiod!=period){
         summ+=period*50;
         //myfile<<period*50<<','<<summ<<'\n';
         lastperiod = period;
         }
         }
     }
     lastindh=ind;*/
   if (indexTemp ==0)
   {
      for (unsigned int i=0;i<NUM_PTS_DISTANCE_TIME_PLOT;i++)
      {
        xDistTimePlot[i]=indexTemp;
        yDistTimePlot[i]=phaseWfm_Out;
        heartWfmBuffer[i] = heartWfm_Out;
        breathingWfmBuffer[i] = breathWfm_Out;
      }
   }

  double max = *std::max_element(breathingWfmBuffer.constBegin(), breathingWfmBuffer.constEnd());
  double min = *std::min_element(breathingWfmBuffer.constBegin(), breathingWfmBuffer.constEnd());

  double breathingWfm_display_max, breathingWfm_display_min;

  if (max < BREATHING_PLOT_MAX_YAXIS)
      breathingWfm_display_max = BREATHING_PLOT_MAX_YAXIS;
  else
      breathingWfm_display_max = max;

  if (min > -BREATHING_PLOT_MAX_YAXIS)
      breathingWfm_display_min = -BREATHING_PLOT_MAX_YAXIS;
  else
      breathingWfm_display_min = min;

   xDistTimePlot[indexTemp] = indexTemp;
   yDistTimePlot[indexTemp] = phaseWfm_Out;
   heartWfmBuffer[indexTemp] = -heartWfm_Out;//heartWfm_Out;
   breathingWfmBuffer[indexTemp] = breathWfm_Out;

   ui->phaseWfmPlot->yAxis->setRange(-10, 10);
   ui->phaseWfmPlot->graph(0)->setData(xDistTimePlot,yDistTimePlot);
   ui->phaseWfmPlot->yAxis->rescale();
   ui->phaseWfmPlot->replot();

   ui->BreathingWfmPlot->graph(0)->setData(xRRsignal,jjsignalBuffer);
   ui->BreathingWfmPlot->yAxis->setRangeLower(400);
   ui->BreathingWfmPlot->yAxis->setRangeUpper(1600);
   ui->BreathingWfmPlot->xAxis->setRange(0,60);
   ui->BreathingWfmPlot->replot();

   if(breathOrheart){
       ui->heartWfmPlot->graph(0)->setData(xDistTimePlot,heartWfmBuffer);
       ui->heartWfmPlot->yAxis->rescale();//zitha
   }else{
       ui->heartWfmPlot->graph(0)->setData(xDistTimePlot,breathingWfmBuffer);
       ui->heartWfmPlot->yAxis->setRangeLower(breathingWfm_display_min);
       ui->heartWfmPlot->yAxis->setRangeUpper(breathingWfm_display_max);
   }
   ui->heartWfmPlot->replot();

   ui->plot_RangeProfile->graph(0)->setData(xRangePlot,yRangePlot);
   ui->plot_RangeProfile->xAxis->setRange(demoParams.rangeStartMeters,demoParams.rangeEndMeters);

//   ui->plot_RangeProfile->yAxis->rescale();
   if (maxRCS < (ui->SpinBox_RCS->value()))
   {
       ui->plot_RangeProfile->yAxis->setRangeUpper(ui->SpinBox_RCS->value());
   }
   else
   {
       ui->plot_RangeProfile->yAxis->setRangeUpper(maxRCS);
   }

   ui->plot_RangeProfile->replot();
 }
   // Update Numerical Displays
   ui->lcdNumber_FrameCount->display((int) globalCountOut);

   QString myString_BreathRate;
   ui->lcdNumber_Breathingrate->setDigitCount(8);
   myString_BreathRate.sprintf("%1.0f",BreathingRate_Out);

   QString myString_HeartRate;
   ui->lcdNumber_HeartRate->setDigitCount(3);
   myString_HeartRate.sprintf("%1.0f",heartRate_Out);//reserved7//heartRate_Out
   if(globalCountOut<600){
       myString_HeartRate.sprintf("%s","--");
       myString_BreathRate.sprintf("%s","--");
   }
   ui->lcdNumber_HeartRate->display(myString_HeartRate);
   ui->lcdNumber_Breathingrate->display(myString_BreathRate);

   QString myString_RangeBinIndex;
   ui->lcdNumber_Index->setDigitCount(8);
   myString_RangeBinIndex.sprintf("%d",rangeBinIndexOut);
   ui->lcdNumber_Index->display(myString_RangeBinIndex);

   QString myString_BreathingRatePK_Out;
   ui->lcdNumber_Breath_pk->setDigitCount(8);
   myString_BreathingRatePK_Out.sprintf("%1.0f",BreathingRatePK_Out);
   ui->lcdNumber_Breath_pk->display(myString_BreathingRatePK_Out);

   QString myString_heartRate_Pk;
   ui->lcdNumber_Heart_pk->setDigitCount(8);
   myString_heartRate_Pk.sprintf("%1.0f",heartRate_Pk);
   ui->lcdNumber_Heart_pk->display(myString_heartRate_Pk);

   QString myString_BreathingRate_FFT;
   ui->lcdNumber_Breath_FT->setDigitCount(8);
   myString_BreathingRate_FFT.sprintf("%1.0f",BreathingRate_FFT);
   ui->lcdNumber_Breath_FT->display(myString_BreathingRate_FFT);

   QString myString_HeartRate_FFT;
   ui->lcdNumber_Heart_FT->setDigitCount(8);
   myString_HeartRate_FFT.sprintf("%1.0f",heartRate_FFT);
   ui->lcdNumber_Heart_FT->display(myString_HeartRate_FFT);

   QString myString_breathRate_CM;
   ui->lcdNumber_CM_Breath->setDigitCount(8);
   myString_breathRate_CM.sprintf("%1.3f",breathRate_CM);
   ui->lcdNumber_CM_Breath->display(myString_breathRate_CM);

   QString myString_heartRate_CM;
   ui->lcdNumber_CM_Heart->setDigitCount(8);
   myString_heartRate_CM.sprintf("%1.3f",heartRate_2hz_CM);
   ui->lcdNumber_CM_Heart->display(myString_heartRate_CM);

   QString myString_heartRate_4Hz_CM;
   ui->lcdNumber_Display4->setDigitCount(8);
   myString_heartRate_4Hz_CM.sprintf("%1.3f",heartRate_4Hz_CM);
   ui->lcdNumber_Display4->display(myString_heartRate_4Hz_CM);

   QString myString_Breathing_WfmEnergy;
   ui->lcdNumber_BreathEnergy->setDigitCount(8);
   myString_Breathing_WfmEnergy.sprintf("%1.3f",outSumEnergyBreathWfm);
   ui->lcdNumber_BreathEnergy->display(myString_Breathing_WfmEnergy);

   QString myString_Heart_WfmEnergy;
   ui->lcdNumber_HeartEnergy->setDigitCount(8);
   myString_Heart_WfmEnergy.sprintf("%1.3f",outSumEnergyHeartWfm);
   ui->lcdNumber_HeartEnergy->display(myString_Heart_WfmEnergy);

   QString myString_RCS;
   ui->lcdNumber_RCS->setDigitCount(8);
   myString_RCS.sprintf("%1.0f",   maxRCS_updated);
   ui->lcdNumber_RCS->display(myString_RCS);

   QString myString_xCorr;
   ui->lcdNumber_Heart_xCorr->setDigitCount(8);
   myString_xCorr.sprintf("%1.0f", heartRate_xCorr);
   ui->lcdNumber_Heart_xCorr->display(myString_xCorr);

   QString myString_FFT_4Hz;
   ui->lcdNumber_Heart_FT_4Hz->setDigitCount(8);
   myString_FFT_4Hz.sprintf("%1.0f",   heartRate_4Hz_CM);
   ui->lcdNumber_Heart_FT_4Hz->display(myString_FFT_4Hz);

   QString myString_Reserved_1;
   ui->lcdNumber_Display3->setDigitCount(8);
   myString_Reserved_1.sprintf("%1.3f", outMotionDetectionFlag);
   ui->lcdNumber_Display3->display(myString_Reserved_1);
   if (BreathingRate_FFT)//outMotionDetectionFlag)
   {
       ui->lcdNumber_Display3->setAutoFillBackground(true);
       ui->lcdNumber_Display3->setPalette(Qt::red);
   }
   else
   {
       ui->lcdNumber_Display3->setPalette(Qt::white);
   }

   QString myString_heartRate_FFT_4Hz;
   ui->lcdNumber_Heart_FT_4Hz->setDigitCount(8);
   myString_heartRate_FFT_4Hz.sprintf("%1.3f", heartRate_FFT_4Hz);
   ui->lcdNumber_Heart_FT_4Hz->display(myString_heartRate_FFT_4Hz);

   QString myString_CM_heart_xCorr;
   ui->lcdNumber_CM_Heart_xCorr->setDigitCount(8);
   myString_CM_heart_xCorr.sprintf("%1.3f", heartRate_xCorr_CM);
   ui->lcdNumber_CM_Heart_xCorr->display(myString_CM_heart_xCorr);

   QString myString_CM_breath_xCorr;
   ui->lcdNumber_CM_Breath_xCorr->setDigitCount(8);
   myString_CM_breath_xCorr.sprintf("%1.3f", BreathingRate_xCorr_CM);
   ui->lcdNumber_CM_Breath_xCorr->display(myString_CM_breath_xCorr);

   QString myString_breathRate_harmEnergy;
   ui->lcdNumber_breathRate_HarmEnergy->setDigitCount(8);
   myString_breathRate_harmEnergy.sprintf("%1.3f", BreathingRate_HarmEnergy);
   ui->lcdNumber_breathRate_HarmEnergy->display(myString_breathRate_harmEnergy);

   QString myString_breathRate_xCorr;
   ui->lcdNumber_Breath_xCorr->setDigitCount(8);
   myString_breathRate_xCorr.sprintf("%1.3f", BreathingRate_xCorr);
   ui->lcdNumber_Breath_xCorr->display(myString_breathRate_xCorr);

   }
  //qDebug() << "The Process Function took" << timer.elapsed() << "milliseconds";
   //int recent_time=timer.elapsed();
   //myfile<<recent_time-previous_time<<" milliseconds\n";
   //previous_time=recent_time;
  } // If GUI-Paused end
  myfile<<"\n";
  qDebug()<<'\n';
  localCount = localCount + 1;
 }  //Magic-OK if ends
}

void MainWindow::gui_statusUpdate()
{
    if (current_gui_status == gui_running)
    {
            ui->pushButton_start->setStyleSheet("background-color: red");
            ui->pushButton_stop->setStyleSheet("background-color: none");
            ui->pushButton_pause->setStyleSheet("background-color: none");

            statusBar()->showMessage(tr("Sensor Running"));
    }

    if (current_gui_status == gui_stopped)
    {
            ui->pushButton_stop->setStyleSheet("background-color: red");
            ui->pushButton_start->setStyleSheet("background-color: none");
            ui->pushButton_pause->setStyleSheet("background-color: none");
            statusBar()->showMessage(tr("Sensor Stopped"));
            qDebug()<<"Sensor is Stopped";
            statusBar()->showMessage(tr("Sensor Stopped"));

    }
    if (current_gui_status == gui_paused)
    {
            ui->pushButton_stop->setStyleSheet("background-color: none");
            ui->pushButton_start->setStyleSheet("background-color: none");
            ui->pushButton_pause->setStyleSheet("background-color: red");
            statusBar()->showMessage(tr("GUI is Paused Stopped"));
    }
}

void MainWindow::on_pushButton_pause_clicked()
{
    localCount = 0;
    for (unsigned int i=0;i<NUM_PTS_DISTANCE_TIME_PLOT;i++)
    {
      xDistTimePlot[i]=0;
      //xRRsignal[i]=0;
      yDistTimePlot[i]=0;
      heartWfmBuffer[i] = 0;
      breathingWfmBuffer[i] = 0;
     }
    current_gui_status = gui_paused;
    emit gui_statusChanged();
}









