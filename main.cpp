#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include "Filter.h"
#include "DataView.h"
#include "ParallelCoordsView.h"
#include "DataSet.h"
#include "ImageView.h"
#include "DataFileReader.h"
#include "3DView.h"
#include "ColorPallete.h"
#include "ColorProperty.h"
#include "SizeProperty.h"
#include "UIView.h"
#include "HeatMapView.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

const float DEG2RAD = 3.14159/180;

/* window width and height */
//int width = 1680;
//int height = 1000;

int width = 1400;
int height = 800;

RIVClusterSet* clusters; //HERE ONLY FOR DEBUG REASONS
const size_t clusterK = 2;

bool isDirty = true;

/* window position */
int posX = 0;
int posY = 0;

int mainWindow;                   /* GLUT window handle */

/* All the sub window handles of the custom views */
int imageViewWindow;
int sceneViewWindow;
int parallelViewWindow;
int uiViewWindow;
int heatMapViewWindow;

/* For debugging purposes, keep track of mouse location */
int lastMouseX,lastMouseY = 0;

/* Contains pointers to all the views to be drawn */
std::vector<RIVDataView*> views;

RIVImageView *imageView = NULL;
RIV3DView *sceneView = NULL;
ParallelCoordsView *parallelCoordsView = NULL;
RIVHeatMapView *heatMapView = NULL;
UIView *uiView = NULL;

/* The dataset, views have pointers to this in order to draw their views consistently */
RIVDataSet dataset;

/* Image loaded as texture for the image view */
BMPImage image;

/* The 3D model */
MeshModel model;

std::string bmpPath = "";
std::string dataPath = "";
std::string pbrtPath = "";

/* Draw the window - this is where all the GL actions are */
void display(void)
{
    printf("Main display function called.\n");
    // Clear frame buffer
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Present frame buffer
    glutSwapBuffers();
}

/* Handles mouse input */
void mouse(int button, int state, int x, int y) {
	//Check what view catches the interaction
	y = height - y; //Very important to invert Y!
	lastMouseX = x;
	lastMouseY = y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouse(button,state,x,y)) {
            printf("View %s caught the MOUSE interaction\n",views[i]->identifier.c_str());
            //            if(state == GLUT_UP)
			glutPostRedisplay();
			return;
		}
	}
}

std::string lastMotionCatch = "";

void motion(int x, int y) {
	y = height - y;
	for(size_t i = 0 ; i < views.size() ; i++) {
		if(views[i]->HandleMouseMotion(x,y)) {
            if(views[i]->identifier != lastMotionCatch) {
                printf("View %s caught the MOTION interaction\n",views[i]->identifier.c_str());
                lastMotionCatch = views[i]->identifier;
            }
			glutPostRedisplay();
			return;
		}
	}
}

///* Handles mouse input */
//void mouse(int button, int state, int x, int y) {
//    sceneView->HandleMouse(button, state, x, y);
//}

//void motion(int x, int y) {
//	sceneView->HandleMouseMotion(x, y);
//}

void invalidateAllViews() {
	for(size_t i = 0 ; i < views.size() ; i++) {
        views[i]->Invalidate();
    }
}

