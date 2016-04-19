#import <Cocoa/Cocoa.h>
#import "NSLvnsImage.h"

int system_type = 0;

int main(int argc, const char *argv[])
{
	[NSLvnsImage Init];

	if (argc > 1) {
		system_type = atoi(argv[1]);
	}
	if (system_type > 2)
		system_type = 0;

    return NSApplicationMain(argc, argv);
}
