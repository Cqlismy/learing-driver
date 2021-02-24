/*************************************************************/
//2014.03.06修改版
/*************************************************************/
#define LOG_TAG "MifareCard"
//#define LOG_NDDEBUG 0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <cassert>
#include <cstdlib>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <cutils/str_parms.h>

#include "pn512_io.h"
#include "pn512_reg.h"
#include "MifareCard.h"

MifareCard::MifareCard(PN512_IO *dev)
{
	nfc_dev = dev;
}

/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		TRUE    :认证成功																	 */
/*		FALSE :认证失败																  	 */
/*****************************************************************************************/
int MifareCard::Auth(unsigned char mode, unsigned char sector, unsigned char *mifare_key, unsigned char *card_uid)
{
	unsigned char send_buff[12],rece_buff[1];
	unsigned int  rece_bitlen;
	int res;

	if (mode==0x0)
		send_buff[0]=0x60;//kayA认证
	else if(mode==0x01)
		send_buff[0]=0x61;//keyB认证
	else
		return -1;

  	send_buff[1]=sector*4;
	send_buff[2]=mifare_key[0];
	send_buff[3]=mifare_key[1];
	send_buff[4]=mifare_key[2];
	send_buff[5]=mifare_key[3];
	send_buff[6]=mifare_key[4];
	send_buff[7]=mifare_key[5];
	send_buff[8]=card_uid[0];
	send_buff[9]=card_uid[1];
	send_buff[10]=card_uid[2];
	send_buff[11]=card_uid[3];

// 	Pcd_SetTimer(1);
// 	Clear_FIFO();
// 	res =Pcd_Comm(MFAuthent,send_buff,12,rece_buff,&rece_bitlen);//Authent认证

	res = nfc_dev->transfer(MFAuthent, 1,
		send_buff, 12,
		rece_buff, 1, &rece_bitlen);

	if (res >= 0)
	{
		//判断加密标志位，确认认证结果
		if (nfc_dev->reg_read(Status2Reg) & 0x08)
			return 0;
		else
			return -1;
	}

	return res;
}
/*****************************************************************************************/
/*名称：Mifare_Blockset																	 */
/*功能：Mifare_Blockset卡片数值设置														 */
/*输入：block，块号；*buff，需要设置的4字节数值数组										 */
/*																						 */
/*输出:																					 */
/*		TRUE    :设置成功																	 */
/*		FALSE :设置失败																	 */
/*****************************************************************************************/
int MifareCard::BlockSet(unsigned char block,unsigned char *buff)
{
	unsigned char  block_data[16];

	block_data[0]=buff[3];
	block_data[1]=buff[2];
	block_data[2]=buff[1];
	block_data[3]=buff[0];
	block_data[4]=~buff[3];
	block_data[5]=~buff[2];
	block_data[6]=~buff[1];
	block_data[7]=~buff[0];
	block_data[8]=buff[3];
	block_data[9]=buff[2];
	block_data[10]=buff[1];
	block_data[11]=buff[0];
	block_data[12]=block;
	block_data[13]=~block;
	block_data[14]=block;
	block_data[15]=~block;

	return BlockWrite(block, block_data);
}

/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/
int MifareCard::BlockRead(unsigned char block, unsigned char *buff)
{	
	unsigned char  send_buff[2];
	unsigned int  rece_bitlen;
	int res;

	
	send_buff[0]=0x30;//30 读块
	send_buff[1]=block;//块地址

// 	Pcd_SetTimer(1);
// 	Clear_FIFO();
// 	res =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//

	res = nfc_dev->transfer(Transceive, 1,
		send_buff, 2,
		buff, 16, &rece_bitlen);

	if ((res <= 0) | (rece_bitlen != 16 * 8)) //接收到的数据长度为16
		return -1;

	return res;
}

/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/
int MifareCard::BlockWrite(unsigned char block,unsigned char *buff)
{	
	unsigned char send_buff[16],rece_buff[1];
	unsigned int  rece_bitlen;
	int res;

	
	send_buff[0]=0xa0;//a0 写块
	send_buff[1]=block;//块地址

// 	Pcd_SetTimer(1);
//  Clear_FIFO();
// 	res =Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);//
	res = nfc_dev->transfer(Transceive, 1,
		send_buff, 2,
		rece_buff, 1, &rece_bitlen);

	//如果未接收到0x0A，表示无ACK
	if ((res < 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;
	
// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	res =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);//
	res = nfc_dev->transfer(Transceive, 5,
		buff, 16,
		rece_buff, 1, &rece_bitlen);

	//如果未接收到0x0A，表示无ACK
	if ((res <= 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;

	return res;
}
/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/
int MifareCard::BlockInc(unsigned char block,unsigned char *buff)
{	
	unsigned char send_buff[2],rece_buff[1];
	unsigned int  rece_bitlen;
	int res;

	send_buff[0]=0xc1;//
	send_buff[1]=block;//块地址

// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	res=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);

	res = nfc_dev->transfer(Transceive, 5,
		send_buff, 2,
		rece_buff, 1, &rece_bitlen);
	
	//如果未接收到0x0A，表示无ACK
	if ((res <= 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;


// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
	res = nfc_dev->transfer(Transceive, 5,
		buff, 4,
		rece_buff, 1, &rece_bitlen);

	return res;
}

/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/

int MifareCard::BlockDec(unsigned char block, unsigned char *buff)
{	
	unsigned char send_buff[2],rece_buff[1];
	unsigned int  rece_bitlen;
	int res;
	
	send_buff[0]=0xc0;//
	send_buff[1]=block;//块地址

// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	res=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	res = nfc_dev->transfer(Transceive, 5,
		send_buff, 2,
		rece_buff, 1, &rece_bitlen);

	//如果未接收到0x0A，表示无ACK
	if ((res <= 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;

// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
	res = nfc_dev->transfer(Transceive, 5,
		buff, 4,
		rece_buff, 1, &rece_bitlen);

	return res;
}

/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/
int MifareCard::Transfer(unsigned char block)
{	
	unsigned char  send_buff[2],rece_buff[1];
	unsigned int   rece_bitlen;
	int res;

	send_buff[0]=0xb0;//
	send_buff[1]=block;//块地址
// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	res=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);

	res = nfc_dev->transfer(Transceive, 5,
		send_buff, 2,
		rece_buff, 1, &rece_bitlen);

	//如果未接收到0x0A，表示无ACK
	if ((res <= 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;

	return res;
}

/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		TRUE    :成功																		 */
/*		FALSE :失败																		 */
/*****************************************************************************************/
int MifareCard::Restore(unsigned char block)
{	
	unsigned char  send_buff[4],rece_buff[1];
	unsigned int   rece_bitlen;
	int res;
	
	send_buff[0]=0xc2;//
	send_buff[1]=block;//块地址
// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	res=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
	res = nfc_dev->transfer(Transceive, 5,
		send_buff, 2,
		rece_buff, 1, &rece_bitlen);

	//如果未接收到0x0A，表示无ACK
	if ((res <= 0) | ((rece_buff[0] & 0x0F) != 0x0A))
		return -1;
		
	send_buff[0]=0x00;
	send_buff[1]=0x00;
	send_buff[2]=0x00;
	send_buff[3]=0x00;
// 	Pcd_SetTimer(5);
// 	Clear_FIFO();
// 	Pcd_Comm(Transceive,send_buff,4,rece_buff,&rece_bitlen);
	res = nfc_dev->transfer(Transceive, 5,
		send_buff, 4,
		rece_buff, 1, &rece_bitlen);

	return res;
}
