#import "MainWindowController.h"

@implementation MainWindowController
- (void)windowDidLoad {
	NSLog(@"MainWindowController.windowDidLoad");
	
	[self.window setAcceptsMouseMovedEvents:YES];
	
	mapViewController = [[MapViewController alloc] initWithNibName:@"MapView" bundle:nil];
	[mainView addSubview:mapViewController.view];
	mapView = [(MapView*)mapViewController.view retain];
}

- (void)dealloc {
	[mapViewController release];
	[mapView release];
	[super dealloc];
}

@synthesize mapView;
@synthesize mapViewController;

@end
