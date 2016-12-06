/*******************************************************************************

  Copyright (C) 2010, 2013 LIFIA - Facultad de Informatica - Univ. Nacional de La Plata

********************************************************************************

  This file is part of DTV-canvas implementation.

    DTV-canvas is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 2 of the License.

    DTV-canvas is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License along with
  this program. If not, see <http://www.gnu.org/licenses/>.

********************************************************************************

  Este archivo es parte de la implementación de DTV-canvas.

    DTV-canvas es Software Libre: Ud. puede redistribuirlo y/o modificarlo
  bajo los términos de la Licencia Pública General Reducida GNU como es publicada por la
  Free Software Foundation, según la versión 2 de la licencia.

    DTV-canvas se distribuye esperando que resulte de utilidad, pero SIN NINGUNA
  GARANTÍA; ni siquiera la garantía implícita de COMERCIALIZACIÓN o ADECUACIÓN
  PARA ALGÚN PROPÓSITO PARTICULAR. Para más detalles, revise la Licencia Pública
  General Reducida GNU.

    Ud. debería haber recibido una copia de la Licencia Pública General Reducida GNU
  junto a este programa. Si no, puede verla en <http://www.gnu.org/licenses/>.

*******************************************************************************/

#include "canvas.h"
#include "error.h"
#include "surface.h"
#include "font.h"
#include <util/mcr.h>
#include <util/assert.h>
#include <util/log.h>
#include "generated/config.h"
#if USE_GLES2
#	ifdef __APPLE__
#		include <ES2/gl.h>
#	else
#		include <GLES2/gl2.h>
#	endif
#else
#	include <GL/glew.h>
#endif
#include <cstdlib>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace canvas {
namespace gl {

namespace impl {

static GLuint drawingShaders = 0;
static GLuint blitingShaders = 0;
static GLuint textShaders    = 0;


static const char drawVShader[] =
	"#version 100\n"
	"attribute vec4 aPos;\n"
	"uniform mat4 uMVP;\n"
	"void main() {\n"
	"  gl_Position = uMVP * aPos;\n"
	"  gl_PointSize = 1.0;\n"
	"}";

static const char drawFShader[] =
	"#version 100\n"
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"
	"uniform vec4 uColor;\n"
	"void main() {\n"
	"  gl_FragColor = uColor;\n"
	"}";

static const char loadTexVShader[] =
	"#version 100\n"
	"attribute vec4 aPos;\n"
	"attribute vec2 aTexCoord;\n"
	"varying vec2 vTexCoord;\n"
	"uniform mat4 uMVP;\n"
	"void main() {\n"
	"  gl_Position = uMVP * aPos;\n"
	"  vTexCoord = aTexCoord;\n"
	"}";

static const char blendFShader[] =
	"#version 100\n"
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"
	"varying vec2 vTexCoord;\n"
	"uniform sampler2D uTex;\n"
	"uniform float uAlpha;\n"
	"void main() {\n"
	"  gl_FragColor = uAlpha * texture2D( uTex, vTexCoord );\n"
	"}";

static const char textVShader[] =
	"#version 100\n"
	"uniform mat4 uMVP;\n"
	"attribute vec4 aPos;\n"
	"attribute vec2 aTexCoord;\n"
	"varying vec2 vTexCoord;\n"
	"void main() {\n"
	"  vTexCoord = aTexCoord;\n"
	"  gl_Position = uMVP * aPos;\n"
	"}";

static const char textFShader[] =
	"#version 100\n"
	"#ifdef GL_ES\n"
	"precision mediump float;\n"
	"#endif\n"
	"uniform sampler2D uTex;\n"
	"uniform vec4 uColor;\n"
	"varying vec2 vTexCoord;\n"
	"void main() {\n"
	"  gl_FragColor = uColor * texture2D(uTex, vTexCoord).a;\n"
	"}";


static GLuint loadShader( GLenum type, const char *shaderSrc ) {
	GLuint shader = glCreateShader(type);
	if (shader != 0) {
		glShaderSource( shader, 1, &shaderSrc, NULL);
		glCompileShader( shader );

		GLint compiled;
		glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
		if (!compiled) {
			GLint infoLen=0;
			glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen );
			if (infoLen > 1) {
				char* infoLog = (char*) malloc( sizeof(char) * infoLen );
				glGetShaderInfoLog( shader, infoLen, NULL, infoLog );
				LERROR("gl::Canvas", "Fail to compile %s shader. Reason: %s%s", type==GL_VERTEX_SHADER ? "vertex" : "fragment", infoLog, shaderSrc);
				free(infoLog);
			}
			else {
				LERROR( "gl::Canvas", "Fail to compile with no message" );
			}
			glDeleteShader( shader );
			shader = 0;
		}
	}
	return shader;
}

static GLuint loadProgram( const char *vertShader, const char *fragShader ) {
	GLuint vertexShader = loadShader( GL_VERTEX_SHADER, vertShader );
	if (vertexShader == 0) {
		LERROR( "gl::Canvas", "Cannot load GL_VERTEX_SHADER" );
		return 0;
	}

	GLuint fragmentShader = loadShader( GL_FRAGMENT_SHADER, fragShader );
	if (fragmentShader == 0) {
		LERROR( "gl::Canvas", "Cannot load GL_FRAGMENT_SHADER" );
		glDeleteShader( vertexShader );
		return 0;
	}

	GLuint programObject = glCreateProgram();
	if (programObject!=0) {
		glAttachShader( programObject, vertexShader );
		glAttachShader( programObject, fragmentShader );
		glLinkProgram( programObject );

		GLint linked;
		glGetProgramiv( programObject, GL_LINK_STATUS, &linked );
		if (!linked) {
			GLint infoLen=0;
			glGetProgramiv( programObject, GL_INFO_LOG_LENGTH, &infoLen );
			if (infoLen > 1) {
				char* infoLog = (char*) malloc( sizeof(char) * infoLen );
				glGetProgramInfoLog( programObject, infoLen, NULL, infoLog );
				LERROR("gl::Canvas", "Fail to link program: %s", infoLog );
				free( infoLog );
			}
			else {
				LERROR( "gl::Canvas", "Fail to link with no message" );
			}
			glDeleteProgram( programObject );
			programObject = 0;
		}
	}

	glDeleteShader( vertexShader );
	glDeleteShader( fragmentShader );

	return programObject;
}

}	//	namespace impl

