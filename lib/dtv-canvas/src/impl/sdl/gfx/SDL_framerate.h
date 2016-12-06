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


/*

SDL_framerate: framerate manager

LGPL (c) A. Schiffler

*/

#ifndef _SDL_framerate_h
#define _SDL_framerate_h

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

	/* --- */

#include <SDL/SDL.h>

	/* --------- Definitions */

/*!
\cond PRIVATE
\brief Highest possible rate supported by framerate controller in Hz (1/s).
*/
#define FPS_UPPER_LIMIT		200

/*!
\brief Lowest possible rate supported by framerate controller in Hz (1/s).
*/
#define FPS_LOWER_LIMIT		1

/*!
\brief Default rate of framerate controller in Hz (1/s).
*/
#define FPS_DEFAULT		30

/*! 
\brief Structure holding the state and timing information of the framerate controller. 
*/
	typedef struct {
		Uint32 framecount;
		float rateticks;
		Uint32 lastticks;
		Uint32 rate;
	} FPSmanager;

	/* ---- Function Prototypes */

#if defined(WIN32) || defined(WIN64)
#  if defined(DLL_EXPORT) && !defined(LIBSDL_GFX_DLL_IMPORT)
#    define SDL_FRAMERATE_SCOPE __declspec(dllexport)
#  else
#    ifdef LIBSDL_GFX_DLL_IMPORT
#      define SDL_FRAMERATE_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef SDL_FRAMERATE_SCOPE
#  define SDL_FRAMERATE_SCOPE extern
#endif

	/* Functions return 0 or value for sucess and -1 for error */

	SDL_FRAMERATE_SCOPE void SDL_initFramerate(FPSmanager * manager);
	SDL_FRAMERATE_SCOPE int SDL_setFramerate(FPSmanager * manager, int rate);
	SDL_FRAMERATE_SCOPE int SDL_getFramerate(FPSmanager * manager);
	SDL_FRAMERATE_SCOPE int SDL_getFramecount(FPSmanager * manager);
	SDL_FRAMERATE_SCOPE void SDL_framerateDelay(FPSmanager * manager);

	/* --- */

	/* Ends C function definitions when using C++ */

/*!
\endcond
*/
#ifdef __cplusplus
}
#endif

#endif				/* _SDL_framerate_h */
