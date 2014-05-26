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

class RIV3DView : public RIVDataView {
private:
    std::vector<float> modelData;
    Point3D eye;
    bool isDirty = true;
    Point3D ScreenToWorldCoordinates(int mouseX, int mouseY, float zPlane);
public:
    RIV3DView(int,int,int,int,int,int);
     void ComputeLayout();
	 void Draw();
	 bool HandleMouse(int button, int state, int x, int y);
	 bool HandleMouseMotion(int x, int y);
     void OnFilterChange();
    
    void SetModelData(std::vector<float>);
    void MoveCamera(float,float,float);
    
    //The models to draw

    //The camera
    
    //Anything else?
};

#endif /* defined(__Afstuderen___DView__) */
