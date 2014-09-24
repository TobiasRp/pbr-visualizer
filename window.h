#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

#include "RIV/PBRTConfig.h"
#include "imagewidget.h"
#include "heatmapwidget.h"
#include "pcwidget.h"
#include "scenewidget.h"
#include "RIV/Graphics/BMPImage.h"

class Window : public QMainWindow
{
    Q_OBJECT
private:
	RIVDataSet dataset;
	PBRTConfig pbrtConfiguration;
	
	void createWidgets();
	
	SceneWidget* sceneWidget = NULL;
	PCWidget* pcWidget = NULL;
	ImageWidget* imageWidget = NULL;
	HeatmapWidget* heatmapWidget = NULL;
public:
    explicit Window(const RIVDataSet& dataset, BMPImage* image, const PBRTConfig& config, QWidget *parent = 0);
	
	//QT event callback functionss
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent ( QResizeEvent* event );
signals:

public slots:

};

#endif // WINDOW_H