/**
  ******************************************************************************
  * @file    ameba_ota.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the OTA functions.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _AMEBA_OTA_H_
#define _AMEBA_OTA_H_


/** @addtogroup AmebaD_Platform
  * @{
  */
#define SERVER_LOCAL	1
#define SERVER_CLOUD	2
#define SERVER_TYPE		SERVER_LOCAL						/*configure OTA demo type*/
#define MAX_IMG_NUM		4

#define HTTP_OTA_UPDATE
#define HTTPS_OTA_UPDATE
#define SDCARD_OTA_UPDATE

#define BUF_SIZE			2048								/*the size of the buffer used for receiving firmware data from server*/

extern u32 IMG_ADDR[MAX_IMG_NUM][2];


#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)

#define HEADER_BAK_LEN			32
#define HEADER_LEN				8
#define SUB_HEADER_LEN			24

typedef struct {
	u32	status_code;
	u32	header_len;
	u8		*body;
	u32	body_len;
	u8		*header_bak;
	u32	parse_status;
} http_response_result_t;
#endif


/** @defgroup OTA
  * @brief OTA driver modules
  * @{
  */

/* Exported Types --------------------------------------------------------*/
/** @defgroup OTA_Exported_Types OTA Exported Types
  * @{
  */

/**
  * @brief  OTA firmware file header structure definition
  */
typedef struct {
	u32	FwVer;	/*!< Specifies the OTA firmware verision.
	                         This parameter is in first Dword in the firmware file. */
	u32	HdrNum;/*!< Specifies the OTA firmware header number.
	                         This parameter indicates how many headers in firmware file. */
} update_file_hdr;

/**
  * @brief  OTA firmware file image header structure definition
  */
typedef struct {
	u8	Signature[4];	/*!< Specifies the OTA image signature, the value is "OTA".
	                         	This parameter is used to identify the OTA header needed. */
	u32	ImgHdrLen;	/*!< Specifies the OTA image header length.
	                         	This parameter indicates the Image Header Length. */
	u32	Checksum;	/*!< Specifies the OTA image checksum.
	                         	This parameter is used to judge whether the image received is correct. */
	u32  ImgLen;		/*!< Specifies the OTA image length. */
	u32  Offset;		/*!< Specifies the the location in the total firmware file. */
	u32  ImgID;    /*!< Specifies the image ID of the corresponding image. */
} update_file_img_hdr;

/**
  * @brief  OTA firmware file download information structure definition
  */
typedef struct {
	u32 ImgId;		/*!< Specifies the Image ID.*/

	u32	FlashAddr;	/*!< Specifies the Flash Address.
	                         	This parameter is used to write the Image to the flash. */
	u32	ImgOffset;	/*!< Specifies the Image location in Firmware header.
	                         	This parameter indicates the Image location in firmware file. */
	u32	ImageLen;	/*!< Specifies the OTA image length. */
} update_dw_info;

/**
  * @brief  OTA target image manifest structure definition
  */
typedef struct {
	unsigned int Pattern[2];
	unsigned char Rsvd1[8];
	unsigned char Ver;
	unsigned char ImgID;
	unsigned char AuthAlg;
	unsigned char HashAlg;
	unsigned short MajorImgVer;
	unsigned short MinorImgVer;
} update_manifest_info;


/**
  * @brief  OTA target image header structure definition
  */
typedef struct {
	update_file_hdr	FileHdr;			/*!< Specifies the firmware file header. */
	update_file_img_hdr	FileImgHdr[MAX_IMG_NUM];	/*!< Specifies the target OTA image firmware file header. */
	update_manifest_info Manifest[MAX_IMG_NUM];			/*!< Specifies the manifest of target image. */
	u8 ValidImgCnt;						/*!< Specifies valid image number in file. */
} update_ota_target_hdr;

/**
  * @brief  OTA target image download control information structure definition
  */
