//
//  helper.h
//  Afstuderen
//
//  Created by Gerard Simons on 16/05/14.
//  Copyright (c) 2014 Gerard Simons. All rights reserved.
//

#ifndef __Afstuderen__helper__
#define __Afstuderen__helper__

#include <iostream>
#include <stdio.h>
#include <map>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cmath>

#if __APPLE_
_#include <GLUT/GLUT.h>
#elif __linux
#include <GL/glut.h>
#endif

void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );

//template <typename T, size_t N>
//class Vector {
//    T values[N];
//};

void println(const std::string& text);

template <typename C> struct get_template_type;

template <template <typename > class C, typename T>
struct get_template_type<C<T>>
{
	using type = T;
};

namespace riv {
	template <typename T>
	class Sampler {
	protected:
		T lowerBound;
		T upperBound;
		T range;
	public:
		Sampler(T lowerBound_, T upperBound_) {
			if(upperBound_ <= lowerBound_) {
				throw "Invalid sampling boundaries\n";
			}
			lowerBound = lowerBound_;
			upperBound = upperBound_;
			range = upperBound - lowerBound;
		}
		
		virtual T RequestSample() {
			T range = upperBound - lowerBound;
			return (T)(rand() % range);
		}
	};
}

template<typename T>
size_t memorySize(const std::vector<T>& vs) {
	return sizeof(vs) + vs.capacity() * sizeof(T);
}

template <typename T>
void reportVectorStatistics(const std::string& name, const std::vector<T> vs) {
	size_t byteSize = memorySize(vs);
	printf("Vector %s has %zu elements and uses %zu bytes (%.3f MB) of memory.\n",name.c_str(),vs.size(),byteSize,byteSize / 1000000.F);
}

template<class T>
inline void deletePointerVector(std::vector<T*> v)
{
    for (typename std::vector<T*>::iterator i = v.begin(); i != v.end(); ++i)
    {
        delete *i;
    }
	
}

void printDelimiterLine(char c = '*',int width = 50);

template<class T>
void printVar(const char* name, T var) {
    std::cout << name << " = " << var << std::endl;
}

void printHeader(const std::string& text,unsigned int width = 50);

template <typename T>
class NonReplacementSampler {
    std::vector<T> samplePool;
    std::vector<T>* origin;
    int requestCount = 0;
public:
    ~NonReplacementSampler() {
        samplePool.clear();
    }
    NonReplacementSampler() {
        //Do nothing
    }
    NonReplacementSampler(std::vector<T>* origin_) {
        origin = origin_;
        Reset();
    }
    NonReplacementSampler(T lowerbound, T upperbound) {
        if(upperbound < lowerbound) {
            throw "Invalid bounds.";
        }
        origin = new std::vector<T>;
        for(size_t i = lowerbound ; i < upperbound ; ++i) {
            origin->push_back(i);
        }
        Reset();
    }
    void Reset() {
        samplePool = (*origin);
    }
    T RequestSample() {
        ++requestCount;
        //        printf("Request #%d\n",requestCount);
        if(!samplePool.empty()) {
            size_t index = (T)(rand() % samplePool.size());
            T value = samplePool[index];
            samplePool.erase(samplePool.begin() + index);
            return value;
        }
        else {
            throw "Out of samples!";
        }
    }
};

unsigned long long
choose(unsigned long long n, unsigned long long k);

std::vector<std::vector<size_t> > generateCombinations(unsigned long long n, unsigned long long r, size_t requested);
std::vector<std::vector<size_t> > generateCombinations(std::vector<size_t> pool, unsigned long long r, size_t requested);



template <typename T>
size_t find(std::vector<T> haystack, T needle) {
    int i = 0 ;
    for(T value : haystack) {
        if(value == needle) {
            return i;
        }
        i++;
    }
    return -1;
}

template<typename T,typename U>
void printMap(std::map<T,U> map) {
    for(typename std::map<T, U>::const_iterator it = map.begin();
        it != map.end(); ++it)
    {
        std::cout << it->first << " : " << it->second << "\n";
    }
}

