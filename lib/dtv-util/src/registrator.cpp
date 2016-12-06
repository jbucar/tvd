/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-util implementation.

    DTV-util is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-util is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-util.

    DTV-util es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-util se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "registrator.h"
#include "cfg/cfg.h"
#include "assert.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <stdio.h>

namespace util {
namespace reg {
namespace impl {

typedef bool (*Comparator)( const Registrator *first, const Registrator *second);

static inline bool compareByPriority( const Registrator *first, const Registrator *second) {
	return first->priority() < second->priority();
}

static inline bool compareByInversePriority( const Registrator *first, const Registrator *second) {
	return second->priority() < first->priority();
}

static std::list<Registrator *> &initCallbacks() {
	static std::list<Registrator *> initCallbacks;
	return initCallbacks;
}

static std::list<Registrator *> &finCallbacks() {
	static std::list<Registrator *> finCallbacks;
	return finCallbacks;
}

static inline void call( std::list<Registrator *> &calls, Comparator fnc ) {
	if (!calls.empty()) {
		calls.sort(fnc);
		BOOST_FOREACH( Registrator *reg, calls ) {
			try {
				( *reg )( );
			} catch (...) {
				fprintf( stderr, "Error calling to registrator: name=%s\n", reg->name().c_str() );
				throw;
			}
		}
	}
}

}       //	namespace impl


void addinitCallbacks(Registrator *aReg) {
	DTV_ASSERT(!util::cfg::get().hasChildren());
	impl::initCallbacks().push_front( aReg );
}

void addfinCallbacks(Registrator *aReg) {
	impl::finCallbacks().push_front( aReg );

}

void init() {
	impl::call( impl::initCallbacks(), impl::compareByPriority );
}

void fin() {
	impl::call( impl::finCallbacks(), impl::compareByInversePriority );
}

Registrator::Registrator(  const std::string &name, impl::addCallback aCall )
	: _name( name ) {
	boost::replace_all(_name,"_",".");
	( *aCall )( this );
}

Registrator::~Registrator() {
}

const std::string &Registrator::name() const {
	return _name;
}

void Registrator::operator()() {
}

int Registrator::priority() const {
	return count(_name.begin(), _name.end(), '.');
}

}       // end reg namespace
}       // end util namespace