typedef struct {
	u8 NextImgFg;			/*!< Specifies the Flag that Buffer belong to next image. */
	u8 NextImgBuf[BUF_SIZE];/*!< Specifies the Buffer belong to next image. */
	int NextImgLen;			/*!< Specifies the Buffer Length belong to next image. */
	u32 FlashAddr;			/*!< Specifies the Flash Address.
									This parameter is used to write the Image to the flash. */
	int ReadBytes;		/*!< Specifies the Bytes already received. */
	int RemainBytes;		/*!< Specifies the Remain Bytes of OTA firmware. */
	int ImgOffset;		/*!< Specifies the Offset of Manifest in firmware. */
	int SigCnt;		/*!< Specifies the Manifest received length. */
	u8 SigFg;		/*!< Specifies the Flag that Manifest received finished. */
	u8 SkipBootOTAFg;	/*!< Specifies the Flag that skip update the bootloader. */
	u8 FirstBufFg;		/*!< Specifies the Flag that exist a buffer before downloading. */
} update_ota_ctrl_info;


/* Exported constants --------------------------------------------------------*/

/** @defgroup OTA_Exported_Constants OTA Exported Constants
  * @{
  */
/** @defgroup OTA_system_parameter_definitions
  * @{
  */

#define OTA_IMGID_BOOT		0
#define OTA_IMGID_IMG2		1
#define OTA_IMGID_IMG3		2
#define OTA_IMGID_DSP		3

#define OTA_IMAG			0								/*identify the OTA image*/

/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup OTA_Exported_Functions OTA Exported Functions
  * @{
  */
u32 OTA_Change(u32 OTAIdx);

void *ota_update_malloc(unsigned int size);
void ota_update_free(void *buf);
void ota_platform_reset(void);
int ota_write_ota2_addr(uint32_t ota_addr);
u32 ota_get_cur_index(u32 idx);
int  ota_readstream_user(u32 address, u32 len, u8 *data);

u32 recv_file_info_from_server(u8 *Recvbuf, u32 len, int socket);
u32 recv_ota_file_hdr(u8 *Recvbuf, u32 *len, update_ota_target_hdr *pOtaTgtHdr, int socket);
u32 get_ota_tartget_header(u8 *buf, u32 len, update_ota_target_hdr *pOtaTgtHdr, u8 target_idx);
void erase_ota_target_flash(u32 addr, u32 len);
u32 download_new_fw_from_server(int socket, update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index);
u32 verify_ota_checksum(update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index);
u32 change_ota_version(update_ota_target_hdr *pOtaTgtHdr, u32 ota_target_index, int index);


#if (defined HTTP_OTA_UPDATE) || (defined HTTPS_OTA_UPDATE)
int  parser_url(char *url, char *host, u16 *port, char *resource);
int parse_http_response(unsigned char *response, unsigned int response_len, http_response_result_t *result);
#ifdef HTTP_OTA_UPDATE
int update_ota_http_connect_server(int server_socket, char *host, int port);
u32 recv_ota_file_hdr_http(u8 *Recvbuf, u32 writelen, u32 *len, update_ota_target_hdr *pOtaTgtHdr, int socket);
int http_read_socket(int socket, u8 *recevie_buf, int buf_len);
u32 download_new_fw_from_server_http(u8 *first_buf, unsigned int firstbuf_len, int socket,
									 update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index);
int http_update_ota(char *host, int port, char *resource);
#endif
#ifdef HTTPS_OTA_UPDATE
#include <mbedtls/config.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
u32 recv_ota_file_hdr_https(u8 *Recvbuf, u32 writelen, u32 *len, update_ota_target_hdr *pOtaTgtHdr,
							mbedtls_ssl_context *ssl);
int https_read_socket(mbedtls_ssl_context *ssl, u8 *recevie_buf, int buf_len);
u32 download_new_fw_from_server_https(u8 *first_buf, unsigned int firstbuf_len, mbedtls_ssl_context *ssl,
									  update_ota_target_hdr *pOtaTgtHdr, u8 targetIdx, int index);
int https_update_ota(char *host, int port, char *resource);
#endif
#endif


#define OTA_GET_FWVERSION(address) \
	(HAL_READ16(SPI_FLASH_BASE, address + 22) << 16) | HAL_READ16(SPI_FLASH_BASE, address + 20)

#define _OTA_INFO_	3
#define _OTA_WARN_	2
#define _OTA_ERR_	1

#define ota_printf(level, fmt, arg...)     \
do {\
	if(level == _OTA_INFO_) \
		printf("\r\n[OTA INFO]: " fmt, ##arg);\
	else if(level == _OTA_WARN_) \
		printf("\r\n[OTA WRAN]: " fmt, ##arg);\
	else if(level == _OTA_ERR_) \
		printf("\r\n[OTA ERROR]: " fmt, ##arg);\
}while(0)

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif //_AMEBA_OTA_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
