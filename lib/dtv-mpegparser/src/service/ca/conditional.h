#pragma once

#include "types.h"
#include "../../demuxer/descriptors.h"
#include "../../demuxer/descriptors/ca.h"
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace tuner {

class Service;
class Cat;

namespace ca {

class Delegate;
class System;
class Decrypter;

class Conditional {
public:
	explicit Conditional( FilterDelegate *filter );
	virtual ~Conditional();

	//	Setup
	void addDelegate( Delegate *d );

	//	API
	void getAvailables( std::vector<ID> &systems ) const;
	void getAvailables( std::vector<ID> &systemscaSystems, ID srvID ) const;
	System *getSystem( ID caID ) const;

	Decrypter *allocDecrypter( ID srvID );
	void freeDecrypter( Decrypter * );

	//	Start
	void start();
	void stop();

	//	Events
	void onStart( Service *srv );
	void onStop( Service *srv );

	static Conditional *create( FilterDelegate *filter );

protected:
	//	Types
	typedef std::vector<Delegate *> Delegates;
	typedef std::vector<System *> Systems;
	typedef std::vector<Decrypter *> Decrypters;

	//	Systems
	void updateECM( ID srvID, const desc::Descriptors &descs, bool start );
	void onCat( const boost::shared_ptr<Cat> &cat );
	void onCatChanged();
	void clearSystems();

	//	Keys
	void onKeys( ID srvID, Keys *keys );

	//	Getters
	FilterDelegate *filter() const;

private:
	FilterDelegate *_filter;
	Decrypters _decrypters;
	Delegates _delegates;
	Systems _systems;
	bool _started;
};

}
}

