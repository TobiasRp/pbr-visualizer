//
//  ParallelCoordsAxisGroup.h
//  Afstuderen
//
//  Created by Gerard Simons on 15/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__ParallelCoordsAxisGroup__
#define __Afstuderen__ParallelCoordsAxisGroup__

#include "ParallelCoordsAxis.h"
#include "../Data/Table.h"

#include <tuple>
#include <vector>

template<typename... Ts>
class ParallelCoordsAxisGroup {
private:
public:
	
	~ParallelCoordsAxisGroup() {
		tuple_for_each(axes, [&](auto tAxes) {
//			deletePointerVector(tAxes);
		});
	}
	
    RIVTable<Ts...>* table;
	std::tuple<std::vector<ParallelCoordsAxis<Ts>*>...> axes;
	
	template<typename U>
	std::vector<ParallelCoordsAxis<U>*>* GetAxes() {
		return &std::get<std::vector<ParallelCoordsAxis<U>*>>(axes);
	}
	
	template<typename U>
	void AddAxis(ParallelCoordsAxis<U*>& newAxis) {
		auto axes = GetAxes<U>();
		axes->push_back(newAxis);
	}
	
	template<typename U>
	ParallelCoordsAxis<U>* CreateAxis(RIVRecord<U>* record, int x, int y, int axisHeight, int min, int max, const std::string& name, int divisionCount) {
		std::vector<ParallelCoordsAxis<U>*>* tAxes = GetAxes<U>();
		tAxes->push_back(new ParallelCoordsAxis<U>(x,y,axisHeight,min,max,name,record,divisionCount));
		return tAxes->at(tAxes->size() - 1);
	}
    
//    ParallelCoordsAxisGroup* connectedGroup = 0;
//    ParallelCoordsAxis* connectorAxis;
	
};

#endif /* defined(__Afstuderen__ParallelCoordsAxisGroup__) */