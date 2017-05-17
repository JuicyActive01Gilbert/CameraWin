#ifndef CAMAREWIN_H
#define CAMAREWIN_H

#include "ui_camerawin.h"
#include <QWidget>
#include <QSize>

#define SAVE_PATH "./��Ƭ/" //����·��
#define TEMP_DIR "./temp/" //����·��
#define TEMP_FILE_NAME "temp.png" //��������
#define TIMER_CYCLE 24 //��ʱ������

class QImage;
class QCamera;
class QCameraViewfinderSettings;
class QCameraImageCapture;
class QTimer;

//��Ļ����
typedef enum ScreenDirationEnum
{
	HOR_SCRREN = 0,
	VER_SCRREN = 1
} SCREEN_DIR;

//����ͷ����
typedef struct CameraParameterStruct
{
	int nMinFrame;//��С֡��
	int nMaxFrame;//���֡��
	QSize sizeResolution;//�ֱ���

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

	void setScreenDir(SCREEN_DIR dir = HOR_SCRREN);//������Ļ����,Ĭ��Ϊ����
    void startCamera();//��������ͷ
	void stopCamera();//ֹͣ����ͷ
	void takePhoto();//����
	void resTakePhoto();//����
	void clearImagePath();//���·��
	void removeImage();//���ͼƬ
	QString getImagePath();//��ȡͼƬ·��
	void setCameraSettings(CAMERA_PARAM settings);//��������ͷ����
	QList<CAMERA_PARAM> getCameraSupportSettings();//��ȡ����ͷ���в���
signals:
	void signalCameraOpen(bool);//����ͷ���ź�
protected:
	void initialize();//��ʼ��
	QImage cutImage(QImage src);//�ü�ͼƬ���Ӻ����س�����
protected slots:
	void slotDisplayImage(int,QImage);//��ʾÿ��ͼƬ
	void slotTimeOut();//��ʱ������
private:
	QCamera* m_pCamera;
	QCameraImageCapture* m_pCapture;
	QTimer* m_pTimer;//��ʱ��
	SCREEN_DIR m_dirScreen;//��Ļ����
	QString m_strPath;//ͼƬ·��
	QImage m_image;//ͼƬ

};

#endif // CAMAREWIN_H
