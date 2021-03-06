//
//  TableInterface.h
//  embree
//
//  Created by Gerard Simons on 10/12/14.
//
//

#ifndef embree_TableInterface_h
#define embree_TableInterface_h

class RIVReference;

class RIVTableInterface {
public:
	const std::string name;
    
    std::vector<RIVReference*> references;

	//	size_t ID = ++IDCOUNTER;
	
	virtual bool HasRecord(const std::string& name) const = 0;
	virtual void FilterRow(size_t row) = 0;
    virtual void FilterRowFromReference(size_t row,RIVReference* ref) = 0;
    virtual void FilterRowFromReference(size_t row,std::set<RIVReference*>& ref) = 0;
	virtual size_t NumberOfRows() const = 0;
    virtual size_t NumberOfUnfilteredRows() const = 0;
	virtual void ClearFilteredRows() = 0;
    virtual void ClearFilteredRows(RIVTableInterface* sourceTable) = 0; //What table tells it to clear its rows, important to avoid cyclic behaviour in updating references
    
    virtual RIVReference* GetReferenceTo(const std::string& tableName) = 0;
protected:
	RIVTableInterface(const std::string& name) : name(name) {
		
	}
};

#endif
