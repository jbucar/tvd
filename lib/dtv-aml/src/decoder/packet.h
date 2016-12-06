#pragma once

namespace aml {

class Packet {
public:
	Packet();
	~Packet();

	void copy( Packet *pkt );
	void copy( unsigned char *new_data, int new_size );
	void assign( const unsigned char *new_data, int new_size, int new_alloc_size, bool owner );

	bool alloc( int s );
	void clean();

	unsigned char *data;
	int size;

private:
	int _alloc_size;
	bool _owner;
};

}

