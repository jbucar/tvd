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

#pragma once

#include "mcr.h"
#include <algorithm>
#include <typeinfo>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/type_with_alignment.hpp>
#include <boost/static_assert.hpp>
#include <boost/throw_exception.hpp>
#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable: 4100 )
#endif

#define ANY_CONDITION(T,R)  sizeof(T) <= sizeof(R)

namespace util {

	class bad_any_cast : public std::bad_cast {
	public:
		bad_any_cast(const std::type_info& src, const std::type_info& dest)
			: from(src.name()), to(dest.name())
		{}
		virtual const char* what() const throw() {
			return "util::bad_any_cast: failed conversion using util::any_cast";
		}
		const char* from;
		const char* to;
	};

	namespace any { namespace detail {

		//	Create
		template <typename T,typename R>
		typename boost::enable_if_c<ANY_CONDITION(T,R)>::type
			create( void **obj, const T &x ) {
				new(obj) T(x);
		}

		template <typename T,typename R>
		typename boost::disable_if_c<ANY_CONDITION(T,R)>::type
			create( void **obj, const T &x ) {
				*obj = new T(x);
		}

		//	Get
		template <typename T,typename R>
		typename boost::enable_if_c<ANY_CONDITION(T,R), const T &>::type
		get( void* const* obj ) {
			return *reinterpret_cast<T const*>(obj);
		}

		template <typename T,typename R>
		typename boost::disable_if_c<ANY_CONDITION(T,R), const T &>::type
		get( void* const* obj ) {
			return *reinterpret_cast<T const*>(*obj);
		}

		//	Type
		template <typename T,typename R>
		const std::type_info &type( void ) {
			return typeid(T);
		}

		//	Delete
		template <typename T,typename R>
		typename boost::enable_if_c<ANY_CONDITION(T,R)>::type
			del( void **obj ) {
			reinterpret_cast<T *>(obj)->~T();
		}

		template <typename T,typename R>
		typename boost::disable_if_c<ANY_CONDITION(T,R)>::type
			del( void **obj ) {
			delete (*reinterpret_cast<T**>(obj));
		}

		//	Clone
		template <typename T,typename R>
		typename boost::enable_if_c<ANY_CONDITION(T,R)>::type
			clone( void* const* src, void **dest ) {
			new(dest) T(*reinterpret_cast<T const*>(src));
		}

		template <typename T,typename R>
		typename boost::disable_if_c<ANY_CONDITION(T,R)>::type
			clone( void* const* src, void **dest ) {
			*dest = new T(**reinterpret_cast<T* const*>(src));
		}

		//	Move
		template <typename T,typename R>
		typename boost::enable_if_c<ANY_CONDITION(T,R)>::type
			move( void* const* src, void **dest ) {
			reinterpret_cast<T*>(dest)->~T();
			new(dest) T(*reinterpret_cast<T const*>(src));
		}

		template <typename T,typename R>
		typename boost::disable_if_c<ANY_CONDITION(T,R)>::type
			move( void* const* src, void **dest ) {
			(*reinterpret_cast<T**>(dest))->~T();
			new(*dest) T(**reinterpret_cast<T* const*>(src));
		}

		//	function pointer table
		struct fxn_ptr_table {
			const std::type_info& (*type)();
			void (*static_delete)( void ** );
			void (*clone)( void* const*, void ** );
			void (*move)( void* const*, void ** );
		};

		//	getTable (T=basic sizeof,R type sizeof)
		template <typename T,typename R>
		struct getTable {
			static fxn_ptr_table* get() {
				static fxn_ptr_table static_table = {
					&type<T,R>,		//	type
					&del<T,R>,		//	delete
					&clone<T,R>,	//	clone
					&move<T,R>,		//	move
				};
				return &static_table;
			}
		};

	}}

	template<typename S>
	class BasicAny {
	public:
		// structors
		template <typename T>
		BasicAny( const T& x ) {
			_table = any::detail::getTable<T,S>::get();
			any::detail::create<T,S>( &_holder.pointer, x );
		}

		BasicAny( void ) {
			_table  = NULL;
		}

		BasicAny( const BasicAny &x ) {
			_table  = NULL;
			assign(x);
		}

		~BasicAny() {
			reset();
		}

		//	queries
		bool empty() const {
			return _table == NULL;
		}

		const std::type_info &type() const {
			return empty() ? typeid(void *) : _table->type();
		}

		template<typename ValueType>
		bool operator==( const ValueType &val ) const {
			return _table == any::detail::getTable<ValueType,S>::get() && get<ValueType>() == val;
		}

		template<typename ResultType>
		const ResultType &get() const {
			if (empty() || type() != typeid(ResultType)) {
				throw bad_any_cast( type(), typeid(ResultType) );
			}
			return any::detail::get<ResultType,S>(&_holder.pointer);
		}

		//	operations
		BasicAny &operator=( const BasicAny &other ) {
			assign( other );
			return *this;
		}

		template<typename ValueType>
		BasicAny &operator=( const ValueType &val ) {
			assign(val);
			return *this;
		}

	protected:
		template <std::size_t OrigSize, std::size_t RoundTo>
		struct ct_rounded_size {
			enum { value = ((OrigSize-1)/RoundTo+1)*RoundTo };
		};
		static const std::size_t Alignment = boost::alignment_of<boost::detail::max_align>::value;
		static const std::size_t AlignmentBytes = ct_rounded_size<sizeof(S), Alignment>::value;
		// //	Due to embedded bits in size, Alignment must be at least 4
		// BOOST_STATIC_ASSERT((Alignment >= 4));
		// //	Due to size optimizations, Alignment must have at least pointer alignment
		// BOOST_STATIC_ASSERT((Alignment >= boost::alignment_of<void *>::value));

		//	Type holder
		union ObjectHolder {
			char buffer[AlignmentBytes];	// used fields
			void *pointer;
		};

		//	Assignment
		template <typename T>
		BasicAny &assign( const T &x ) {
			//	are we copying between the same type?
			any::detail::fxn_ptr_table *x_table = any::detail::getTable<T,S>::get();
			if (_table != x_table) {
				reset();
				//	update table pointer
				_table = x_table;
			}
			else {
				_table->static_delete(&_holder.pointer);
			}
			any::detail::create<T,S>( &_holder.pointer, x );
			return *this;
		}

		BasicAny &assign( const BasicAny &x ) {
			//	are we copying between the same type?
			if (!empty() && !x.empty() && _table == x._table) {
				//	if so, we can avoid reallocation
				_table->move(&x._holder.pointer, &_holder.pointer);
			}
			else if (!x.empty()) {
				reset();
				x._table->clone(&x._holder.pointer, &_holder.pointer);
				_table = x._table;
			}
			else {
				reset();
			}
			return *this;
		}

		void reset() {
			if (!empty()) {
				_table->static_delete(&_holder.pointer);
				_table  = NULL;
			}
		}

	private:
		// fields
		any::detail::fxn_ptr_table *_table;
		ObjectHolder _holder;

		bool operator==( const BasicAny<S> & /*val*/ ) const {
			return false;
		}
	};

	//	Define Any as BasicAny of sizeof(void *)
	typedef BasicAny<std::string> Any;

}

#ifdef _WIN32
#pragma warning( pop )
#endif
