//
//  Histogram.h
//  embree
//
//  Created by Gerard Simons on 01/12/14.
//
//

#ifndef __embree__Histogram__
#define __embree__Histogram__

#include <map>
#include <vector>
#include <exception>
#include <iostream>
#include <string>
#include <cmath>

#include "TupleIterator.h"
#include "SampleSet.h"

//histogram implementation. Counts the values in the given vector and divides by the number of samples to normalize
template<typename T>
class Histogram {
private:
	//Use ints because we also want to have negative counts when using difference of two histograms
	std::map<unsigned int,int> hist;
	size_t nrElements;
	
	T lowerBound;
	T upperBound;
	
	unsigned int bins = 0;
	float binWidth = 0;
	
	//Count the values and divide by the total number to get a normalized histogram
	void count(const std::vector<T>& ts) {
//		if(ts.size() == 0) {
//			throw std::runtime_error("Empty data vector");
//		}
		for(const T& t : ts) {
			Add(t);
		}
		nrElements = ts.size();
	}
	//Create the histogram from the given values and also use it to determine the natural bounds of the data
	void createFromData(const std::vector<T>& ts) {
//		if(ts.size() == 0) {
//			throw std::runtime_error("Empty data vector");
//		}
		
		count(ts);
		
		lowerBound = ts[0];
		upperBound = ts[0];
		
		for(auto it : hist) {
			lowerBound = std::min(lowerBound,it.first);
			upperBound = std::max(upperBound,it.first);
		}
		
		nrElements = ts.size();
	}
	float determineBinWidth() {
		if(upperBound == lowerBound) {
			return 0.5F;
		}
		else return (float)(upperBound - lowerBound) / bins;
	}
public:
	std::string name;
	int BinValue(int key) {
		return hist[key];
	}
	~Histogram() {
		
	}
	Histogram() {
		nrElements = 0;
	}
	Histogram(const std::string& name, ushort lowerBound, ushort upperBound) : name(name), lowerBound(lowerBound), upperBound(upperBound) {
		nrElements = 0;
		bins = upperBound - lowerBound;
		binWidth = 1;
	}
	Histogram(const std::string& name, T lowerBound, T upperBound, unsigned int bins) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()) {
		nrElements = 0;
	}
	Histogram(const std::string& name, T lowerBound, T upperBound, unsigned int bins, size_t nrElements) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()), nrElements(nrElements) {
		
	}
	Histogram(const std::string& name, const std::vector<T>& ts, T lowerBound, T upperBound, unsigned int bins) : name(name), lowerBound(lowerBound), upperBound(upperBound), bins(bins), binWidth(determineBinWidth()) {
		count(ts);
	}
	Histogram(const std::string& name, const std::vector<T>& ts, unsigned int bins)  : name(name) {
		createFromData(ts,bins);
	}
	Histogram(const std::string& name, const SampleSet<T>& sampleset, unsigned int bins) : name(name){
		createFromData(sampleset.GetSamples(),bins);
	}
	Histogram(const std::string& name, const std::vector<T>& ts) : name(name){
		createFromData(ts);
	}
	Histogram(const std::string& name, const SampleSet<T>& sampleset) : name(name){
		createFromData(sampleset.GetSamples());
	}
	T LowerBound() {
		return lowerBound;
	}
	T UpperBound() {
		return upperBound;
	}
	void Set(unsigned int bin, int count) {
		hist[bin] = count;
	}
	void Add(const T& value) {
		
		T delta = upperBound - lowerBound;
		float interpolated = (float)(value - lowerBound) / (delta);
		unsigned int bin = floor(interpolated * bins);
		
		++hist[bin];
		++nrElements;
	}
	unsigned int NumberOfBins() {
		return bins;
	}
	Histogram operator-(Histogram& right) {
		if(right.bins != bins) {
			throw std::runtime_error("The histograms should have the same number of bins");
		}
		
		
		printf("left - right = \n\n");
		printf("left = \n");
		Print();
		printf("right = \n");
		right.Print();
		printf("\n");
		
		T lowerBound = std::min(LowerBound(),right.LowerBound());
		T upperBound = std::max(UpperBound(),right.UpperBound());
		Histogram result(name,lowerBound,upperBound,bins,nrElements + right.NumberOfElements());
		
		//Use normalized values!
		for(int i = 0 ; i < bins ; i++) {
			int thisValue = BinValue(i);
			int rightValue = right.BinValue(i);
			int diff = (thisValue - rightValue);
			result.Set(i,diff);

		}
		printf("Result = \n");
		result.Print();
		return result;
	}
	
	float DistanceTo(Histogram* right) {
		if(right->bins != bins) {
			throw std::runtime_error("The histograms should have the same number of bins");
		}
		
		T lowerBound = std::min(LowerBound(),right->LowerBound());
		T upperBound = std::max(UpperBound(),right->UpperBound());
		float totalDiff = 0;
		
		//Use normalized values!
		for(T i = lowerBound ; i <= upperBound ; i++) {
			float thisValue = NormalizedValue(i);
			float rightValue = right->NormalizedValue(i);
			float diff = std::abs((double)(thisValue - rightValue));
			totalDiff += diff;
		}
		return totalDiff;
	}
	float NormalizedValue(int i) {
		if(nrElements)
			return BinValue(i) / (float) nrElements;
		else
			return 0;
	}
	void Clear() {
		hist.clear();
		nrElements = 0;
	}
	void Print() {
		int maxBarSize = 48;
		printf("Histogram %s :\n",name.c_str());
		for(int i = 0 ; i < bins ; ++i) {
			printf("%.1f - %.1f : ",(float)i * binWidth,float(i+1)*binWidth);
			float normalValue = NormalizedValue(i);
			int barSize = normalValue * maxBarSize;
			int tabs = maxBarSize / 4 - barSize / 4  + 1; //4 is the tabwidth in spaces
			for(int i = 0 ; i < barSize ; i++) {
				std::cout << "#";
			}
			for(int i = 0 ; i < tabs ; ++i) {
				std::cout << "\t";
			}
			int value = BinValue(i);
			std::cout << value << std::endl;
		}
		std::cout << std::endl;
	}
	size_t NumberOfElements() {
		return nrElements;
	}
};

