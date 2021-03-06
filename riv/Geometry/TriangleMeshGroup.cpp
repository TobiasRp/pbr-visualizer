//
//  TriangleMeshGroup.cpp
//  embree
//
//  Created by Gerard Simons on 24/11/14.
//
//

#include "TriangleMeshGroup.h"

void TriangleMeshGroup::init() {

    xBounds[0] = std::numeric_limits<float>::max();
    xBounds[1] = -std::numeric_limits<float>::max();
    yBounds[0] = std::numeric_limits<float>::max();
    yBounds[1] = -std::numeric_limits<float>::max();
    zBounds[0] = std::numeric_limits<float>::max();
    zBounds[1] = -std::numeric_limits<float>::max();
    
	float cX = 0;
	float cY = 0;
	float cZ = 0;
	size_t nrVertices = 0;
	//Determine the center
	for(TriangleMeshFull *mesh : triangleMeshes) {
		vector_t<Vec3fa>& position = mesh->position;
		for(size_t i = 0 ; i < position.size() ; ++i) {
			Vec3fa& vertex = position[i];
            
            xBounds[0] = std::min(xBounds[0],vertex[0]);
            xBounds[1] = std::max(xBounds[1],vertex[0]);
            yBounds[0] = std::min(yBounds[0],vertex[1]);
            yBounds[1] = std::max(yBounds[1],vertex[1]);
            zBounds[0] = std::min(zBounds[0],vertex[2]);
            zBounds[1] = std::max(zBounds[1],vertex[2]);
            
			cX += vertex[0];
			cY += vertex[1];
			cZ += vertex[2];
		}
		nrVertices += position.size();
	}
	cX /= nrVertices;
	cY /= nrVertices;
	cZ /= nrVertices;
    
	center[0] = cX;
	center[1] = cY;
	center[2] = cZ;
    
	float maxDistance = 0;
    
	//Determine the scale we need to scale it to unit
	//Find the vertex with the most distance to the center
	for(TriangleMeshFull *mesh : triangleMeshes) {
		vector_t<Vec3fa>& position = mesh->position;
		for(size_t i = 0 ; i < position.size() ; ++i) {
			Vec3fa vertex = position[i];
            
			float dX = vertex[0] - cX;
			float dY = vertex[1] - cY;
			float dZ = vertex[2] - cZ;
			float distance = sqrt(dX * dX + dY * dY + dZ * dZ);
			if(distance > maxDistance) {
				maxDistance = distance;
			}
		}
	}
	scale = 1 / maxDistance;
	printf("meshmodel center = [%f,%f,%f]\n",center[0],center[1],center[2]);
	printf("meshmodel scale = %f\n",scale);
}

bool TriangleMeshGroup::Intersect(const Ray& ray, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance) const {
    return Intersect(ray, resultIndex, Phit, shortestDistance, ray.org);
}

bool TriangleMeshGroup::Intersect(const Ray& ray, ushort& resultIndex, Vec3fa& Phit, float& shortestDistance, const Vec3fa& distanceCompare) const {
    bool intersects = false;
    bool reverse = true;

    if (reverse) {
        shortestDistance = -std::numeric_limits<float>::max();
    }
    else shortestDistance = std::numeric_limits<float>::max();

    Vec3fa bestPhit = Phit;
    
    //DEBUGGING
    glColor3f(1, 1, 0);
    
    for(size_t i = 0 ; i < triangleMeshes.size() ; ++i) {
        //			float d;
        //		printf("i = %d\n",i);
        TriangleMeshFull* mesh = triangleMeshes[i];
        vector_t<TriangleMeshFull::Triangle> triangles = mesh->triangles;
        vector_t<Vec3fa>& position = mesh->position;
        for(size_t j = 0 ; j < triangles.size() ; ++j) {
            Vec3fa v0 = position[triangles[j].v0];
            Vec3fa v1 = position[triangles[j].v1];
            Vec3fa v2 = position[triangles[j].v2];
            
            Vec3fa edge1 = v1 - v0;
            Vec3fa edge2 = v2 - v0;
            
            Vec3fa N = cross(edge1, edge2);
            //		std::cout << "Normal = " << N << std::endl;
            
            float nDotRay = dot(N, ray.dir);
            //		printf("nDotRay = %f\n",nDotRay);
            if (dot(N, ray.dir) == 0) {
                //			printf("Test 1 failed\n");
                continue; // ray parallel to triangle
            }
            
            float d = dot(N, v0);
            d = -d;
            float t = -(dot(N, ray.org) + d) / nDotRay;
            //		printf("d = %f\n",d);
            //		printf("t = %f\n",t);
            
            // inside-out test
            Phit = ray.org + t * ray.dir;
            //			Phit[2] = -Phit[2];
            
            
            // inside-out test edge0
            Vec3fa v0p = Phit - v0;
            float v = dot(N, cross(edge1, v0p));
            if (v < 0) {
                //			printf("Test 2 failed\n");
                continue; // P outside triangle
            }
            
            // inside-out test edge1
            Vec3fa v1p = Phit - v1;
            Vec3fa v1v2 = v2 - v1;
            float w = dot(N, cross(v1v2, v1p));
            if (w < 0) {
                //			printf("Test 3 failed\n");
                continue;
            }
            
            // inside-out test edge2
            Vec3fa v2p = Phit - v2;
            Vec3fa v2v0 = v0 - v2;
            float u = dot(N, cross(v2v0, v2p));
            if (u < 0) {
                continue; // P outside triangle
            }

            float distance = embree::length((distanceCompare - Phit));
//            printf("Intersects with %zu\n",i);
//            printf("distance = %f\n",distance);
            
            glPushMatrix();
            glTranslatef(Phit.x, Phit.y, Phit.z);
            glutSolidSphere(1, 10, 10);
            glPopMatrix();
            
//            if(distance < shortestDistance ) {
            if(reverse) {
                if(distance > shortestDistance ) {
                    
//                    printf("New shortest distance!\n");
                    resultIndex = i;
                    shortestDistance = distance;
                    bestPhit = Phit;
                    intersects = true;
            }
            }
            else {
                if(distance < shortestDistance ) {
                    
//                    printf("New shortest distance!\n");
                    resultIndex = i;
                    shortestDistance = distance;
                    bestPhit = Phit;
                    intersects = true;
                }
            }
        }
    }
    Phit = bestPhit;
    return intersects;
}
void TriangleMeshGroup::Translate(const Vec3fa& translation) {
    for(size_t i = 0 ; i < triangleMeshes.size() ; ++i) {
        TriangleMeshFull* mesh = triangleMeshes[i];
//        vector_t<TriangleMeshFull::Triangle>& triangles = mesh->triangles;
        vector_t<Vec3fa>& position = mesh->position;
        for(size_t j = 0 ; j < position.size() ; ++j) {
            position[j] = position[j] + translation;
        }
    }
    center = center + translation;
}
void TriangleMeshGroup::Translate(float x, float y, float z) {
    Translate(Vec3fa(x,y,z));
}