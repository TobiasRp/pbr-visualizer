//
//  3DView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 19/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "3DView.h"
#include "helper.h"
#include "Geometry.h"
#include "helper.h"
#include "reporter.h"
#include "trackball.h"

#include "graphics_helper.h"

#if __APPLE__
    #include <GLUT/GLUT.h>
#else
	#include <GL/glut.h>
#endif

//Init instance to draw
RIV3DView* RIV3DView::instance = NULL;

//TODO : Move these to the header file
const float sizeMultiplier = 5.F;

RIV3DView::RIV3DView() {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    sizeProperty = new RIVFixedSizeProperty(0.1F);
    const float black[] = {0,0,0};
    colorProperty = new RIVFixedColorProperty(black);
    instance = this;
    identifier = "3DView";
}

RIV3DView::RIV3DView(int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(x,y,width,height,paddingX,paddingY,colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
    
};

RIV3DView::RIV3DView(RIVColorProperty *colorProperty, RIVSizeProperty* sizeProperty) : RIVDataView(colorProperty,sizeProperty) {
    if(instance != NULL) {
        throw "Only 1 instance of RIV3DView allowed.";
    }
    instance = this;
    identifier = "3DView";
};

//void RIV3DView::initialize() {
//	
//}

void RIV3DView::CyclePathSegment(bool direction) {
    float delta = 1.F / maxBounce;
    direction ? MovePathSegment(delta) : MovePathSegment(-delta);
}

void RIV3DView::Reshape(int newWidth, int newHeight) {
    width = newWidth;
    this->height = newHeight;
    
    eye.x = 0;
    eye.y = 0;
    eye.z = 2;
    
//    selectionBox = Box3D(0,0,0,1.F,1.F,1.F);
    
//    cursorNear.x = 0.F;
//    cursorNear.x = 0.F;
//    cursorNear.z = zNear;
//    
//    cursorFar.x = 0.F;
//    cursorFar.x = 0.F;
//    cursorFar.z = zFar;
    
    tbInitTransform();
    tbHelp();
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double)width/height, 1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void RIV3DView::ToggleDrawClusterMembers() {
    drawClusterMembers = !drawClusterMembers;
    isDirty = true;
}

//Test function to draw a simple octree
void RIV3DView::drawOctree() {
	if(tree) {
		//Draw the tree to the required depth
	}
}

void RIV3DView::Draw() {
//    printf("3DView Draw!\n");
	
	reporter::startTask("3D Draw");
	
    glEnable(GL_DEPTH_TEST);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear( GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-eye.x,-eye.y,-eye.z);
//    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    
//    printf("eye (x,y,z) * modelScale = (%f,%f,%f)\n",-eye.x * modelData.GetScale(),-eye.y * modelData.GetScale(),-eye.z * modelData.GetScale());
    
    tbVisuTransform();
    
    drawCoordSystem();
    
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glColor3f(.5f,.2f,1.0f); //Purple
    /** Draw the model **/
    glBegin(GL_QUADS);
    const std::vector<float>& vertices = modelData.GetVertices();
    for(size_t i = 0 ; i < vertices.size() ; i += 3) {
        glVertex3f(vertices[i], vertices[i+1], vertices[i+2]);
    }
    glEnd();
    
    /* Draw the intersection positions */
    GLUquadric* quadric = gluNewQuadric();
    
    //Draw
    glColor3f(1, 1, 1);
    Point3D modelCenter = modelData.GetCenter();

//    glPushMatrix();
//    glLoadIdentity();
    glScalef(modelData.GetScale(), modelData.GetScale(), modelData.GetScale());
    glTranslatef(-modelCenter.x, -modelCenter.y, -modelCenter.z);
    
//    Translate -278.000000 -273.000000 500.000000
    //Draw camera position
    glPushMatrix();
    glTranslatef(278, 273, -500);
//    glTranslatef(0, 0, 0);
    glScalef(1/modelData.GetScale(), 1/modelData.GetScale(), 1/modelData.GetScale());
    glScalef(0.01, 0.01, 0.01);
    gluSphere(quadric, 5, 5, 5);
    glPopMatrix();

	drawPoints();

//    reporter::stop(drawTask);
    glPopMatrix();
    
    //Draw some lines
	drawPaths(segmentStart,segmentStop);
    
    glFlush();
    
    glutSwapBuffers();
	
	reporter::stop("3D Draw");
}

void RIV3DView::drawPoints() {
	//Draw the points if requested
	if(drawClusterMembers) {
		RIVFloatRecord* xRecord = isectTable->GetRecord<RIVFloatRecord>("intersection X");
		RIVFloatRecord* yRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Y");
		RIVFloatRecord* zRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Z");
		
		//Only use 1 size
		float size = sizeProperty->ComputeSize(isectTable, 0);
		
		if(sizesAllTheSame) {
			glPointSize(size);
			printf("Sizes are all the same!\n");
		}
		
		if(isectTable != NULL) {
			TableIterator* iterator = isectTable->GetIterator();
			size_t row;
			glBegin(GL_POINTS);
			while(iterator->GetNext(row)) {
				const float* color = colorProperty->Color(isectTable, row);
				//			float const* color = colorProperty->Color(isectTable, row); //Check if any color can be computed for the given row
				
				if(!sizesAllTheSame) {
					glPointSize(pointsSize[row]);
				}
				
				float x = xRecord->Value(row);
				float y = yRecord->Value(row);
				float z = zRecord->Value(row);
				
				glPushMatrix();
				glColor3fv(color);
				glVertex3f(x,y,z);
				
			}
			glEnd();
		}
	}
}

//Move this function somewhere else
void RIV3DView::generateOctree(size_t maxDepth, size_t maxCapacity, float minNodeSize) {
	
	std::string taskName = "Generating octree";
	reporter::startTask(taskName);
	
	if(tree) {
		delete tree;
	}
	
	size_t row;
	
	RIVTable* isectTable = dataset->GetTable("intersections");
	TableIterator *iterator = isectTable->GetIterator();
	
	//Generate the index subset
	std::vector<size_t> indices;
	
	while(iterator->GetNext(row)) {
		indices.push_back(row);
	}
	
	RIVFloatRecord* xRecord = isectTable->GetRecord<RIVFloatRecord>("intersection X");
	std::vector<float>* xValues = xRecord->GetValuesPointer();
	RIVFloatRecord* yRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Y");
	std::vector<float>* yValues = yRecord->GetValuesPointer();
	RIVFloatRecord* zRecord = isectTable->GetRecord<RIVFloatRecord>("intersection Z");
	std::vector<float>* zValues = zRecord->GetValuesPointer();
	
	OctreeConfig config = OctreeConfig(maxDepth, maxCapacity, minNodeSize);
	
	tree = new Octree(xValues, yValues, zValues, indices, config);
	
	printf("Tree generated with \n");
	printf("\tDepth %zu\n",tree->Depth());
	printf("\tNodes = %zu\n",tree->NumberOfNodes());
	
	reporter::stop(taskName);
	
	
}

//Create buffered data for points, not working anymore, colors seem to be red all the time.
void RIV3DView::createPoints() {
    
    //Clear buffer data
    pointsR.clear();
    pointsG.clear();
    pointsB.clear();
    pointsSize.clear();
	
	std::vector<float> uniqueSizes;
    
    //Get the records we want;
    //Get the iterator, this iterator is aware of what rows are filtered and not
    TableIterator *iterator = isectTable->GetIterator();
    
    size_t row = 0;
	
	sizesAllTheSame = true;
    
    while(iterator->GetNext(row)) {
        float const* color = colorProperty->Color(isectTable, row); //Check if any color can be computed for the given row
		
//		printf("Computed color for point : ");
//        printArray(color, 3);
//		printf("\n");
		
        if(color != NULL) {
            
			float size = sizeProperty->ComputeSize(isectTable, row);
			if(uniqueSizes.empty()) {
				uniqueSizes.push_back(size);
			}
			
			if(sizesAllTheSame && !vectorContains(uniqueSizes, size)) {
				sizesAllTheSame = false;
			}
			else {
				pointsSize.push_back(sizeProperty->ComputeSize(isectTable, row));
			}
            pointsR.push_back(color[0]);
            pointsG.push_back(color[1]);
            pointsB.push_back(color[2]);
        }
    }
	if(sizesAllTheSame) {
		pointsSize.clear(); //Not necessary
	}
    
    printf("%zu points created.\n",pointsX.size());
}

void RIV3DView::MovePathSegment(float ratioIncrement) {
    segmentStart += ratioIncrement;
    segmentStop += ratioIncrement;
    
    float undershoot = 0 - segmentStart;
    if(undershoot > 0) {
        segmentStart += undershoot;
        segmentStop += undershoot;
        
//        else {
//            segmentStart = 1.F - segmentWidth;
//            segmentStop = 1;
//            CyclePathSegment(false);
//        }
    }
    float overshoot = segmentStop - 1.F;
    if(overshoot > 0) {
        segmentStart -= overshoot;
        segmentStop -= overshoot;
    }
}


void RIV3DView::drawPaths(float startSegment, float stopSegment) {
//    printf("drawPaths(%f,%f)\n",startSegment,stopSegment);
    if(showPaths) { //Drawing paths is turned off
        //See if it should consist of two partial segments
        for(float i = 1 ; i < maxBounce ; i++) {
            if(startSegment < i / maxBounce && stopSegment > i / maxBounce) {
//                printf("(%f,%f) segment split in (%f,%f) and (%f,%f)\n",startSegment,stopSegment,startSegment,i/maxBounce,i/maxBounce,stopSegment);
                drawPaths(startSegment, i / maxBounce);
                drawPaths(i / maxBounce, stopSegment);
                return;
            }
        }
        RIVTable *table = dataset->GetTable("intersections");
        //Get the records we want;
        RIVFloatRecord* xRecord = table->GetRecord<RIVFloatRecord>("intersection X");
        RIVFloatRecord* yRecord = table->GetRecord<RIVFloatRecord>("intersection Y");
        RIVFloatRecord* zRecord = table->GetRecord<RIVFloatRecord>("intersection Z");
        RIVUnsignedShortRecord *bounceRecord = table->GetRecord<RIVUnsignedShortRecord>("bounce#");
        //Get the iterator, this iterator is aware of what rows are filtered and not
        TableIterator *iterator = table->GetIterator();
        
        size_t row = 0;
        size_t lastRow = 0;
        ushort lastBounceNr = 0;
        float lastColor[3] = {0};
        
        bool pathStartFound = false;
        size_t linesDrawn = 0;
        
        if(startSegment < 1.F / maxBounce) { //Connect with camera
            glBegin(GL_LINES);
            while(iterator->GetNext(row)) {
                ushort bounceNr = bounceRecord->Value(row);
                if(bounceNr == 1) {
                    //Arbitrary camera color
                    float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
                    if(color) {
                        glColor3f(1, 1, 1); //White
                        float deltaX = xRecord->Value(row) - cameraPosition[0];
                        float deltaY = yRecord->Value(row) - cameraPosition[1];
                        float deltaZ = zRecord->Value(row) - cameraPosition[2];
                        glVertex3f(cameraPosition[0] + deltaX * startSegment * maxBounce,cameraPosition[1] + deltaY * startSegment * maxBounce,cameraPosition[2] + deltaZ * startSegment * maxBounce);
                        glColor3fv(color);
                        glVertex3f(cameraPosition[0] + deltaX * stopSegment * maxBounce,cameraPosition[1] + deltaY * stopSegment * maxBounce,cameraPosition[2] + deltaZ * stopSegment * maxBounce);
                    }
                }
            }
            glEnd();
            return;
        }
        
        glBegin(GL_LINES);
        while(iterator->GetNext(row)) {
            int bounce = floor(startSegment * maxBounce);
            float const* color = colorProperty->Color(table, row); //Check if any color can be computed for the given row
            if(color != NULL) {
                ushort bounceNr = bounceRecord->Value(row);
                if(!pathStartFound && bounceNr == bounce ) {
                    //                printf("Row %zu is valid start point for path\n",row);;
                    pathStartFound = true;
                }
                else if(pathStartFound && row == lastRow + 1 && bounceNr == bounce + 1) { //Valid path found, draw
                    //                printf("Row %zu is valid end point for path\n",row);
                    //                printf("Last color = ");
                    //                printArray(lastColor, 3);
                    
//                    printf("lastBounceNr = %d\n",lastBounceNr);
//                    printf("bounceNr = %d\n",bounceNr]);
                    
                    float lastX = xRecord->Value(lastRow);
                    float lastY = yRecord->Value(lastRow);
                    float lastZ = zRecord->Value(lastRow);
                    
                    float deltaX = xRecord->Value(row) - lastX;
                    float deltaY = yRecord->Value(row) - lastY;
                    float deltaZ = zRecord->Value(row) - lastZ;
                    
                    float C_one = linearInterpolate(startSegment * maxBounce, lastBounceNr, bounceNr);
                    float C_two = linearInterpolate(stopSegment * maxBounce, lastBounceNr, bounceNr);

                    //Sanity check
//                    if(C_one < 0 || C_one > 1 || C_two < 0 || C_two > 1) {
//                        
//                    }
                    glColor3fv(lastColor);
                    glVertex3f(lastX + deltaX * C_one, lastY + deltaY * C_one, lastZ + deltaZ * C_one);
                    glColor3fv(color);
                    glVertex3f(lastX + deltaX * C_two, lastY + deltaY * C_two, lastZ + deltaZ * C_two);
                    

//                    glVertex3f(lastX + deltaX * segmentStart,lastY + deltaY * segmentStart,lastZ + deltaZ * segmentStart);
//                    glColor3fv(color);
//                    glVertex3f(lastX + deltaX * segmentStop,lastY + deltaY * segmentStop,lastZ + deltaZ * segmentStop);
                    
                    linesDrawn++;
                    pathStartFound = false;
                }
                //            else if(bounceNr == startBounce) {
                ////                printf("Last row was not valid after all, it had no successor.\n");
                //
                //            }
                lastRow = row;
                lastBounceNr = bounceNr;
                lastColor[0] = color[0];
                lastColor[1] = color[1];
                lastColor[2] = color[2];
            }
        }
        glEnd();
    }
}

void RIV3DView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
    else {
        printf("No instance to draw.\n");
    }
}

