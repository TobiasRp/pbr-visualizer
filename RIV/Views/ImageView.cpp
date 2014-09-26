//
//  ImageView.cpp
//  Afstuderen
//
//  Created by Gerard Simons on 01/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#include "ImageView.h"
#include "../Graphics/loadppm.h"
#include "../Data/Filter.h"

#include <math.h>

RIVImageView* RIVImageView::instance = NULL;

RIVImageView::RIVImageView(RIVDataSet* dataset, BMPImage *image, int x, int y, int width, int height, int paddingX, int paddingY,RIVColorProperty *colorProperty,RIVSizeProperty* sizeProperty) : RIVDataView(dataset,x,y,width,height, paddingX, paddingY,colorProperty,sizeProperty){
	renderedImage = image;
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    
    identifier = "ImageView";

	imageWidth = width;
	imageHeight = height;

	isDragging = false;
    
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
}

RIVImageView::RIVImageView(RIVDataSet* dataset, BMPImage* image, RIVColorProperty* color, RIVSizeProperty* size) : RIVDataView(dataset,color,size) {
	renderedImage = image;
    if(instance != NULL) {
        throw "Only 1 instance of ImageView allowed.";
    }
    instance = this;
    identifier = "ImageView";

    
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
}

void RIVImageView::InitializeGraphics() {
	createTextureImage();
}

RIVImageView::~RIVImageView() {
	
}

////This shit aint working, the data gets garbled up?!
void RIVImageView::createTextureImage() {
    createTextureImage(renderedImage);
}

void RIVImageView::DrawInstance() {
    if(instance != NULL) {
        instance->Draw();
    }
    else {
        printf("No instance to draw.\n");
    }
}

void RIVImageView::ReshapeInstance(int width, int height) {
    if(instance != NULL) {
        instance->Reshape(width,height);
    }
    else printf("No instance to reshape");
}

void RIVImageView::Mouse(int button, int state, int x, int y) {
    if(instance != NULL) {
        instance->HandleMouse(button,state,x,y);
    }
}

void RIVImageView::Motion(int x, int y) {
    if(instance != NULL) {
        instance->HandleMouseMotion(x, y);
    }
}

void RIVImageView::Reshape(int width, int height) {
    
    printf("RIVImageView Reshape\n");
    
    this->width = width;
    this->height = height;
    
    paddingX = 0;
    paddingY = 0;
    
    startX = 0;
    startY = 0;
    
    imageStart.x = startX + paddingX;
	imageStart.y = startY + paddingY;
    
	imageEnd.x = startX + width - paddingX;
	imageEnd.y = startY + height - paddingY;
    
    imageMagnificationX = (width - 2 * paddingX) / (float)renderedImage->sizeX;
	imageMagnificationY = (height - 2 * paddingY) / (float)renderedImage->sizeY;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, width, 0.0, height);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
//	createTextureImage(this->renderedImage);
}
void RIVImageView::createTextureImage(BMPImage* image) {
    glGenTextures(1, &imageTexture);
    glBindTexture(GL_TEXTURE_2D, imageTexture);
    
//    printf("image ID = %d\n",image->ID);
//    std::cout << *image;
    
    if(image->hasAlpha) {
//        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, image.sizeX, image.sizeY,GL_BGRA_EXT, GL_UNSIGNED_BYTE, image.data); //For BMP images use this
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->sizeX, image->sizeY, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image->data);
//		unsigned int error = glGetError();
//		printf("ERROR = %d\n",error);
    }
    else {
//        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_BGR_EXT, GL_UNSIGNED_BYTE, image.data); //For BMP images use this
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->sizeX, image->sizeY, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image->data);
    }
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.sizeX, image.sizeY,GL_RGB, GL_UNSIGNED_BYTE, image.data); //for P6 formatted PPM use this
    
    printf("Texture created.\n");
}

void RIVImageView::Draw() {
    needsRedraw = true;
	if(needsRedraw) {
		printf("\nImageView Draw!\n");
		glClearColor(1.0, 0, 0, 0.0);
		glClear( GL_COLOR_BUFFER_BIT );
		
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDisable(GL_DEPTH_TEST);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, imageTexture);
    
		//remember all states of the GPU
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColor3f(1,1,1);
//		glNormal3d(0, 0, -1);
		glEnable(GL_TEXTURE_2D);
    
		// texture addition
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,  GL_NEAREST);
        
