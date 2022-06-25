// Replace the ROM resource USBHIDMouseModule with a PEF appended to this one
// Participates in the early boot process

#include <CodeFragments.h>
#include <Memory.h>
#include <Resources.h>
#include <Types.h>

#include "nkprintf.h"

#ifndef BUG
#define BUG 0
#endif

// Patch to "re-patch" the ROM resource handle
// Apply to any trap taking a handle in a0
// Come-from patch is protected when the PowerPC Resource Manager is installed
char patch[] = {
	0x60,0x06,                      //  +0          bra.s   mycode    (come-from patch sig)
	0x4e,0xf9,0x00,0x00,0x00,0x00,  //  +2  orig    jmp     $00000000 (come-from patch sig)
	0xb1,0xfc,0x00,0x00,0x00,0x00,  //  +8  mycode  cmp.l   #$00000000,a0
	0x66,0x06,                      //  +e          bne.s   escape
	0x20,0xbc,0x00,0x00,0x00,0x00,  // +10          move.l  #$00000000,(a0)
	0x60,0xea,                      // +16  escape  bra.s   orig
};

#define patchHandOffset 0xa
#define patchPtrOffset 0x12
#define patchJmpOffset 0x4

// HSetState
#define trapTabAddr (0x400 + 4 * 0x6a)

OSErr init(CFragInitBlockPtr initBlock);

OSErr init(CFragInitBlockPtr initBlock) {
	void **romDriverHand;

	// The build script calculates the size of THIS binary
	char *driverPEF = (char *)initBlock->fragLocator.u.inMem.address +
#include ":Build:PayloadOffset.inc"
	;

	if (BUG) nkprintf("%s:%d new driver ptr=%#x\n", __FILE__, __LINE__, driverPEF);

	// Get the handle whose master pointer we must change
	*(short *)0xb9e = 0xffff; // TempInsertROMMapTrue
	romDriverHand = (void **)GetResource('usbd', -20779);
	if (BUG) nkprintf("%s:%d ROM usbd pre-patch: hand=%#x ptr=%#x\n", __FILE__, __LINE__, romDriverHand, *romDriverHand);

	// Change the master ptr persistently, every time this trap is called
	*(void **)(patch + patchHandOffset) = romDriverHand;
	*(void **)(patch + patchPtrOffset) = driverPEF;
	*(void **)(patch + patchJmpOffset) = *(void **)trapTabAddr;
	*(void **)trapTabAddr = patch;

	// Test the change
	if (BUG) {
		*(short *)0xb9e = 0xffff; // TempInsertROMMapTrue
		romDriverHand = (void **)GetResource('usbd', -20779);
		nkprintf("%s:%d ROM usbd post-patch hand=%#x ptr=%#x\n", __FILE__, __LINE__, romDriverHand, *romDriverHand);
	}

	// Ensure that this fragment's globals last as long as the system
	return noErr;
}