void RIV3DView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void RIV3DView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void RIV3DView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
    else printf("No instance to reshape");
}

void RIV3DView::OnDataSetChanged() {
//    printf("3D View received on filter change.");
//    createPoints();
    glutPostRedisplay();
    isDirty = true;
}

void RIV3DView::MoveCamera(float x, float y, float z) {
    eye.x += x;
    eye.y += y;
    eye.z += z;
    
//    zNear += z;
//    zFar += z;
    
    printf("new eye (x,y,z) = (%f,%f,%f)\n",eye.x,eye.y,eye.z);
    isDirty = true;
}

Point3D RIV3DView::screenToWorldCoordinates(int screenX, int screenY, float zPlane) {
    
    Point3D worldPos;
    
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    
    double x,y,z;
    gluUnProject((GLdouble) screenX, (GLdouble) screenY, zPlane, mvmatrix, projmatrix, viewport, &x,&y,&z);
    
//    double distanceEyeToPlane = fabs(eye.z - zNear);
    
    //    printf("Distance near plane to camera = %f\n",distanceEyeToPlane);
//    if(distanceEyeToPlane == 0) {
//        distanceEyeToPlane = 1;
//    }
    //    worldPos.x = x * distanceEyeToPlane;
    //    worldPos.y = y * distanceEyeToPlane
    worldPos.x = x;
    worldPos.y = y;
    worldPos.z = z;
    //    printf("worldPos = [%f,%f,%f]\n",worldPos.x,worldPos.y,worldPos.z);
    
    return worldPos;
}

