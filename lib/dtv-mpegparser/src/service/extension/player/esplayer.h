#pragma once

#include "types.h"
#include <vector>

namespace tuner {
namespace player {

class StreamInfo;
class Player;
class Delegate;

namespace es {

class Player {
public:
	Player( Delegate *delegate, pes::type::type type, int first=0 );
	Player( pes::type::type type, int first=0 );
	virtual ~Player();

	//	Initialization
	bool initialize( player::Player *player );
	void finalize();

	//	Start
	StreamInfo *canPlay( const ElementaryInfo &info, ID tag ) const;
	void addStream( StreamInfo *sInfo, const ElementaryInfo &info, ID tag );
	void addStream( const Stream &sInfo );
	void start();
	void stop( bool clean );
	bool canNext() const;
	int next();
	int current() const;

	//	Getters
	int count() const;
	const Stream *get( int id=-1 ) const;
	const Stream *findStream( fncFindStream fnc );
	pes::type::type esType() const;

protected:
	bool start( const Stream *sInfo );
	void stop( const Stream *sInfo );
	void start( int id );

	const Stream *getStream( int id ) const;
	int first() const;
	int next( int pos, int factor, int size ) const;

	Delegate *dlg() const;

private:
	Delegate *_delegate;
	pes::type::type _pesType;
	int _first;
	int _current;
	std::vector<Stream> _currentStreams;
};

}
}
}

