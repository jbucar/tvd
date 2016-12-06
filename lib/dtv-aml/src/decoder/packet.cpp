#include "packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace aml {

Packet::Packet()
{
	data = NULL;
	_alloc_size = 0;
	size = 0;
	_owner = true;
}

Packet::~Packet() {
	clean();
}

void Packet::clean() {
	if (_owner) {
		free(data);
	}
	data = NULL;
	_alloc_size = 0;
	size = 0;
	_owner = true;
}

bool Packet::alloc( int s ) {
	if (_alloc_size < s || !_owner) {
		clean();

		data = (unsigned char *)malloc( s );
		if (!data) {
			return false;
		}

		size = 0;
		_alloc_size = s;
		_owner = true;
	}
	return true;
}

void Packet::copy( Packet *pkt ) {
	copy( pkt->data, pkt->size );
}

void Packet::copy( unsigned char *new_data, int new_size ) {
	if (alloc( new_size )) {
		memcpy( data, new_data, new_size );
		size = new_size;
	}
}

void Packet::assign( const unsigned char *new_data, int new_size, int new__alloc_size, bool owner ) {
	clean();

	data = (unsigned char *)new_data;
	size = new_size;
	_alloc_size = new__alloc_size;
	_owner = owner;
}

}