void keys(int keyCode, int x, int y) {
    //    printf("Pressed %d at (%d,%d)\n",keyCode,x,y);
    bool postRedisplay = true;
    switch(keyCode) {
        case 27: //ESC key
            printf("Clear filters\n");
            //            invalidateAllViews();
            dataset.ClearFilters();
            break;
        case 98: // 'b' key
            glutSwapBuffers();
            printf("Manual swap buffers\n");
            //            copy_buffer();
            postRedisplay = true;
            break;
        case 99: // 'c' key
            if(sceneView) {
                sceneView->ToggleDrawClusterMembers();
                postRedisplay = true;
            }
            break;
        case 114: // 'r' key, recluster {
        {
            RIVTable *intersectTable = dataset.GetTable("intersections");
            intersectTable->Cluster("intersection X","intersection Y","intersection Z",clusterK,1);
            postRedisplay = true;
            break;
        }
        case 111: // 'o' key, optimize clusters (debug feature)
        {
            clusters->OptimizeClusters();
            postRedisplay = true;
            break;
        }
        case 116: // 't' key, use as temp key for some to-test function
        {
            imageView->createTextureImage();
            postRedisplay = true;
            break;
        }
        case GLUT_KEY_UP:
            sceneView->MoveCamera(0,0,1.F);
            uiView->MoveMenu(0,-10.F);
            break;
        case GLUT_KEY_DOWN:
            sceneView->MoveCamera(0,0,-1.F);
            uiView->MoveMenu(0,10);
            break;
        case GLUT_KEY_LEFT:
            sceneView->MoveCamera(-1.F,0,0);
            uiView->MoveMenu(-10.F,0);
            break;
        case GLUT_KEY_RIGHT:
            //            sceneView->MoveCamera(1.F,0,0);
            uiView->MoveMenu(10.F,0);
            break;
        default:
            postRedisplay = false;
    }
    if(postRedisplay) {
        glutPostRedisplay();
    }
}

void idle() {
    //Do animations?
}

/* Called when window is resized,
 also when window is first created,
 before the first call to display(). */
void reshape(int w, int h)
{
    printf("MAIN reshape called.\n");
    /* save new screen dimensions */
    width = w;
    height = h;
    
    int padding = 10;
    
    //Reshape and reposition all windows according to new dimensions
    
    //Parallel view window
    glutSetWindow(parallelViewWindow);
    glutPositionWindow(padding,padding);
    glutReshapeWindow(width-2*padding,height/2-2*padding); //Upper half and full width of the main window
    //
    //    //image view window
    glutSetWindow(imageViewWindow);
    glutPositionWindow(padding, height/2+padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding); //Square bottom left corner
    //
    //    //3D scene view inspector window
    glutSetWindow(sceneViewWindow);
    glutPositionWindow(padding + height/2, height/2+padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding); //Square bottom right next to imageview window

    //Heat map view
    glutSetWindow(heatMapViewWindow);
    glutPositionWindow(padding + height, height / 2 + padding);
    glutReshapeWindow(height / 2 - 2*padding, height /2 - 2 *padding);
    
    //    //UI view window
    glutSetWindow(uiViewWindow);
    glutPositionWindow(padding + 1.5f * height, height / 2 + padding);
    glutReshapeWindow(height /2 - 2*padding, height/2 - 2 *padding);
    

}

void generatePaths(int argc, char* argv[]) {
    if(argc < 4) {
        throw "Too few arguments given, 3 are expected.";
    }
    if(argc == 4) {
        dataPath = argv[1];
        pbrtPath = argv[2];
        bmpPath = argv[3];
    }
}

void loadData() {
    if(!dataPath.empty() && !pbrtPath.empty()) {
        BMPImage image = BMPImage(bmpPath.c_str(),false);
        //        dataset = DataFileReader::ReadAsciiData(dataPath + ".txt",image,0);
        dataset = DataFileReader::ReadBinaryData(dataPath + ".bin",image,0);
        model = DataFileReader::ReadModelData(pbrtPath);
    }
    else throw "Data paths not generated.";
}

