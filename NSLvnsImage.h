#import <Cocoa/Cocoa.h>
#include "Lvns.h"

@interface NSLvnsImage : NSObject
{
    unsigned char pixels[256][3];
    int pixel_num;
	int width;
	int height;
	int bpr;
	unsigned char *data;
}

+ (void) Init;

- (id) init : (NSSize) size;
- (void) draw : (NSRect) bounds;

- (void) clear : (int) color;
- (void) clearArea : (int)x : (int)y : (int)w : (int)h: (int) color;
- (void) putPattern24 : (int) x : (int) y : (int) color : (u_char *) data;
- (void) putPattern24_2 : (int) x : (int) y : (int) color : (u_char *) data;
- (void) setPalette : (int) index r:(int)r  g:(int)g  b:(int) b;
- (void) setPal : (LvnsImage *) image;
- (void) setPalMulti : (LvnsImage *) image : (int) par16;
- (void) setPalScreen : (LvnsImage *) image : (int) par16;
- (void) copy : (LvnsImage *) image : (int) x : (int) y;
- (void) copyArea : (LvnsImage *) image : (int) x : (int) y 
                  : (int) w : (int) h : (int) x2 : (int) y2;
- (void) copyMaskUnit : (LvnsImage *) image : (int) x : (int) y : (int) x2 : (int) y2 : (int) mask;
- (void) copySquareMaskUnit : (LvnsImage *) image : (int) x : (int) y : (int) x2 : (int) y2 : (int) mask;

@end

