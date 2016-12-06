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

#include "customappdelegate.h"
#include "customview.h"
#include "customviewcontroller.h"
#include "ios.h"
#include "../log.h"
#include "../main.h"

@implementation CustomAppDelegate

// Application state handlers
- (BOOL) application: (UIApplication*) application didFinishLaunchingWithOptions: (NSDictionary*) launchOptions {
	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] applicationFrame]];
	self.viewController = [[CustomViewController alloc] init];
    self.window.rootViewController = self.viewController;
	util::ios::setView( (CustomView*) self.viewController.view );
	[self.window makeKeyAndVisible];
	return YES;
}

// - (void) applicationWillResignActive: (UIApplication*) application {
// 	// Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
// 	// Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
// }

// - (void) applicationDidEnterBackground: (UIApplication*) application {
// 	// Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
// 	// If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
// }

// - (void) applicationWillEnterForeground: (UIApplication*) application {
// 	// Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
// }

- (void) applicationDidBecomeActive: (UIApplication*) application {
	// Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
	[self performSelectorInBackground: @selector(mainThread) withObject: nil];
}

- (void) applicationWillTerminate: (UIApplication*) application {
	// Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

// Touch handlers
// - (void) touchesBegan: (NSSet*) touches withEvent: (UIEvent*) event {
// 	// Tells the receiver when one or more fingers touch down in a view or window.
// 	CGPoint point = [[touches anyObject] locationInView: [[self.window subviews] lastObject]];
// 	LINFO("ios", "Event: touchesBegan pos=(%d,%d)", (int)point.x, (int)point.y);
// }

// - (void) touchesMoved: (NSSet*) touches withEvent: (UIEvent*) event {
// 	// Tells the receiver when one or more fingers associated with an event move within a view or window.
// 	LINFO("ios", "Event: touchesMoved:withEvent");
// }

// - (void) touchesEnded: (NSSet*) touches withEvent: (UIEvent*) event {
// 	// Tells the receiver when one or more fingers are raised from a view or window.
// 	CGPoint point = [[touches anyObject] locationInView: [[self.window subviews] lastObject]];
// 	LINFO("ios", "Event: touchesEnded pos=(%d,%d)", (int)point.x, (int)point.y);
// }

// - (void) touchesCancelled: (NSSet*) touches withEvent: (UIEvent*) event {
// 	// Sent to the receiver when a system event (such as a low-memory warning) cancels a touch event.
// 	LINFO("ios", "Event: touchesCancelled:withEvent");
// }

- (void) mainThread {
	int argc = 1;
	const char *argv[] = { "ios", NULL };
	util::main::run_tvd_tool( argc, (char**) argv );
}

@end
