#include "camerawin.h"
#include <QCamera>
#include <QCameraViewfinderSettings>
#include <QCameraImageCapture>
#include <QImage>
#include <QTimer>
#include <QDir>
#include <QDateTime>

CamareWin::CamareWin(QWidget *parent) :
    QWidget(parent)
{
	m_pCamera = NULL;
	m_pCapture = NULL;
	m_pTimer = NULL;
	m_dirScreen = HOR_SCRREN;
    setupUi(this);

	initialize();
}

CamareWin::~CamareWin()
{

}

void CamareWin::setScreenDir(SCREEN_DIR dir /* = HOR_SCRREN */)
{
	m_dirScreen = dir;
}

void CamareWin::startCamera()
{
	if (!m_pTimer->isActive())
	{
		m_pTimer->start(TIMER_CYCLE);
	}
	if (!m_pCamera->isAvailable())
	{
		m_pCamera->start();
	}
}

void CamareWin::stopCamera()
{
	if (m_pTimer->isActive() )
	{
		m_pTimer->stop();
	}

	if (m_pCamera->isAvailable())
	{
		m_pCamera->stop();
	}

}

void CamareWin::takePhoto()
{
	stopCamera();
	QString path = QDateTime::currentDateTime().toString("Pic_yyyy_MM_dd_hh_mm_ss") + ".png";
	m_strPath = SAVE_PATH + path;
	m_image.save(m_strPath);
}

void CamareWin::resTakePhoto()
{
	startCamera();
	clearImagePath();
	removeImage();
}

void CamareWin::clearImagePath()
{
	m_strPath.clear();
}

void CamareWin::removeImage()
{
	if (m_strPath.isEmpty())
	{
		return;
	}
	QDir dir;
	if (!dir.exists(m_strPath))
	{
		return;
	}

	dir.remove(m_strPath);
}

QString CamareWin::getImagePath()
{
	return m_strPath;
}

void CamareWin::setCameraSettings(CAMERA_PARAM settings)
{
	if (m_pCamera == NULL)
	{
		return;
	}

	QCameraViewfinderSettings set;
	set.setMinimumFrameRate(settings.nMinFrame);
	set.setMaximumFrameRate(settings.nMaxFrame);
	set.setResolution(settings.sizeResolution);
	m_pCamera->setViewfinderSettings(set);
}

QList<CAMERA_PARAM> CamareWin::getCameraSupportSettings()
{
	QList<CAMERA_PARAM> lstSettings;

	lstSettings.clear();
	if (m_pCamera == NULL)
	{
		return lstSettings;
	}

	QList<QCameraViewfinderSettings> lstSize = m_pCamera->supportedViewfinderSettings();
	for (int i = 0; i < lstSize.count(); i++)
	{
		CAMERA_PARAM setting;
		setting.nMaxFrame = lstSize.at(i).maximumFrameRate();
		setting.nMinFrame = lstSize.at(i).minimumFrameRate();
		setting.sizeResolution = lstSize.at(i).resolution();
		lstSettings.append(setting);
	}
	return lstSettings;
}

void CamareWin::initialize()
{
	m_pCamera = new QCamera(this);
	m_pCapture = new QCameraImageCapture(m_pCamera);
	connect(m_pCapture, SIGNAL(imageCaptured(int, QImage)), this, SLOT(slotDisplayImage(int, QImage)));
	m_pCamera->setCaptureMode(QCamera::CaptureStillImage);
	m_pCamera->start();

	//定时器
	m_pTimer = new QTimer(this);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
	if (!m_pCamera->isAvailable())
	{
		emit signalCameraOpen(false);
		return;
	}
	//判断缓存目录是否存在，不存在则创建
	QDir dir;
	if (!dir.exists(TEMP_DIR))
	{
		dir.mkpath(TEMP_DIR);
	}
	if (!dir.exists(QStringLiteral(SAVE_PATH)))
	{
		dir.mkpath(QStringLiteral(SAVE_PATH));
	}

	emit signalCameraOpen(true);
}

QImage CamareWin::cutImage(QImage src)
{
	QImage resImage;
	int nOldW = 0;
	int nOldH = 0;
	int nScaledW = 0;//缩放的宽度
	int nScaledH = 0;//缩放的高度
	int nWinW = 0;
	int nWinH = 0;
	int x = 0;
	int y = 0;

	nWinW = this->width();
	nWinH = this->height();
	nOldW = src.width();
	nOldH = src.height();

	switch (m_dirScreen)
	{
		//竖屏
	case VER_SCRREN:
	{
		if (nOldW/nOldH*1.0 > nWinW/nWinH*1.0)
		{
			nScaledH = nWinH;
			nScaledW = nWinH / nOldH * nOldW;//得到能覆盖整个窗口的照片的大小
			x = (nScaledW - nWinW) / 2;
		}
		else
		{
			nScaledW = nWinW;
			nScaledH = nWinW / nOldW * nOldH;//得到能覆盖整个窗口的照片的大小
			y = (nScaledH - nWinH) / 2;
		}
	}
		break;
		//横屏
	case HOR_SCRREN:
	{
		if (nOldH / nOldW*1.0 > nWinH / nWinW*1.0)
		{
			nScaledH = nWinH;
			nScaledW = nWinH / nOldH * nOldW;//得到能覆盖整个窗口的照片的大小
			x = (nScaledW - nWinW) / 2;
		}
		else
		{
			nScaledW = nWinW;
			nScaledH = nWinW / nOldW * nOldH;//得到能覆盖整个窗口的照片的大小
			y = (nScaledH - nWinH) / 2;
		}
	}
		break;
	default:
		break;
	}
	src = src.scaled(nScaledW, nScaledH);
	resImage = src.copy(x, y, nWinW, nWinH);
	return resImage;
}

void CamareWin::slotDisplayImage(int, QImage img)
{
	img = img.mirrored(true, false);
	img = cutImage(img);//img.scaled(m_pLabImage->size());//, Qt::KeepAspectRatio);
	m_image = img;
	m_pLabImage->setPixmap(QPixmap::fromImage(img));
}

void CamareWin::slotTimeOut()
{
	QDir temDir(TEMP_DIR);//path中不能包含中文
	QString strDir = temDir.absolutePath();
	m_pCapture->capture(strDir + "/" + TEMP_FILE_NAME);
}
