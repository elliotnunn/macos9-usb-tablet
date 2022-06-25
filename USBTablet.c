#include <Events.h>
#include <MixedMode.h>
#include <USB.h>
#include <Types.h>

#include "nkprintf.h"

// Change the device class to FF (vendor specific), to make it unappealing
// to drivers that might replace this one later in boot.
// The USB Expert respects the Name Registry as a source of truth.
// According to the published USB search algorithm, this driver should "win"
// over a class driver, but because we pretend to be a ROM resource,
// we lose and get replaced by a HID driver from the USB Device Extension.
// This is done in 68k assembly due to our critical space shortage.
char makeVendorSpecific68[] = {
	0x00,0x00,0x00,0x00, // deviceRef: dc.l 0

	0x4e,0x56,0xff,0xe8, // link.w  a6,#-$18

	0x2f,0x0f,           // move.l  sp,-(sp)            ; cookie at -$18(a6)
	0x70,0x07,           // moveq   #7,d0               ; RegistryEntryIterateCreate
	0xab,0xe9,           // dc.w    $abe9

	0x48,0x78,0x00,0x04, // pea     4                   ; propertySize
	0x48,0x7a,0xff,0xec, // pea     deviceRef           ; propertyValue
	0x48,0x7a,0x00,0x3a, // pea     #'deviceRef'        ; propertyName
	0x48,0x6e,0xff,0xfc, // pea     -$04(a6)            ; done
	0x48,0x6e,0xff,0xec, // pea     -$14(a6)            ; foundEntry
	0x48,0x78,0x00,0x05, // pea     $05                 ; relationship
	0x48,0x6e,0xff,0xe8, // pea     -$18(a6)            ; cookie
	0x70,0x0b,           // moveq   #$b,d0              ; RegistryEntrySearch
	0xab,0xe9,           // dc.w    $abe9

	0x48,0x78,0x00,0x04, // pea     4                   ; propertySize
	0x48,0x78,0x0a,0x06, // pea     $a06                ; propertyValue = minusOne
	0x48,0x7a,0x00,0x0e, // pea     #'DeviceClass'      ; propertyName
	0x48,0x6e,0xff,0xec, // pea     -$14(a6)            ; entryID
	0x70,0x18,           // moveq   #$18,d0             ; RegistryPropertySet
	0xab,0xe9,           // dc.w    $abe9

	0x4e,0x5e,           // unlk    a6
	0x4e,0x75,           // rts

	'D','e','v','i','c','e','C','l','a','s','s',0,
	'd','e','v','i','c','e','R','e','f',0,
};

// Reduce the cross-TOC glue code in the finished binary
pascal OSErr (*MyPPostEvent)(EventKind, UInt32, EvQElPtr *) = &PPostEvent;
#define PPostEvent MyPPostEvent

long (*MyCallUniversalProc)(UniversalProcPtr, ProcInfoType, ...) = CallUniversalProc;
#define CallUniversalProc MyCallUniversalProc

enum {
	stateInit,
	stateFindInterface,
	stateOpenDevice,
	stateNewInterfaceRef,
	stateConfigureInterface,
	stateFindNextPipe,
	stateIntRead,
};

OSStatus returnNoErr(void);
OSStatus initialize(USBDeviceRef device, USBDeviceDescriptorPtr pDesc, UInt32 busPowerAvailable);
void stateMachine(USBPB *pb);

int state = stateInit;

UInt8 interfacenum;

USBPB pb;

USBPB pbClean = {
	NULL,           // qLink
	0,              // qType
	sizeof(USBPB),  // pbLength ?ensures we are wired in memory
	0x0100,         // pbVersion
	0, 0,           // reserved
	0,              // usbStatus
	stateMachine,   // usbCompletion
};

unsigned char report[16];
unsigned char prevButton;

USBDriverDescription TheUSBDriverDescription = {
	// Signature info
	kTheUSBDriverDescriptionSignature,
	kInitialUSBDriverDescriptor,

	// Device Info
	0x0627,                                 // vendor = QEMU
	0x0001,                                 // product = QEMU mouse/tablet
	0,                                      // version of product = not device specific
	0,                                      // protocol = not device specific

	// Interface Info
	0,                                      // Configuration Value
	0,                                      // Interface Number
	3,                                      // Interface Class: HID
	kUSBNoInterfaceSubClass,                // Interface SubClass: 0 or 1 is fine
	kUSBNoInterfaceProtocol,                // Interface Protocol: neither kbd nor mouse

	// Driver Info
	"\pQEMUTab",                            // Driver name for Name Registry
	0,                                      // Device Class  (from USBDeviceDefines.h)
	0,                                      // Device Subclass
	0x01, 0x00, finalStage, 0x00,           // version of driver

	// Driver Loading Info
	0,                                      // Flags
};

