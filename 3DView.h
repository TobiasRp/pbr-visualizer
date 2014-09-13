//
//  3DView.h
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen___DView__
#define __Afstuderen___DView__

#include "DataView.h"
#include "MeshModel.h"
#include "Octree.h"
#include "Ray.h"
#include <limits>

class RIV3DView : public RIVDataView, public RIVDataSetListener {
private:
    MeshModel modelData;
//    float cameraPosition[3] = {278, 273, -200}; //The camera used by PBRT for the rendered scene, not used as camera for OpenGL rendering! TODO: Read from file
    float cameraPosition[3] = {278, 273, -500}; //The original
    Point3D eye;
    bool isDirty = true;
    //Whether to draw the members of clusters, or only the cluster medoid
    bool drawClusterMembers = false;
	//Whether the generated octree should be drawn (if any is generated)
	bool drawHeatmapTree = false;
	bool drawLightPaths = false;
	
	//Octree generated from 3D points (generated in createPoints)
	Octree* heatmap = NULL;
	ColorMap treeColorMap;
    
    //Path drawing variables
    int maxBounce = 5; //TODO: Deduce this value from the bounce record
    
    const float segmentWidth = .05F; // a tenth of the total path length
    float segmentStart = 0;
    float segmentStop = segmentWidth;
    
    //The mouse cursor in world coordinates at last click
    Point3D cursorNear;
    Point3D cursorFar;
	
	Vec3Df Phit; //Supposedly the point of intersection of the ray with the plane supporting the triangle
	Ray<float> pickRay;
	bool meshSelected = false;
	size_t selectedMeshIndex;
	
	RIVTable* isectTable = NULL;
    
    //Buffered graphics point data, generated from the data, stored here for speed, TODO: Only store indices and a pointer to these records?
	bool sizesAllTheSame; //Because sizes are often set to the same, we take advantage of this to get a big performance boost
	//Indices of the points to draw
	std::vector<size_t> pointsToDraw;
    std::vector<float> pointsR;
    std::vector<float> pointsG;
    std::vector<float> pointsB;
	std::vector<float> pointsSize;

    //Possibly the selection box created by clicking and dragging (NOT IMPLEMENTED)
    Box3D selectionBox;
    
	//Start and end point of selection ray
	Point3D selectNear;
	Point3D selectFar;

	//Generate a octree from the unfiltered intersection points
	void generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize);
	//Draw the mesh model loaded from the PBRT file
	void drawMeshModel();
    void drawPaths(float startSegment, float stopSegment); //Draw the paths between two consecutive bounces
	//Draw octree representation of the generated points
	void drawHeatmap();
	//Draw the intersection points
	void drawPoints();
	//draw the leaf nodes starting from the given node
	void drawLeafNodes(OctreeNode* node);
    
    static RIV3DView* instance;
    Point3D screenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView();
    RIV3DView(int,int,int,int,int,int,RIVColorProperty*,RIVSizeProperty*);
    RIV3DView(RIVColorProperty*,RIVSizeProperty*);
	
	static int windowHandle;
	
	//Create graphics buffer from unfiltered data rows
    void createPoints();
    
    void Reshape(int newWidth, int newHeight);
    void Draw();
    bool HandleMouse(int button, int state, int x, int y);
    bool HandleMouseMotion(int x, int y);
    void OnDataSetChanged();
    
    void SetData(RIVDataSet *newDataSet) {
		RIVDataView::SetData(newDataSet);
		isectTable = dataset->GetTable("intersections");
	};
	
	void InitializeGraphics();
	
    static void DrawInstance(); //Override
    static void ReshapeInstance(int,int);
    static void Mouse(int button, int state, int x, int y);
    static void Motion(int x, int y);
    
    void MovePathSegment(float ratioIncrement);
    void CyclePathSegment(bool direction = true); //Cycle the path segment to draw, direction bool indicates direction of cycling, positive meaning incrementing
    void ToggleDrawClusterMembers();
	void ToggleDrawHeatmap();
	void ToggleDrawPaths();
    void SetModelData(const MeshModel&);
    void MoveCamera(float,float,float);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
