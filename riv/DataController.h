//
//  DataController.h
//  embree
//
//  Created by Gerard Simons on 16/11/14.
//
//

#ifndef __embree__DataController__
#define __embree__DataController__

#include <mutex>

#include "Data/DataSet.h"
#include "Data/SampleSet.h"
#include "Data/Histogram.h"

#include "devices/device_singleray/dataconnector.h"

class DataController {
private:
	
	std::vector<RIVDataSetListener*> dataListeners;
	void notifyFilterListeners();
	void notifyDataListeners();
	
	std::mutex mutex;
	
	//The datasets currently being used, this is what the views use to draw
	RIVDataSet<float,ushort>* rendererDataOne;
	RIVDataSet<float,ushort>* rendererDataTwo;
	
	//Fresh dataset that is a candidate for becoming the current data set
	RIVDataSet<float,ushort>* candidateDataOne;
	RIVDataSet<float,ushort>* candidateDataTwo;
	
	//Bootstrap sets
	RIVDataSet<float,ushort>* bootstrapOne = NULL;
	RIVDataSet<float,ushort>* bootstrapTwo = NULL;
	
	//The scores of the bootstrap sets for the renderers
	float bestBootstrapResultOne = -1;
	float bestBootstrapResultTwo = -1;
	
	/* Histograms to approximate the true distribution */
	HistogramSet<float,ushort> trueDistributionsOne;
	HistogramSet<float,ushort> trueDistributionsTwo;
	
	/* Shortcut pointers for quick access */
	RIVTable<float,ushort>* currentPathTable;
	RIVTable<float,ushort>* currentIntersectionsTable;
//
//	RIVShortRecord *rendererId = NULL;
	RIVFloatRecord* xPixels = NULL;
	RIVFloatRecord* yPixels = NULL;
	RIVFloatRecord* lensUs = NULL;
	RIVFloatRecord* lensVs = NULL;
	RIVFloatRecord* times = NULL;
	RIVFloatRecord* colorRs = NULL;
	RIVFloatRecord* colorGs = NULL;
	RIVFloatRecord* colorBs = NULL;
	RIVFloatRecord* throughputRs = NULL;
	RIVFloatRecord* throughputGs = NULL;
	RIVFloatRecord* throughputBs = NULL;
	RIVShortRecord *depths = NULL;
	
	RIVShortRecord* bounceNrs = NULL;
	RIVFloatRecord* xs = NULL;
	RIVFloatRecord* ys = NULL;
	RIVFloatRecord* zs = NULL;
	RIVFloatRecord* isectColorRs = NULL;
	RIVFloatRecord* isectColorGs = NULL;
	RIVFloatRecord* isectColorBs = NULL;
	RIVShortRecord *primitiveIds = NULL;
	RIVShortRecord *shapeIds = NULL;
	RIVShortRecord *interactionTypes = NULL;
	RIVShortRecord *lightIds = NULL;
	
	RIVMultiReference* pathsToIsectRef = NULL;
	RIVSingleReference* isectsToPathsRef = NULL;
	
	const int bins = 10;
	int reduceRounds = 0;
	
	int lastFrameOne = 0;
	size_t pathsPerFrameOne = 0;
	size_t raysPerFrameOne = 0;
	float acceptProbabilityOne = .5F;
	
	int lastFrameTwo = 0;
	size_t pathsPerFrameTwo = 0;
	size_t raysPerFrameTwo = 0;
	float acceptProbabilityTwo = .5F;
	
	//The number of data points per renderer
	std::map<ushort,size_t> pathCounts;
	
	//The first time we will fill both the candidate and current, after which we will bootstrap, keep the best bootstrap as current and only fill up candidate
	bool firstTime = true;
	
	bool paused = false;
	
	bool delayed = false;
	clock_t startDelay;
	int delayTimerInterval = 10000;
	
	const size_t maxPaths;
	size_t updateThrottle = 0;
	const size_t bootstrapRepeat = 1000;
	
	//Generate the datasets; create tables records and the histogramset
	void createDataStructures();
	void clearPathCounts();
	void createHistograms();
	
	void initDataSet(RIVDataSet<float,ushort> *dataset) ;
	//Setup the shortcut pointer to the innards of the dataset, tables etc.
	void resetPointers(RIVDataSet<float,ushort>* dataset);
public:

	RIVDataSet<float,ushort>* Bootstrap(RIVDataSet<float, ushort>* dataset, const size_t N);
	void Unpause() {
		paused = false;
	}
	void Delay() {
		printf("Delay DataController....\n");
		startDelay = clock();
		delayed = true;
	}
	void TogglePause() {
		printf("DataController is now ");
		if(paused) {
			printf("running.\n");
			Unpause();
		}
		else {
			printf("paused.\n");
			Pause();
		}
	}
	void Pause() {
		paused = true;
		printf("DataController is now paused..\n");
	}
	bool IsActive() { //The data controller is inactive either when it has been manually paused or is still delayed
		return !(paused || IsDelayed());
	}
	bool IsDelayed() {
		if(delayed) {
			int timeDelayed = (clock() - startDelay) / (float)CLOCKS_PER_SEC * 1000;
			if(timeDelayed < delayTimerInterval) {
				return true;
			}
			else {
				printf("END Delay DataController....\n");
				delayed = false;
				return false;
			}
		}
		return false;
		
	}
	void SetAcceptProbabilityOne(float newProb);
	void SetAcceptProbabilityTwo(float newProb);
	//The number of renderers to expect data from and the maximum number of paths per renderer before data reduction should kick in
	DataController(const ushort renderers, const size_t maxPaths);
	//Returns a pointer to a pointer of the dataset for renderer one
	RIVDataSet<float,ushort>** GetDataSetOne();
	//Returns a pointer to a pointer of the dataset for renderer two
	RIVDataSet<float,ushort>** GetDataSetTwo();
	bool ProcessNewPath(int frame, ushort renderer, PathData* newPath);
	//Reduce the data, first dataset is the current data being used for a renderer, candidate data is the new dataset, bestBootstrap is the slot used for creating and maintaining the best bootstrap and the best bootstrap results so far...
	void Reduce(RIVDataSet<float,ushort>** rendererData, RIVDataSet<float,ushort>** candidateData, HistogramSet<float,ushort>* trueDistribution, RIVDataSet<float,ushort>** bestBootstrap, float* bestBootstrapResult);
	void RendererOneFinishedFrame(size_t numPaths,size_t numRays);
	void RendererTwoFinishedFrame(size_t numPaths,size_t numRays);
};
#endif /* defined(__embree__DataController__) */