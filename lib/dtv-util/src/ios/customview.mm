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

#include "customview.h"
#include "ios.h"
#include "../log.h"
#include "../keydefs.h"

@implementation CustomView {
	UIImage *_image;
	CGDataProviderRef _provider;
	CGColorSpaceRef _colorSpace;
	int _width;
	int _height;
	int _stride;
}

- (id) initWithFrame: (CGRect) frame {
	self = [super initWithFrame:frame];
	if (self) {
		_image = nil;
	}
	return self;
}

- (void) dealloc {
	CGDataProviderRelease(_provider);
	CGColorSpaceRelease(_colorSpace);
	[super dealloc];
}

- (void) drawRect: (CGRect) rect {
	[super drawRect: rect];
	if (_image) {
		CGPoint pos = {0, 0};
		[_image drawAtPoint: pos blendMode: kCGBlendModeNormal alpha: 1.0];
	}
	if (![self isFirstResponder]) {
		[self becomeFirstResponder];
	}
}

- (void) initLayer: (util::DWORD*) pixels width: (int) w height: (int) h {
	_provider = CGDataProviderCreateWithData( NULL, pixels, w *h * 4, NULL );
	_colorSpace = CGColorSpaceCreateDeviceRGB();
	_width = w;
	_height = h;
	_stride = w*4;
}

- (void) render {
	CGImageRef imageRef = CGImageCreate( _width, _height, 8, 32, _stride, _colorSpace, kCGBitmapByteOrderDefault, _provider, NULL, NO, kCGRenderingIntentDefault );
	_image = [UIImage imageWithCGImage: imageRef];
	[self performSelectorOnMainThread:@selector(setNeedsDisplay) withObject: nil waitUntilDone: NO];
	CGImageRelease(imageRef);
}

- (void) insertText: (NSString*) text {
	const char *c = [text UTF8String];
	util::key::type key = util::key::null;
	int value = *c;
	if (value>=48 && value<=57) {
		key = static_cast<util::key::type>( util::key::number_0 + (value - 48) );
	}
	else if (value>=65 && value<=90) {
		key = static_cast<util::key::type>( util::key::capital_a + (value - 65) );
	}
	else if (value>=97 && value<=122) {
		key = static_cast<util::key::type>( util::key::small_a + (value - 97) );
	}
	else {
		switch (value) {
			case 10: key = util::key::enter; break;
			case 27: key = util::key::escape; break;
		}
	}

	if (key!=util::key::null) {
		util::ios::dispatchKey( key, false );
		util::ios::dispatchKey( key, true );
	} else {
		LWARN("ios::CustomView", "key %d not mapped", value);
	}
}

- (BOOL) hasText {
	return NO;
}

- (void) deleteBackward {
}

- (BOOL) canBecomeFirstResponder {
    return YES;
}

@end
