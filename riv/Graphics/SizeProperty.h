//
//  DimensionProperty.h
//  Afstuderen
//
//  Created by Gerard Simons on 14/06/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__DimensionProperty__
#define __Afstuderen__DimensionProperty__

#include "../Data/Table.h"
#include "Property.h"



//Determines the size or dimension of a given row for a given table. It may be based on a
class RIVSizeProperty  {
protected:
    float defaultSize;
    RIVSizeProperty(float defaultSize_) {
        defaultSize = defaultSize_;
    }
public:
    virtual float ComputeSize(RIVTableInterface* sourceTable,const size_t& row) = 0;
};


class RIVFixedSizeProperty : public RIVSizeProperty {
public:
    RIVFixedSizeProperty(float defaultSize) : RIVSizeProperty(defaultSize) {
        
    }
    float ComputeSize(RIVTableInterface* sourceTable, const size_t& row) {
        return defaultSize;
    }
};

template <typename T>
class RIVEvaluatedSizeProperty : public RIVSizeProperty, public RIVEvaluatedProperty<T>{
private:
    
public:
    float ComputeSize(RIVTableInterface* sourceTable, const size_t& row) {
        float size;
        if(RIVEvaluatedProperty<T>::Value(sourceTable, row, size)) {
            return size;
        }
        else return defaultSize;
    }
    
    RIVEvaluatedSizeProperty(RIVTableInterface* sourceTable, float defaultSize_) : RIVEvaluatedProperty<T>(sourceTable,defaultSize_), RIVSizeProperty(defaultSize_) {
        defaultSize = defaultSize_;
    }

};

#endif /* defined(__Afstuderen__DimensionProperty__) */
