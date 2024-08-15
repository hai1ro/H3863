﻿/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2023. All rights reserved.
 * 文 件 名   : hmac_arp_offload.h
 * 生成日期   : 2015年5月20日
 * 功能描述   : hmac_arp_offload.c 的头文件
 */

#ifndef __HMAC_ARP_OFFLOAD_H__
#define __HMAC_ARP_OFFLOAD_H__

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "hmac_vap.h"
#include "hmac_config.h"
#include "msg_wow_rom.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef  THIS_FILE_ID
#define THIS_FILE_ID OAM_FILE_ID_HMAC_ARP_OFFLOAD_H

/*****************************************************************************
  2 宏定义
*****************************************************************************/


/*****************************************************************************
  3 枚举定义
*****************************************************************************/


/*****************************************************************************
  4 全局变量声明
*****************************************************************************/


/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/

/*****************************************************************************
  8 UNION定义
*****************************************************************************/

/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/

/*****************************************************************************
  10 函数声明
*****************************************************************************/
osal_void hmac_arp_offload_get_param(mac_arp_offload_param_sync *arp_param);

static osal_u32 hmac_arp_offload_init_weakref(osal_void)
    __attribute__ ((weakref("hmac_arp_offload_init"), used));
static osal_void hmac_arp_offload_deinit_weakref(osal_void)
    __attribute__ ((weakref("hmac_arp_offload_deinit"), used));

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of hmac_arp_offload.h */