bool initGL() {
	LINFO( "gl::Canvas", "Initialize GL" );

#if !USE_GLES2
	GLenum err = glewInit();
	if (err!=GLEW_OK) {
		LERROR("gl::Canvas", "Fail to init glew: %s", glewGetErrorString(err));
		return false;
	}
#endif

	LDEBUG( "gl::Canvas", "\tGL_VENDOR=%s", glGetString(GL_VENDOR) );
	LDEBUG( "gl::Canvas", "\tGL_RENDERER=%s", glGetString(GL_RENDERER) );
	LDEBUG( "gl::Canvas", "\tGL_VERSION=%s", glGetString(GL_VERSION) );

#if !USE_GLES2
	//	Check if shading language is supported (HACK!)
	const char *shVer = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	if (!shVer) {
		LERROR("gl::Canvas", "GL version does not support shading language 100");
		return false;
	}
#endif

	glEnable( GL_BLEND );
	CHECK_GL_ERROR_ASSERT( "glEnable(GL_BLEND)");

	glEnable( GL_SCISSOR_TEST );
	CHECK_GL_ERROR_ASSERT( "glEnable(GL_SCISSOR_TEST)");

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	CHECK_GL_ERROR_ASSERT( "glPixelStorei(GL_UNPACK_ALIGNMENT)");

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	CHECK_GL_ERROR_ASSERT( "glPixelStorei(GL_PACK_ALIGNMENT)");

	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	CHECK_GL_ERROR_ASSERT( "glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)");

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	CHECK_GL_ERROR_ASSERT( "glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)");

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	CHECK_GL_ERROR_ASSERT( "glClearColor(0.0f, 0.0f, 0.0f, 0.0f)");

	impl::drawingShaders = impl::loadProgram( impl::drawVShader, impl::drawFShader );
	if (!impl::drawingShaders) {
		LERROR("gl::Canvas", "Fail to load drawing shaders program");
		return false;
	}

	impl::blitingShaders = impl::loadProgram( impl::loadTexVShader, impl::blendFShader );
	if (!impl::blitingShaders) {
		LERROR("gl::Canvas", "Fail to load blitting shaders program");
		return false;
	}

	impl::textShaders = impl::loadProgram( impl::textVShader, impl::textFShader );
	if (!impl::textShaders) {
		LERROR("gl::Canvas", "Fail to load text shaders program");
		return false;
	}

	LDEBUG( "gl::Canvas", "GL initialized successfuly. GL info:");
	LDEBUG( "gl::Canvas", "\tGL_SHADING_LANGUAGE_VERSION=%s", glGetString(GL_SHADING_LANGUAGE_VERSION) );
	LDEBUG( "gl::Canvas", "\tShader programs:");
	LDEBUG( "gl::Canvas", "\t\tDrawing shader : %d", impl::drawingShaders );
	LDEBUG( "gl::Canvas", "\t\tBliting shader : %d", impl::blitingShaders );
	LDEBUG( "gl::Canvas", "\t\tText shader    : %d", impl::textShaders );

	return true;
}

Canvas::Canvas()
{
	impl::drawingShaders = 0;
	impl::blitingShaders = 0;
	impl::textShaders = 0;
}

Canvas::~Canvas()
{
}

bool Canvas::init() {
	if (FT_Init_FreeType(&_freeType)) {
		LERROR("gl::Canvas", "FontLoader fail to initialize FreeType library");
		return false;
	}
	return true;
}

void Canvas::fin() {
	FT_Done_FreeType( _freeType );

	glDeleteProgram( impl::drawingShaders );
	impl::drawingShaders = 0;
	CHECK_GL_ERROR_LOG( "glDeleteProgram(impl::drawingShaders)" );

	glDeleteProgram( impl::blitingShaders );
	impl::blitingShaders = 0;
	CHECK_GL_ERROR_LOG( "glDeleteProgram(impl::blitingShaders)" );

	glDeleteProgram( impl::textShaders );
	impl::textShaders = 0;
	CHECK_GL_ERROR_LOG( "glDeleteProgram(impl::textShaders)" );
}

std::string Canvas::name() {
	return "gl";
}

canvas::Surface *Canvas::createSurfaceImpl( ImageData *img ) {
	return new Surface( this, img );
}

canvas::Surface *Canvas::createSurfaceImpl( const Rect &rect ) {
	return new Surface( this, rect );
}

canvas::Surface *Canvas::createSurfaceImpl( const std::string &file ) {
	return new Surface( this, file );
}

GLuint Canvas::drawingShaders() const {
	DTV_ASSERT( impl::drawingShaders != 0 );
	return impl::drawingShaders;
}

GLuint Canvas::blitingShaders() const {
	DTV_ASSERT( impl::blitingShaders != 0 );
	return impl::blitingShaders;
}

GLuint Canvas::textShaders() const {
	DTV_ASSERT( impl::textShaders != 0 );
	return impl::textShaders;
}

canvas::Font *Canvas::createFont( const std::string &filename, size_t size ) {
	return new Font( filename, size, &_freeType );
}

}
}