void RIV3DView::SetModelData(const MeshModel& model) {
    modelData = model;
}

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
//    printf("RIV3DView HandleMouse\n");
    y = height - y;
    printf("isDragging = %d\n",isDragging);
	if(isDragging || containsPoint(x,y)) {
        if(state == GLUT_DOWN) {
            isDragging = true;
            tbMouseFunc(button, state, x, y);
            return true;
        }
    }
    isDragging = false;
    return false;
}

bool RIV3DView::HandleMouseMotion(int x, int y) {
//    printf("RIV3DView HandleMouseMotion\n");
//    ToViewSpaceCoordinates(&x, &y);   
    y = height - y;
    if(isDragging) {
        tbMotionFunc(x, y);
        return true;
    }
    return false;
}

/*
int lastX = -1;
int lastY = -1;

bool RIV3DView::HandleMouse(int button, int state, int x, int y) {
    ToViewSpaceCoordinates(&x, &y);
	if(isDragging || containsPoint(x,y)) {
        ToViewSpaceCoordinates(&x, &y);
        if(state == GLUT_DOWN && !isDragging) {
            isDragging = true;
            lastX = x;
            lastY = y;
            isDirty = true;
            return true;
        } else if(state == GLUT_UP) {
            isDragging = false;
            isDirty = true;
            return true;
        }
    }
    return false;
}

bool RIV3DView::HandleMouseMotion(int x, int y) {
    if(isDragging) {
        int deltaX = x - startX;
        int deltaY = y - startY;
        
        float modifier = .5F;
        
        yRotated = -modifier * deltaX;
        xRotated = -modifier * deltaY;
        isDirty = true;
        return true;
    }
    return false;
}
*/