template<typename T>
void printMatrix(size_t rows, size_t columns, T* matrix) {
    for(size_t i = 0 ; i < rows ; i++) {
        std::cout << "[";
        for(size_t j = 0 ; j < columns ; j++) {
            std::cout << matrix[i+j];
            if(j != columns - 1) {
                std::cout << " ";
            }
        }
        std::cout << "]\n";
    }
}
bool almost_equal(double val1,double val2,double epsilon);
//T has to be a basic number type (float,double,int, ... ) //TODO: How to enforce this
template<typename T>
T maxInArray(T* values, size_t N) {
	if(N > 0) {
		T max = values[0];
		for(size_t i = 1 ; i < N ; ++i) {
			if(values[i] > max) {
				max = values[i];
			}
		}
		return max;
	}
	else throw "N must be > 0.";
}

template<typename T>
void printArray(T* array, size_t size) {
    std::cout << "[";
    for(size_t i = 0 ; i < size ; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << "]\n";
}

template <typename T>
bool arrayContains(T* array, size_t size, T value) {
    for(size_t i = 0 ; i < size ; i++) {
        if(array[i] == value) return true;
    }
    return false;
}
template <typename T>
void printVector(const std::vector<T>& values,std::string name = "") {
    
    std::cout << "vector " + name + " = [ ";
    for(size_t i = 0 ; i < values.size() ; ++i) {
        T value = values[i];
        std::cout << value << " ";
    }
    std::cout << "]\n";
}
template <typename T>
void print2DVector(const std::vector<std::vector<T>>& values2D,std::string name = "") {
    
    std::cout << "2D vector " + name + " = [\n";
    for(const std::vector<T>& values : values2D) {
        for(size_t i = 0 ; i < values.size() ; ++i) {
            T value = values[i];
            std::cout << value << " ";
        }
        printf("\n");
    }
    printf("]\n");
}
template <typename T>
bool vectorContains(const std::vector<T> v, T element) {
	return (std::find(v.begin(), v.end(), element) != v.end());
}

void copy_buffer();
void copy_buffer_back_to_front();
unsigned long factorization(int N,unsigned long maxF);
template <typename T>
T* randInRange(T lower, T upper, size_t N) {
    //Sanity checks
    if(N > (upper - lower)) {
        throw "Numbers requested exceeds random range";
    }
    if(upper <= lower) {
        throw "Invalid range";
    }
    
    std::vector<int> pool;
    //Fill pool
    for(int i = lower ; i < upper ; ++i) {
        pool.push_back(i);
    }
    T* numbersDrawn = new T[N];
    //Draw N numbers from pool
    for(int j = 0 ; j < N ; ++j) {
        size_t index = (size_t)(rand() / (float)RAND_MAX * pool.size());
        numbersDrawn[j] = pool[index];
        pool.erase(pool.begin() + index);
    }
    pool.clear();
    return numbersDrawn;
}
int** combinationsInRange(int lower, int upper, unsigned int N);
int* drawNumbersFromPool(std::vector<int> pool, const int N);
float* linearInterpolateColor(float,const float[3], const float[3]);
inline float linearInterpolate(float value, float start, float end) {
	return (value - start) / (end - start);
}
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
template<typename T = int>
std::vector<T> createRangeVector(const int lower, const int upper, const int step = 1) {
    

    int delta = (upper - lower);
    int trueStep = std::min(step,delta);
    std::vector<T> rangeVector;
    if(delta) {
        rangeVector = std::vector<T>((delta + 1)/(float)trueStep);
    }
    else return std::vector<T> {lower};
    
    if(step > 0 & upper >= lower) {
        int index = 0;
        for(int i = lower ; i <= upper ; i += step) {
            rangeVector[index] = i;
            
            index++;
        }
    }
    else throw std::runtime_error("Bad parameters");
    
    return rangeVector;
}

#endif /* defined(__Afstuderen__helper__) */