template <typename... Ts>
class HistogramSet {
private:
	std::tuple<std::vector<Histogram<Ts>>...> histograms;
	std::tuple<std::map<std::string,Histogram<Ts>*>...> histogramRegisters; //Maps a name to a histogram pointer
	
	//When set to dynamic the set will create new histograms for values added to not yet exsistent histograms
//	bool dynamic = true;
	
//	std::tuple<std::vector<Ts>...> values;
//	std::tuple<std::vector<Histogram<float>>,std::vector<Histogram<ushort>>> histograms;
//	std::tuple<int> ints;
	
public:
	~HistogramSet() {
//		tuple_for_each(histograms, [&](auto tHistograms) {
//			deletePointerVector(tHistograms);
//		});
	}
	HistogramSet() {
		
	}
	/** Copy Assignment Operator */
	HistogramSet& operator= (const HistogramSet& other)
	{
		histograms = other.histograms;
		return *this;
	}
	/** Copy Constructor */
	HistogramSet (const HistogramSet& other)
	{
		histograms = other.histograms;
	}
	
//	template<typename T>
//	std::vector<T> GetValue() {
//		return std::get<std::vector<T>>(values);
//	}
//
//	HistogramSet() {
//		
//	}
//	HistogramSet(const HistogramSet& other) {
//		histograms = other.histograms;
//		histogramRegisters = other.histogramRegisters;
//	}
//	HistogramSet& operator=(const HistogramSet& other) {
//		histograms = other.histograms;
//		histogramRegisters = other.histogramRegisters;
//		return *this;
//	}
	
	template<typename T>
	std::vector<Histogram<T>>* GetHistograms() {
		return &std::get<std::vector<Histogram<T>>>(histograms);
	}
	
	template<typename T>
	std::map<std::string,Histogram<T>*>& GetHistogramRegister() {
		return std::get<std::map<std::string,Histogram<T>*>>(histogramRegisters);
	}
	
	template<typename T>
	Histogram<T>* GetHistogram(const std::string& name) {
		std::map<std::string,Histogram<T>*>& histogramRegister = GetHistogramRegister<T>();
		auto hist = histogramRegister[name];
		if(hist) {
			return hist;
		}
		else {
			void* returnPointer = NULL;
			auto tHistograms = GetHistograms<T>();
			
			for(size_t i = 0 ; i < tHistograms->size() ; ++i) {
				auto hist = tHistograms->at(i);
					if(hist.name == name) {
						histogramRegister[name] = &tHistograms->at(i);
						return &tHistograms->at(i);
					}
				}
//				return returnPointer;
		}
	}
	
	
	template<typename T>
	void AddToHistogram(const std::string name,const T& value) {
//		printf("name = %s\n",name.c_str());
		Histogram<T>* histogram = NULL;
		histogram = GetHistogram<T>(name);
		//TODO: Create histogram when set to dynamic and histogram was not found
		histogram->Add(value);
	}
	
	template<typename T>
	void AddHistogram(const Histogram<T>& hist) {
		std::vector<Histogram<T>>* histograms = GetHistograms<T>();
		
		histograms->push_back(hist);
		auto newHistogram = &histograms->at(histograms->size() - 1);
		std::get<std::map<std::string,Histogram<T>*>>(histogramRegisters)[hist.name] = newHistogram;
	}
	
	//This assumes the right hand operand histogram set is a superset of this histogram set, throws a runtime_error if a histogram in this set could not be found.
	HistogramSet operator-(HistogramSet<Ts...>& other) {
//		return compare<Ts...>();
		
		float total = 0;
		HistogramSet result;
		
		tuple_for_each(histograms, [&](auto tHistograms) {
			for(auto histogram : tHistograms) {
				

				auto otherHistogram = other.GetHistogram<decltype(histogram->LowerBound())>(histogram->name);
				if(!otherHistogram) {
					throw std::runtime_error("Histogram " + histogram->name + " not found in right hand operand");
				}
				
				auto diff = (*histogram) - (*otherHistogram);
				result.AddHistogram(diff);
			}
		});
		
		return total;
	}
	
	float DistanceTo(HistogramSet<Ts...>& other) {
		float total = 0;
		
		tuple_for_each(histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
			
				auto otherHistogram = other.GetHistogram<decltype(histogram.LowerBound())>(histogram.name);
				if(!otherHistogram) {
					throw std::runtime_error("Histogram " + histogram.name + " not found in right hand operand");
				}
				
				total += histogram.DistanceTo(otherHistogram);
			}
		});
		
		return total;
	}
	
	void Print() {
		printf("Histogram set : \n");
		tuple_for_each(histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
				histogram.Print();
			}
		});
	}
		
	void Join(const HistogramSet& other) {
		tuple_for_each(other.histograms, [&](auto tHistograms) {
			for(auto& histogram : tHistograms) {
				AddHistogram(histogram);
			}
		});
	}
//
//	template<typename T>
//	std::vector<T>* GetHistograms() {
//		return &std::get<T>(histograms);
//	}
};

#endif /* defined(__embree__Histogram__) */