//		glColor3f(1, 0, 0);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f,1.0f);
        glVertex3f(imageStart.x,imageEnd.y,1);
        glTexCoord2f(0.0f,0.0f);
        glVertex3f(imageStart.x,imageStart.y,1);
        glTexCoord2f(1.0f,0.0f);
        glVertex3f(imageEnd.x,imageStart.y,1);
        glTexCoord2f(1.0f,1.0f);
        glVertex3f(imageEnd.x,imageEnd.y,1);
		glEnd();

		//reset to previous state
		glPopAttrib();
    
		glDisable(GL_TEXTURE_2D);
		//if selection, draw
		if(selection.start.x != -1) {
			glColor4f(0.317F,.553F, .741F,.5F); //Nice light blue color for selection
			glBegin(GL_QUADS);
				glVertex3f(selection.start.x * imageMagnificationX,selection.start.y  * imageMagnificationY,1);			
				glVertex3f(selection.end.x  * imageMagnificationX,selection.start.y  * imageMagnificationY,1);
				glVertex3f(selection.end.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
				glVertex3f(selection.start.x  * imageMagnificationX,selection.end.y  * imageMagnificationY,1);
			glEnd();
		}
		//needsRedraw = false; //TODO: This does not work when losing and regaining focus!
		glutSwapBuffers();
	}
}

void RIVImageView::OnDataSetChanged() {
    //Do nothing for imageview
    
}

#ifdef __APPLE__
int round(float d)
{
  return (int)floor(d + 0.5F);
}
#endif

bool RIVImageView::HandleMouse(int button, int state, int x, int y) {
//    ToViewSpaceCoordinates(&x, &y);
    y = height - y;
	if(isDragging || containsPoint(x,y)) {
		//If start dragging > init selection
		if(state == GLUT_DOWN) {
			//init selection
			if(!isDragging) {
				selection.start = viewToPixelSpace(x,y);
                printf("selection (startX,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
				isDragging = true;
			}
		}
		else if(state == GLUT_UP) {
			//Finish selection

			printf("selection (startX,endX) = (%d,%d)\n",selection.start.x,selection.end.x);
            printf("selection (startY,endY) = (%d,%d)\n",selection.start.y,selection.end.y);

			if(selection.end.x != selection.start.x && selection.end.y != selection.start.y) {

				dataset->ClearFilter("x");
				dataset->ClearFilter("y");

				//Normalize selection
				if(selection.end.x < selection.start.x) {
					int tempX = selection.start.x;
					selection.start.x = selection.end.x;
					selection.end.x = tempX;
				}
				if(selection.end.y < selection.start.y) {
					int tempY = selection.start.y;
					selection.start.y = selection.end.y;
					selection.end.y = tempY;
				}
//
				riv::Filter *xFilter = new riv::RangeFilter("x",selection.start.x,selection.end.x - 1);
				//Be sure to invert the Y coordinates!
				riv::Filter *yFilter = new riv::RangeFilter("y", renderedImage->sizeY - selection.start.y,renderedImage->sizeY - selection.end.y - 1);
//				Filter *yFilter = new RangeFilter("y", selection.start.y,selection.end.y - 1);
                
//                Filter *xFilter = new RangeFilter("x",9,22);
//                Filter *yFilter = new RangeFilter("y",5,6);
		
				dataset->AddFilter("path",xFilter);
				dataset->AddFilter("path",yFilter);
                
//                dataset->Print();
			}
			else {
				clearSelection(); 
			}
			isDragging = false;
		}
		return true;
	}
	else {
		//Clear any possible selection
//		clearSelection();
		return false;
	}
}

bool RIVImageView::HandleMouseMotion(int x, int y) {
    y = height - y;
	if(isDragging) {
		Point pixel = viewToPixelSpace(x,y);
		selection.end = pixel;
        glutPostRedisplay();
		return true;
	}
	else return false;
}

void RIVImageView::clearSelection() {
	//Set the selection to off
	selection.start.x = -1;
    selection.start.y = -1;
    selection.end.x = -1;
    selection.end.y = -1;
    
	//Clear any filters that may have been applied to the dataset
    if(dataset) {
        dataset->ClearFilter("x");
        dataset->ClearFilter("y");
    }
}

Point RIVImageView::viewToPixelSpace(int x, int y) {
	int pixelX = round((float)x / imageMagnificationX);
	int pixelY = round((float)y / imageMagnificationY);

	Point pixel;

	//How many pixels does the original image have?
	int nrOfXPixels = round(imageWidth / imageMagnificationX);
	int nrOfYPixels = round(imageHeight / imageMagnificationY);

	pixel.x = std::min(std::max(pixelX,0),nrOfXPixels);
	pixel.y = std::min(std::max(pixelY,0),nrOfYPixels);

	return pixel;
}