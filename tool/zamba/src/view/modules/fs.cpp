/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-luaz implementation.

    DTV-luaz is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-luaz is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-luaz.

    DTV-luaz es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-luaz se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include <luaz/lua.h>
#include <zapper/zapper.h>
#include <util/log.h>
#include <util/fs.h>
#include <boost/filesystem.hpp>
#include <vector>
#include <string.h>
#if defined(_WIN32) && ! defined(__MINGW32__)
#define strcasecmp _strcmpi
#endif

#define LUA_FSLIBNAME "fs"

namespace bfs = boost::filesystem;

namespace fs {
namespace impl {

struct ExtMatcher {
	ExtMatcher( const std::string &ext ) : _ext(ext) {};
	virtual ~ExtMatcher() {};

	bool operator()( const bfs::path &file ) const {
		return (strcasecmp(file.extension().string().c_str(), _ext.c_str()) == 0);
	}

private:
	ExtMatcher &operator=( const ExtMatcher &/*extMatcher*/) {
		return *this;
	};
	const std::string &_ext;

};

struct FileNameMatcher {
	FileNameMatcher( const std::string &name ) : _name(name) {};
	virtual ~FileNameMatcher() {};

	bool operator()( const bfs::path &file ) const {
		return _name == file.stem().string();
	}

private:
	FileNameMatcher &operator=( const FileNameMatcher &/*fileMatcher*/) {
		return *this;
	};

	const std::string &_name;

};

struct AllMatcher {
	AllMatcher() {};
	virtual ~AllMatcher() {};

	bool operator()( const bfs::path & /*file*/ ) const {
		return true;
	}
};

template<typename T>
void scanFiles( const std::string &path, const T &match, int maxDepth, std::vector<std::string> &files) {
	if (bfs::exists( path )) {
		bfs::directory_iterator end_itr;
		for (bfs::directory_iterator itr( path ); itr != end_itr; ++itr ) {
			if (bfs::is_directory( itr->status() ) && 0 < maxDepth) {
				scanFiles( itr->path().string(), match, maxDepth - 1, files);
			} else if (bfs::is_regular_file( itr->status() )) {
				bfs::path file = itr->path();
				if (match(file)) {
					files.push_back( file.string() );
				}
			}
		}
	}
}

}	// End impl namespace

static int l_splitPath( lua_State *L ) {
	::bfs::path path(luaL_checkstring(L,1));

	lua_pushstring(L, path.parent_path().string().c_str() );
	lua_pushstring(L, path.stem().string().c_str() );
	lua_pushstring(L, path.extension().string().c_str() );

	return 3;
}

static int l_makePath( lua_State *L ) {
	::bfs::path path1(luaL_checkstring(L, 1));
	::bfs::path path2(luaL_checkstring(L, 2));

	lua_pushstring(L, ::util::fs::make( path1.string(), path2.string() ).c_str() );
	return 1;
}

static int l_pathIsAbsolute( lua_State *L ) {
	::bfs::path path(luaL_checkstring(L, 1));
	lua_pushboolean(L, path.is_absolute());
	return 1;
}

static int l_scanDirectoryByExtension( lua_State *L ) {
	std::string directory(luaL_checkstring(L, 1));
	std::string extension(luaL_checkstring(L, 2));
	int deep = 1;

	if (lua_gettop(L) == 3) {
		deep = luaL_checkint(L, 3);
	}

	std::vector<std::string> files;
	impl::scanFiles(directory, impl::ExtMatcher(extension), deep, files);

	return luaz::lua::pushList(L, files);
}

static int l_createDirectory( lua_State *L ) {
	::bfs::path directory(luaL_checkstring(L, 1));
	::bfs::create_directory(directory);
	return 0;
}

static int l_removeDirectory( lua_State *L ) {
	bool result = ::util::safeRemoveDirectory(luaL_checkstring(L, 1));
	lua_pushboolean(L, result? 1 : 0);
	return 1;
}

static const struct luaL_Reg fs_methods[] = {
	{ "split_path",                        l_splitPath                 },
	{ "make_path",                         l_makePath                  },
	{ "path_is_absolute",                  l_pathIsAbsolute            },
	{ "scan_directory_by_extension",       l_scanDirectoryByExtension  },
	{ "create_directory",                  l_createDirectory           },
	{ "remove_directory",                  l_removeDirectory           },
	{ NULL,                                NULL                        }
};

//	Module methods
void initialize( zapper::Zapper * /*mgr*/ ) {
}

void finalize( void ) {
	LINFO("fs::Module", "Stop");
}

void start( lua_State *L ) {
	LINFO("fs::Module", "Start");
	luaL_register( L, LUA_FSLIBNAME, fs_methods );
}

void attach( std::vector<std::string> & /*deps*/ ) {}
void stop() {}

}