void createViews() {
    if(dataset.IsSet() && bmpPath.size() > 0) {
        RIVSizeProperty *defaultSizeProperty = new RIVFixedSizeProperty(0.1);
        RIVColorProperty *defaultColorProperty = new RIVFixedColorProperty(1,1,1);
        
        parallelViewWindow = glutCreateSubWindow(mainWindow,0,height / 2.F,width,height / 2.F);

        glutSetWindow(parallelViewWindow);
        glutDisplayFunc(ParallelCoordsView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(ParallelCoordsView::ReshapeInstance);
        glutMouseFunc(ParallelCoordsView::Mouse);
        glutMotionFunc(ParallelCoordsView::Motion);
        glutSpecialFunc(keys);
        
        //Load image
        BMPImage image = BMPImage(bmpPath.c_str(),false);
        //image view window
        imageViewWindow = glutCreateSubWindow(mainWindow,0,0,width,height / 2.F);
        glutSetWindow(imageViewWindow);
        imageView = new RIVImageView(image, 0, 0, height / 2.F, height / 2.F, 0, 0, defaultColorProperty, defaultSizeProperty); //If this is not supplied on constructor, the texture becomes garbled
        glutDisplayFunc(RIVImageView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(RIVImageView::ReshapeInstance);
        glutMouseFunc(RIVImageView::Mouse);
        glutMotionFunc(RIVImageView::Motion);
        glutSpecialFunc(keys);
        //
        sceneViewWindow = glutCreateSubWindow(mainWindow, 0, 0, 0, 0);
        glutSetWindow(sceneViewWindow);
        glutDisplayFunc(RIV3DView::DrawInstance);
//        glutDisplayFunc(idle);
        glutReshapeFunc(RIV3DView::ReshapeInstance);
        glutMouseFunc(RIV3DView::Mouse);
        glutMotionFunc(RIV3DView::Motion);
        glutSpecialFunc(keys);
        //
        uiViewWindow = glutCreateSubWindow(mainWindow, 0, 0, 0, 0);
        glutSetWindow(uiViewWindow);
        glutReshapeFunc(UIView::ReshapeInstance);
        glutDisplayFunc(UIView::DrawInstance);
//        glutDisplayFunc(idle);
        glutMouseFunc(UIView::Mouse);
        glutMotionFunc(UIView::Motion);
        glutEntryFunc(UIView::Entry);
        
        heatMapViewWindow = glutCreateSubWindow(mainWindow,0,0,0,0);
        glutSetWindow(heatMapViewWindow);
        glutReshapeFunc(RIVHeatMapView::ReshapeInstance);
        glutDisplayFunc(RIVHeatMapView::DrawInstance);
//        glutDisplayFunc(RIV3DView::DrawInstance);
        glutMouseFunc(RIVHeatMapView::Mouse);
        glutMotionFunc(RIVHeatMapView::Motion);
        
        //Create views
        parallelCoordsView = new ParallelCoordsView();
        sceneView = new RIV3DView();
        uiView = new UIView(defaultColorProperty,defaultSizeProperty);
        heatMapView = new RIVHeatMapView();
        
        //Set data
        imageView->SetData(&dataset);
        parallelCoordsView->SetData(&dataset);
        sceneView->SetData(&dataset);
        heatMapView->SetData(&dataset);
        
        //Set the 3D model loaded from the PBRT file
        sceneView->SetModelData(model);
        
        //Add some filter callbacks
        dataset.AddFilterListener(sceneView);
        dataset.AddFilterListener(parallelCoordsView);
        
        //Add the views to the view vector
        views.push_back(sceneView);
        views.push_back(imageView);
        views.push_back(parallelCoordsView);
        views.push_back(heatMapView);
    }
    else {
        throw "Data must be loaded first.";
    }
}

void initializeViewProperties() {
    RIVTable *imageTable = dataset.GetTable("image");
    RIVTable *pathTable = dataset.GetTable("path");
    RIVTable *intersectionstTable = dataset.GetTable("intersections");
    
    //    imageTable->FilterRowsUnlinkedTo(pathTable);
    
    RIVRecord* bounceRecord = intersectionstTable->GetRecord("bounce#");
    
    //    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty(pathTable,colors::GREEN,colors::RED);
    RIVColorProperty *colorProperty = new RIVEvaluatedColorProperty<float>(intersectionstTable,bounceRecord,colors::GREEN,colors::RED);
    RIVSizeProperty *sizeProperty = new RIVFixedSizeProperty(.05F);
    
    parallelCoordsView->SetColorProperty(colorProperty);
    sceneView->SetSizeProperty(sizeProperty);
    sceneView->SetColorProperty(colorProperty);
    
    //TODO apply color and size properties to views
}

void clusterAndColor() {
    dataset.ClusterTable("intersections","intersection X","intersection Y","intersection Z",clusterK,1);
    std::vector<size_t> medoidIndices = dataset.GetClusterSet()->GetMedoidIndices();
    RIVTable *intersectionsTable = dataset.GetTable("intersections");
    RIVEvaluatedColorProperty<size_t>* colorByCluster = new RIVEvaluatedColorProperty<size_t>(intersectionsTable);
    RIVClusterSet& clusterSet = intersectionsTable->GetClusterSet();
    
    float nrOfClusters = (float)clusterSet.Size();
    
    std::map<size_t,float> indexToClusterSize;
    std::map<size_t,float> indexToClusterMembership;
    
    for(size_t j = 0 ; j < clusterSet.Size() ; ++j) {
        float clusterRatio = j / (nrOfClusters  - 1.F);
        //        printf("clusterRatio = %f\n",clusterRatio);
        Evaluator<size_t,float>* eval = new FixedEvaluator<size_t, float>(clusterRatio);
        RIVCluster* cluster = clusterSet.GetCluster(j);
        std::vector<size_t> members = cluster->GetMemberIndices();
        members.push_back(cluster->GetMedoidIndex());
        
        colorByCluster->AddEvaluationScheme(members, eval);
    }
    
    //    DiscreteEvaluator<size_t, float> colorByClusterEvaluator = new DiscreteEvaluator<size_t, float>(indexToClusterMembership);
    
    RIVEvaluatedSizeProperty<size_t> *sizeByCluster = new RIVEvaluatedSizeProperty<size_t>(intersectionsTable,.05F);
    std::vector<float> relativeSizes;
    float minSize = .4F;
    float maxSize = .8F;
    for(RIVCluster* cluster : clusterSet.GetClusters()) {
        float relativeSize = clusterSet.RelativeSizeOf(cluster);
        printf("\nTotal size of cluster : %zu\n",cluster->Size());
        printf("Relative size of cluster : %f\n",relativeSize);
        //        relativeSizes.push_back(clusterSet.RelativeSizeOf(cluster));
        Evaluator<size_t, float>* clusterSizeEval = new FixedEvaluator<size_t, float>((1-relativeSize) * minSize + relativeSize * maxSize);
        sizeByCluster->AddEvaluationScheme(cluster->GetMedoidIndex(), clusterSizeEval);
    }
    
    parallelCoordsView->SetColorProperty(colorByCluster);
    parallelCoordsView->SetSizeProperty(sizeByCluster);
    
    sceneView->SetColorProperty(colorByCluster);
    sceneView->SetSizeProperty(sizeByCluster);
}

int main(int argc, char **argv)
{
    generatePaths(argc, argv);
    
    srand(time(NULL));
    /* initialize GLUT, let it extract command-line
     GLUT options that you may provide */
    glutInit(&argc, argv);
    
    //Use double buffering!
    //    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    //    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    
    /* set the initial window size */
    glutInitWindowSize(width, height);
    
    /* set the initial window position */
    glutInitWindowPosition(posX,posY);
    
    /* create the window and store the handle to it */
    mainWindow = glutCreateWindow("Rendering InfoVis" /* title */ );
    
    /* register function to handle window resizes */
    glutReshapeFunc(reshape);
    
    // display and idle function
    glutDisplayFunc(display);
    //    glutIdleFunc(renderSceneAll);
    
    /* --- register callbacks with GLUT ---     */
    
    /* register function that handles mouse */
    glutMouseFunc(mouse);
    
    glutSpecialFunc(keys);
    
    glutMotionFunc(motion);
    
    loadData();
    
    createViews();
    
    initializeViewProperties();
    
    /* Transparency stuff */
    glEnable (GL_BLEND);
    
    glClearColor(1,1,1, 0.0);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* start the GLUT main loop */
    glutMainLoop();
    
    
    return EXIT_SUCCESS;
}
