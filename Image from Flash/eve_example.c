/**
 @file eve_example.c
 */
/*
 * ============================================================================
 * History
 * =======
 * Nov 2021	1.0	Initial version
 *
 *
 *
 *
 *
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://www.ftdichip.com/FTSourceCodeLicenceTerms.htm ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of Bridgetek liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on Bridgetek's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, Bridgetek
 * has no liability in relation to those amendments.
 * ============================================================================
 */


#include <stdint.h>
#include "EVE.h"
#include <HAL.h>
#include "eve_example.h"

void Flash_Full_Speed(void)
{
		uint8_t Flash_Status = 0;

		// Detach Flash
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHDETATCH();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = HAL_MemRead8(EVE_REG_FLASH_STATUS);

		if (EVE_FLASH_STATUS_DETACHED != Flash_Status)
		{
			EVE_LIB_BeginCoProList();
			EVE_CMD_DLSTART();
			EVE_CLEAR_COLOR_RGB(255, 0, 0);
			EVE_CLEAR(1,1,1);
			EVE_COLOR_RGB(255,255,255);
			EVE_CMD_TEXT(100, 50, 28, 0, "Error detaching flash");
			EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
			EVE_DISPLAY();
			EVE_CMD_SWAP();
			EVE_LIB_EndCoProList();
			EVE_LIB_AwaitCoProEmpty();
			while(1)
			{
			}
		}

		// Attach
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHATTACH();
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = HAL_MemRead8(EVE_REG_FLASH_STATUS);

		if (EVE_FLASH_STATUS_BASIC != Flash_Status)
		{
			EVE_LIB_BeginCoProList();
			EVE_CMD_DLSTART();
			EVE_CLEAR_COLOR_RGB(0, 255, 0);
			EVE_CLEAR(1,1,1);
			EVE_COLOR_RGB(255,255,255);
			EVE_CMD_TEXT(100, 50, 28, 0, "Error attaching flash");
			EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
			EVE_DISPLAY();
			EVE_CMD_SWAP();
			EVE_LIB_EndCoProList();
			EVE_LIB_AwaitCoProEmpty();
			while(1)
			{
			}
		}

		// Fast mode
		EVE_LIB_BeginCoProList();
		EVE_CMD_FLASHFAST(0);
		EVE_LIB_EndCoProList();
		EVE_LIB_AwaitCoProEmpty();

		Flash_Status = HAL_MemRead8(EVE_REG_FLASH_STATUS);

		if (EVE_FLASH_STATUS_FULL != Flash_Status)
		{

			EVE_LIB_BeginCoProList();
			EVE_CMD_DLSTART();
			EVE_CLEAR(1,1,1);
			EVE_CLEAR_COLOR_RGB(0, 0, 255);
			EVE_COLOR_RGB(255,255,255);
			EVE_CMD_TEXT(100, 50, 28, 0, "Error going to full mode");
			EVE_CMD_TEXT(100,100, 28, EVE_OPT_FORMAT, "Mode is %d ", Flash_Status);
			EVE_DISPLAY();
			EVE_CMD_SWAP();
			EVE_LIB_EndCoProList();
			EVE_LIB_AwaitCoProEmpty();
			while(1)
			{
			}
		}

}

void eve_display(void)
{
	// copy RAW image to RAM_G using Flash Read
	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHREAD(0, 4096, 54720); // Destination: RAM_G + 0, Source: Flash address 4096, Size: 54720. (flash address and size noted from map file)
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	// Inflate compressed bin file direct from the flash
	EVE_LIB_BeginCoProList();
	EVE_CMD_FLASHSOURCE(58816);			// Source address in Flash: 58816 (as determined from map file)
	EVE_CMD_INFLATE2(80000,EVE_OPT_FLASH);	// Destination for inflated data: RAM_G + 80000, Option tells EVE that source is from Flash
	EVE_LIB_EndCoProList();
	EVE_LIB_AwaitCoProEmpty();

	// So we now have a raw image at address RAM_G + 0 and a decompressed image (so now also raw data) at address RAM_G + 80000

	// Create screen
	EVE_LIB_BeginCoProList();							// Begin a new list of commands
	EVE_CMD_DLSTART();									// Start a new Display List
	EVE_CLEAR_COLOR_RGB(255, 255, 255);					// Clear screen to white
	EVE_CLEAR(1,1,1);									// Clear
	EVE_COLOR_RGB(255,255,255);							// Draw following items in white (display images in their original colours)

	EVE_BEGIN(EVE_BEGIN_BITMAPS);						// Begin drawing bitmaps
	EVE_CMD_SETBITMAP(0, EVE_FORMAT_ARGB1555, 240, 114);// First we draw the image which we copied to RAM_G + 0
	EVE_VERTEX2F((20*16),(100*16));						// Place at coordinate 20,100
	//EVE_END();										// no need for end here as we will draw more bitmaps
	//EVE_BEGIN(EVE_BEGIN_BITMAPS);
	EVE_CMD_SETBITMAP(80000, EVE_FORMAT_ARGB1555, 240, 114);// Now we will draw the image which we decompressed from Flash to RAM_G + 80000
	EVE_VERTEX2F((280*16),(100*16));					// Place at coordinate 280, 100
	//EVE_END();										// no need for end here as we will draw more bitmaps
	//EVE_BEGIN(EVE_BEGIN_BITMAPS);
	// Now we will draw the image direct from flash.
	// When displaying from flash, the address is calculated by [0x800000 | (address in the flash via map file/32)]
	EVE_CMD_SETBITMAP(0x800000 | 66496 / 32, EVE_FORMAT_COMPRESSED_RGBA_ASTC_4x4_KHR, 240, 114);
	EVE_VERTEX2F((540*16),(100*16));					// Place at coordinate 540, 100
	EVE_END();

	// Add some text below each image to describe it. Note the use of the \n for a new line which is a new feature in the BT series.
	EVE_COLOR_RGB(0, 0, 255);
	EVE_CMD_TEXT(20 + (240/2), 240, 28, EVE_OPT_FORMAT | EVE_OPT_CENTER, "Raw Image \n Copied to RAM_G");
	EVE_CMD_TEXT(280 + (240/2),240, 28, EVE_OPT_FORMAT | EVE_OPT_CENTER, "Bin Image \n Inflated to RAM_G");
	EVE_CMD_TEXT(540 + (240/2),240, 28, EVE_OPT_FORMAT | EVE_OPT_CENTER, "ASTC Image \n Direct from Flash");

	EVE_DISPLAY();		// Always finish the list with DISPLAY to tell EVE that this is the enmd of the list
	EVE_CMD_SWAP();		// A swap then causes the new Display List to be made active (swap foreground and background buffers)
	EVE_LIB_EndCoProList();		// framework-specific function. Ends the SPI transfer and writes the new address to the CMD FIFO write pointer
	EVE_LIB_AwaitCoProEmpty();	// Await the CMD FIFO Read pointer to catch up, indicating that all commands were executed

	while(1)
	{
	}

}

void eve_example(void)
{
	// Initialise the display
	EVE_Init();

	// Calibrate the display
	//eve_calibrate();

	// Set Flash to full speed
	Flash_Full_Speed();

	// Start example code
	eve_display();
}