USBClassDriverPluginDispatchTable TheClassDriverPluginDispatchTable = {
	kClassDriverPluginVersion,              // Version of this structure
	(void *)returnNoErr,                    // Hardware Validation Procedure
	initialize,                             // Initialization Procedure
	NULL,                                   // Interface Initialization Procedure
	(void *)returnNoErr,                    // Finalization Procedure
	NULL,                                   // Driver Notification Procedure
};

OSStatus returnNoErr(void) {
	return noErr;
}

OSStatus initialize(USBDeviceRef device, USBDeviceDescriptorPtr pDesc, UInt32 busPowerAvailable) {
	(void)pDesc;
	(void)busPowerAvailable;

	// Hack to prevent replacement when we masquerade as a ROM resource
	*(USBDeviceRef *)makeVendorSpecific68 = device;
	CallUniversalProc((void *)(makeVendorSpecific68 + 4), kPascalStackBased);

	pbClean.usbReference = device;
	stateMachine(&pb);
	return noErr;
}

static void wipePB(void) {
	pb = pbClean;
}

void stateMachine(USBPB *_pb) {
	// Calling through a function pointer saves a LOT of cross-TOC glue
	OSStatus (*funcPtr)(USBPB *);

	(void)_pb; // use global pb instead

	//nkprintf("stateMachine state=%d usbStatus=%d\n", state, pb.usbStatus);

	switch (state) {
	case stateInit:
		wipePB();

		// Trust pb.usbReference from initialize()
		pb.usbClassType = kUSBHIDInterfaceClass;
		// pb.usbSubclass = 0; // already zero in our blank pb
		// pb.usbProtocol = 0;

		state = stateFindInterface;
		funcPtr = USBFindNextInterface;
		break;

	case stateFindInterface:
		interfacenum = pb.usb.cntl.WIndex;

		// Comment these out because USBOpenDevice can trust PB from USBFindNextInterface()
		// confignum = pb.usb.cntl.WValue;
		// wipePB();
		// pb.usb.cntl.WValue = confignum;

		state = stateOpenDevice;
		funcPtr = USBOpenDevice; // Synonym for USBSetConfiguration
		break;

	case stateOpenDevice:
		wipePB();
		pb.usb.cntl.WIndex = interfacenum;

		state = stateNewInterfaceRef;
		funcPtr = USBNewInterfaceRef;
		break;

	case stateNewInterfaceRef:
		pbClean.usbReference = pb.usbReference; // save interface ref

		wipePB();

		state = stateConfigureInterface;
		funcPtr = USBConfigureInterface;
		break;

	case stateConfigureInterface:
		wipePB();
		pb.usbFlags = kUSBIn;
		pb.usbClassType = kUSBInterrupt;

		state = stateFindNextPipe;
		funcPtr = USBFindNextPipe;
		break;

	case stateFindNextPipe:
		pbClean.usbReference = pb.usbReference; // save pipe ref
		// fall through

	case stateIntRead:
		//nkprintf("%08x %08x\n", *(long *)report, *(long *)(report + 4));

		if (pb.usbStatus == noErr) {
			unsigned long point;

			// byte 5 is a signed scroll amount

			point = ((((unsigned long)report[4] << 8) | report[3]) * *(unsigned short *)0xc22 >> 15 << 16) |
				((((unsigned long)report[2] << 8) | report[1]) * *(unsigned short *)0xc20 >> 15);

			*(unsigned long *)0x828 = point; // MTemp
			*(unsigned long *)0x82c = point; // RawMouse

			*(char *)0x8ce = *(char *)0x8cf; // CrsrNew = CrsrCouple

			if (!report[0] != !prevButton) {
				EvQEl *event;

				*(unsigned char *)0x172 = report[0] ? 0 : 0x80;
				PPostEvent(report[0] ? mouseDown : mouseUp, 0, &event);

				// Right-click becomes control-click
				if (report[0] & 2) {
					event->evtQModifiers |= 0x1000;
				}

				prevButton = report[0];
			}

			// Call JCrsrTask to redraw the cursor immediately.
			// Feels much more responsive than waiting for another interrupt.
			// Could a race condition garble the cursor? Haven't seen it happen.
			// if (*(char *)(0x174 + 7) & 1) // Uncomment to switch on shift key
			CallUniversalProc(*(void **)0x8ee, kPascalStackBased);
		}

		wipePB();
		pb.usbBuffer = report;
		pb.usbReqCount = sizeof report;
		pb.usb.cntl.WIndex = interfacenum;

		state = stateIntRead;
		funcPtr = USBIntRead;
		break;
	}

	funcPtr(&pb);
}
