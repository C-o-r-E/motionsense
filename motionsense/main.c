//
//  main.c
//  motionsense
//
//  Created by Corey Clayton on 2012-11-05.
//  Copyright (c) 2012 Awake Coding. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <dispatch/dispatch.h>
#include <CoreGraphics/CoreGraphics.h>

//void (^a)(int) = ^(int x){printf("1");};

uint64_t last_time = 0;

void (^handleStream)(CGDisplayStreamFrameStatus, uint64_t, IOSurfaceRef, CGDisplayStreamUpdateRef) =  ^(CGDisplayStreamFrameStatus status,
                   uint64_t displayTime,
                   IOSurfaceRef frameSurface,
                   CGDisplayStreamUpdateRef updateRef)
{
    //printf("handleStream called!\n");
    
    if(displayTime - last_time < 500000000)
        return;
    
    last_time = displayTime;
    /*
     kCGDisplayStreamFrameStatusFrameComplete,
     kCGDisplayStreamFrameStatusFrameIdle,
     kCGDisplayStreamFrameStatusFrameBlank,
     kCGDisplayStreamFrameStatusStopped,
     */
    printf("\tstatus: ");
    switch(status)
    {
            case kCGDisplayStreamFrameStatusFrameComplete:
            printf("Complete\n");
            break;
            
            case kCGDisplayStreamFrameStatusFrameIdle:
            printf("Idle\n");
            break;
            
            case kCGDisplayStreamFrameStatusFrameBlank:
            printf("Blank\n");
            break;
            
            case kCGDisplayStreamFrameStatusStopped:
            printf("Stopped\n");
            break;
    }
    
    printf("\ttime: %lld\n", displayTime);
    
    const CGRect * rects;
    
    size_t num_rects;
    
    rects = CGDisplayStreamUpdateGetRects(updateRef, kCGDisplayStreamUpdateDirtyRects, &num_rects);
    
    printf("\trectangles: %zd\n", num_rects);
    
    CGRect uRect;
    
    uRect = *rects;
    for (size_t i = 0; i < num_rects; i++)
    {
        printf("\t\t(%f,%f),(%f,%f)\n\n",
               (rects+i)->origin.x,
               (rects+i)->origin.y,
               (rects+i)->origin.x + (rects+i)->size.width,
               (rects+i)->origin.y + (rects+i)->size.height);
        
        uRect = CGRectUnion(uRect, *(rects+i));
    }
    
    printf("\t\tUnion: (%f,%f),(%f,%f)\n\n",
           uRect.origin.x,
           uRect.origin.y,
           uRect.origin.x + uRect.size.width,
           uRect.origin.y + uRect.size.height);
    
};

int main(int argc, const char * argv[])
{
    dispatch_queue_t q;
    q = dispatch_queue_create("herp.derp.mcgerp", NULL);
    
    CGDisplayStreamRef stream;
    
    CGDirectDisplayID display_id;
    display_id = CGMainDisplayID();
    
    CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display_id);
    
    size_t pixelWidth = CGDisplayModeGetPixelWidth(mode);
    size_t pixelHeight = CGDisplayModeGetPixelHeight(mode);
    
    CGDisplayModeRelease(mode);
    
    //stream = CGDisplayStreamCreate(display_id, pixelWidth, pixelHeight, 'BGRA', NULL, handleStream);
    stream = CGDisplayStreamCreateWithDispatchQueue(display_id, pixelWidth, pixelHeight, 'BGRA', NULL, q, handleStream);
    
    CGDisplayStreamStart(stream);
    
    
    sleep(1);
    
    CGDisplayStreamStop(stream);
    
    
    printf("Done!\n");
    return 0;
}

