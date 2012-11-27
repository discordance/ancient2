#include "KeyModifier.h"

#import <AppKit/AppKit.h>
#include "testApp.h"

@interface AppleKeyModifiers : NSObject
{
    
}

- (id)setup;
- (BOOL)getAppleCommandModifier;

@end

@implementation AppleKeyModifiers;

- (id) setup{
    
    NSLog(@"KeyModifiers setup");
    self = [super init];
    
    
    [[NSNotificationCenter defaultCenter]
     addObserver:self
     selector:@selector(handleNotification:)
     name:nil
     object:nil];
    
    return self;
}

-(void)handleNotification:(NSNotification *)pNotification
{
    if([[NSApp currentEvent] type] == NSKeyDown) {
        //NSLog(@"message = %@",(NSString*)[pNotification object]);
        testApp *p = (testApp*) ofGetAppPtr();
        unichar key = [[[NSApp currentEvent] charactersIgnoringModifiers] characterAtIndex:0];
    }
}

// could impliment all of these on mac, but I guess we really just want the command key??
//enum {
//    NSAlphaShiftKeyMask         = 1 << 16,
//    NSShiftKeyMask              = 1 << 17,
//    NSControlKeyMask            = 1 << 18,
//    NSAlternateKeyMask          = 1 << 19,
//    NSCommandKeyMask            = 1 << 20,
//    NSNumericPadKeyMask         = 1 << 21,
//    NSHelpKeyMask               = 1 << 22,
//    NSFunctionKeyMask           = 1 << 23,
//#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
//    NSDeviceIndependentModifierFlagsMask    = 0xffff0000UL
//#endif
//};

- (BOOL)getAppleCommandModifier{
    return (([[NSApp currentEvent] modifierFlags] & NSCommandKeyMask) == NSCommandKeyMask);
}

- (BOOL)getAppleShiftModifier{
    return (([[NSApp currentEvent] modifierFlags] & NSShiftKeyMask) == NSShiftKeyMask);
}

- (int)getKey{
    
    if([[NSApp currentEvent] type] == NSKeyDown) {
        return (int) [[NSApp currentEvent] keyCode];
    } else return -1;
}

@end

//--------------------------------------------------------------
KeyModifier::KeyModifier(){
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    modifiers = [[AppleKeyModifiers alloc] setup];
    [pool release];
}

//--------------------------------------------------------------
KeyModifier::~KeyModifier(){
    
}

bool KeyModifier::getAppleCommandModifier(){
    return (bool)[modifiers getAppleCommandModifier];
}

bool KeyModifier::getAppleShiftModifier(){
    return (bool)[modifiers getAppleShiftModifier];
}

int KeyModifier::getKey(){
    return (int) [modifiers getKey];
}