#include <Events.h>
#include <MixedMode.h>
#include <USB.h>
#include <Types.h>

#include "nkprintf.h"

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

	pbClean.usbReference = device;
	stateMachine(&pb);
	return noErr;
}

static void wipePB(void) {
	pb = pbClean;
}

void stateMachine(USBPB *_pb) {
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
		USBFindNextInterface(&pb);
		break;

	case stateFindInterface:
		interfacenum = pb.usb.cntl.WIndex;

		// Comment these out because USBOpenDevice can trust PB from USBFindNextInterface()
		// confignum = pb.usb.cntl.WValue;
		// wipePB();
		// pb.usb.cntl.WValue = confignum;

		state = stateOpenDevice;
		USBOpenDevice(&pb); // Synonym for USBSetConfiguration
		break;

	case stateOpenDevice:
		wipePB();
		pb.usb.cntl.WIndex = interfacenum;

		state = stateNewInterfaceRef;
		USBNewInterfaceRef(&pb);
		break;

	case stateNewInterfaceRef:
		pbClean.usbReference = pb.usbReference; // save interface ref

		wipePB();

		state = stateConfigureInterface;
		USBConfigureInterface(&pb);
		break;

	case stateConfigureInterface:
		wipePB();
		pb.usbFlags = kUSBIn;
		pb.usbClassType = kUSBInterrupt;

		state = stateFindNextPipe;
		USBFindNextPipe(&pb);
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
		USBIntRead(&pb);
		break;
	}
}
