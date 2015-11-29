/**
****************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MMY Application Team
* Version            : V3.3.1
* Date               : 12-June-2012
* Description        : Descriptors for USB Composite device (HID + MSD)
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "usb_desc.h"

const uint8_t COMPOSITE_DeviceDescriptor[COMPOSITE_SIZ_DEVICE_DESC] =
  {
    0x12,   /* bLength  */
    0x01,   /* bDescriptorType */
    0x00,   /* bcdUSB, version 2.00 */
    0x02,
    0x00,   /* bDeviceClass : each interface define the device class */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    0x40,   /* bMaxPacketSize0 0x40 = 64 */
    0x83,   /* idVendor     (0483) */
    0x04,
    0xD0,   /* idProduct */
    0xD0,
    0x00,   /* bcdDevice 2.00*/
    0x02,
    1,              /* index of string Manufacturer  */
    /**/
    2,              /* index of string descriptor of product*/
    /* */
    3,              /* */
    /* */
    /* */
    0x01    /*bNumConfigurations */
  };


const uint8_t COMPOSITE_ConfigDescriptor[COMPOSITE_SIZ_CONFIG_DESC] =
  {

    0x09,   /* bLength: Configuration Descriptor size */
    0x02,   /* bDescriptorType: Configuration */
    COMPOSITE_SIZ_CONFIG_DESC,

    0x00,
#ifdef USE_MSD_DRIVE
    0x02,   /* bNumInterfaces: 2 interface  => MSD + HID*/
#else
    0x01,   /* bNumInterfaces: 1 interface  => HID*/
#endif /*USE_MSD_DRIVE*/
    0x01,   /* bConfigurationValue: */
    /*      Configuration value */
    0x00,   /* iConfiguration: */
    /*      Index of string descriptor */
    /*      describing the configuration */
    0x80,   /* bmAttributes: */
    /*      bus powered */
    0x32,   /* MaxPower 100 mA */

#ifdef USE_MSD_DRIVE
    /******************** Descriptor of Mass Storage interface ********************/
    /* 09 */
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: */
    /*      Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints*/
    0x08,   /* bInterfaceClass: MASS STORAGE Class */
    0x06,   /* bInterfaceSubClass : SCSI transparent*/
    0x50,   /* nInterfaceProtocol */
    4,          /* iInterface: */
    /* 18 */
    0x07,   /*Endpoint descriptor length = 7*/
    0x05,   /*Endpoint descriptor type */
    0x81,   /*Endpoint address (IN, address 1) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00,   /*Polling interval in milliseconds */
    /* 25 */
    0x07,   /*Endpoint descriptor length = 7 */
    0x05,   /*Endpoint descriptor type */
    0x02,   /*Endpoint address (OUT, address 2) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00,     /*Polling interval in milliseconds*/
    /*32*/
#endif /*USE_MSD_DRIVE*/

	/************** Descriptor of Custom HID interface ****************/

    /* 00 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type */
    0x01,         /* bInterfaceNumber: Number of Interface */ //00
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: HID */
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    BRIDGE_SIZ_STRING_PRODUCT,			  /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID ********************/
    /* 09 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID */
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    CUSTOMHID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0,

    /******************** HID endpoints descriptor ******************/ 
    /* 18 */   	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: Endpoint descriptor type */
    0x03,	/* bEndpointAddress: Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    0x40,	/* wMaxPacketSize: 64 Bytes max  */
    0x00,
	0x01,	/* bInterval: Polling Interval (1 ms) */

    /* 25 */	
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint descriptor type */
    0x83,          /* bEndpointAddress: Endpoint Address (IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    0x40,          /* wMaxPacketSize: 64 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (1 ms) */

	/* 32 -- HID length*/
  };



const uint8_t COMPOSITE_StringLangID[COMPOSITE_SIZ_STRING_LANGID] =
  {
    COMPOSITE_SIZ_STRING_LANGID,
    0x03,
    0x09,
    0x04
  }
  ;      /* LangID = 0x0409: U.S. English */





const uint8_t COMPOSITE_StringVendor[COMPOSITE_SIZ_STRING_VENDOR] =
  {
    COMPOSITE_SIZ_STRING_VENDOR, /* Size of manufacturer string */
    0x03,           /* bDescriptorType = String descriptor */
    /* Manufacturer: "STMicroelectronics" */
    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
    'c', 0, 's', 0
  };


const uint8_t COMPOSITE_StringProduct[COMPOSITE_SIZ_STRING_PRODUCT] =
  {
    COMPOSITE_SIZ_STRING_PRODUCT,
    0x03,
    /* Product name: "STM32F10x:USB Mass Storage" */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0,
    's', 0, ' ', 0, 'S', 0, 't', 0, 'o', 0, 'r', 0, 'a', 0, 'g', 0, 'e', 0

  };


uint8_t COMPOSITE_StringSerial[COMPOSITE_SIZ_STRING_SERIAL] =
  {
    COMPOSITE_SIZ_STRING_SERIAL,
    0x03,
    /* Serial number*/
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, '1', 0, '0', 0      
  };



const uint8_t COMPOSITE_StringInterface[COMPOSITE_SIZ_STRING_INTERFACE] =
  {
    COMPOSITE_SIZ_STRING_INTERFACE,
    0x03,
    /* Interface 0: "ST Mass" */
    'S', 0, 'T', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0, 's', 0
  };

  const uint8_t HID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] =
{    
  0x05, 0x8c,            /* USAGE_PAGE (ST Page)           */                   
  0x09, 0x01,            /* USAGE (Demo Kit)               */    
  0xa1, 0x01,            /* COLLECTION (Application)       */            
  /* 6 */
	
		0x85, 0x01, // Report ID (1) 
		0x95, 0x3F, // REPORT_COUNT (64) 
		0x75, 0x08, // REPORT_SIZE (8) 
		0x26, 0xff, 0x00, // LOGICAL_MAXIMUM (255) 
		0x15, 0x00, // LOGICAL_MINIMUM (0) 
		0x09, 0x01, // USAGE (Vendor Usage 1) 
		0x91, 0x02, // OUTPUT (Data,Var,Abs) 
		/*21*/ 
		
		0x85, 0x02, // Report ID (2) 
		0x95, 0x3F, // REPORT_COUNT (64) 
		0x75, 0x08, // REPORT_SIZE (8) 
		0x26, 0xff, 0x00, // LOGICAL_MAXIMUM (255) 
		0x15, 0x00, // LOGICAL_MINIMUM (0) 
		0x09, 0x01, // USAGE (Vendor Usage 1) 
		0x91, 0x02, // OUTPUT (Data,Var,Abs) 
		/*36*/ 

		0x85, 0x07, // Report ID (7) 
		0x95, 0x3F, // REPORT_COUNT (64) 
		0x75, 0x08, // REPORT_SIZE (8) 
		0x26, 0xff, 0x00, // LOGICAL_MAXIMUM (255) 
		0x15, 0x00, // LOGICAL_MINIMUM (0) 
		0x09, 0x01, // USAGE (Vendor Usage 1) 
		0x81, 0x02, // INPUT (Data,Var,Abs) 
		/*51*/

		0xC0 	          /* END_COLLECTION	 */
	/* 127 */
}; /* CustomHID_ReportDescriptor */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
