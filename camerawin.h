#ifndef CAMAREWIN_H
#define CAMAREWIN_H

#include "ui_camerawin.h"
#include <QWidget>
#include <QSize>

#define SAVE_PATH "./照片/" //保存路径
#define TEMP_DIR "./temp/" //缓存路径
#define TEMP_FILE_NAME "temp" //缓存名称
#define TIMER_CYCLE 24 //定时器周期

class QImage;
class QCamera;
class QCameraViewfinderSettings;
class QCameraImageCapture;
class QTimer;

//屏幕方向
typedef enum ScreenDirationEnum
{
	HOR_SCRREN = 0,
	VER_SCRREN = 1
} SCREEN_DIR;

//摄像头参数
typedef struct CameraParameterStruct
{
	int nMinFrame;//最小帧率
	int nMaxFrame;//最大帧率
	QSize sizeResolution;//分辨率

	CameraParameterStruct()
	{
		clear();
	}

	void clear()
	{
		nMinFrame = 5;
		nMaxFrame = 5;
		sizeResolution = QSize(640,480);
	}
	
} CAMERA_PARAM;


class CamareWin : public QWidget,public Ui_CameraWin
{
    Q_OBJECT

public:
    explicit CamareWin(QWidget *parent = 0);
    ~CamareWin();

	void setScreenDir(SCREEN_DIR dir = HOR_SCRREN);//设置屏幕方向,默认为横屏
    void startCamera();//开启摄像头
	void stopCamera();//停止摄像头
	void takePhoto();//照相
	void resTakePhoto();//重照
	void clearImagePath();//清除路径
	void removeImage();//清除图片
	QString getImagePath();//获取图片路径
	void setCameraSettings(CAMERA_PARAM settings);//设置摄像头参数
	QList<CAMERA_PARAM> getCameraSupportSettings();//获取摄像头所有参数
signals:
	void signalCameraOpen(bool);//摄像头打开信号
protected:
	void initialize();//初始化
	QImage cutImage(QImage src);//裁剪图片，从横屏截成竖屏
protected slots:
	void slotDisplayImage(int,QImage);//显示每张图片
	void slotTimeOut();//计时器结束
private:
	QCamera* m_pCamera;
	QCameraImageCapture* m_pCapture;
	QTimer* m_pTimer;//定时器
	SCREEN_DIR m_dirScreen;//屏幕方向
	QString m_strPath;//图片路径
	QImage m_image;//图片

};

#endif // CAMAREWIN_H
