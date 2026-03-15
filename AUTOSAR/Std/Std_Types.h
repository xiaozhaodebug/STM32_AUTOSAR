/**
 * @file        Std_Types.h
 * @brief       AUTOSAR标准类型定义
 * @details     基于AUTOSAR R23-11标准的简化类型定义
 *              包含标准类型、返回类型、版本信息类型
 * 
 * @author      [小昭debug]
 * @date        2026-03-15
 */

#ifndef STD_TYPES_H
#define STD_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/*==================================================================================================
 *                                      标准类型定义
==================================================================================================*/
typedef unsigned char       uint8;      /**< 8位无符号整数 */
typedef unsigned short      uint16;     /**< 16位无符号整数 */
typedef unsigned long       uint32;     /**< 32位无符号整数 */
typedef unsigned long long  uint64;     /**< 64位无符号整数 */

typedef signed char         sint8;      /**< 8位有符号整数 */
typedef signed short        sint16;     /**< 16位有符号整数 */
typedef signed long         sint32;     /**< 32位有符号整数 */
typedef signed long long    sint64;     /**< 64位有符号整数 */

typedef unsigned int        uint8_least;    /**< 至少8位无符号整数 */
typedef unsigned int        uint16_least;   /**< 至少16位无符号整数 */
typedef unsigned long       uint32_least;   /**< 至少32位无符号整数 */

typedef signed int          sint8_least;     /**< 至少8位有符号整数 */
typedef signed int          sint16_least;    /**< 至少16位有符号整数 */
typedef signed long         sint32_least;    /**< 至少32位有符号整数 */

typedef float               float32;    /**< 32位浮点数 */
typedef double              float64;    /**< 64位浮点数 */

/*==================================================================================================
 *                                      标准返回类型
==================================================================================================*/
typedef uint8 Std_ReturnType;

#ifndef E_OK
#define E_OK            (Std_ReturnType)0x00   /**< 操作成功 */
#endif

#ifndef E_NOT_OK
#define E_NOT_OK        (Std_ReturnType)0x01   /**< 操作失败 */
#endif

#define E_BUSY          (Std_ReturnType)0x02   /**< 操作忙 */
#define E_QUEUE_FULL    (Std_ReturnType)0x03   /**< 队列满 */

/*==================================================================================================
 *                                      布尔类型
==================================================================================================*/
typedef bool boolean;   /**< AUTOSAR布尔类型 */

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

/*==================================================================================================
 *                                      物理状态类型
==================================================================================================*/
typedef uint8 Std_TransformerStatus;
#define STD_TRANSFORMER_UNSPECIFIED   0x00
#define STD_TRANSFORMER_SERIALIZER    0x01
#define STD_TRANSFORMER_SAFETY        0x02
#define STD_TRANSFORMER_SECURITY      0x03
#define STD_TRANSFORMER_CUSTOM        0xFF

/*==================================================================================================
 *                                      版本信息类型
==================================================================================================*/
typedef struct {
    uint16  vendorID;           /**< 供应商ID */
    uint16  moduleID;           /**< 模块ID */
    uint8   sw_major_version;   /**< 软件主版本 */
    uint8   sw_minor_version;   /**< 软件次版本 */
    uint8   sw_patch_version;   /**< 软件补丁版本 */
} Std_VersionInfoType;

/*==================================================================================================
 *                                      函数指针类型
==================================================================================================*/
typedef void (*Std_VoidFuncPtr)(void);
typedef void (*Std_CallbackFuncPtr)(uint8 result);

/*==================================================================================================
 *                                      宏定义
==================================================================================================*/
#define STD_HIGH        0x01    /**< 高电平 */
#define STD_LOW         0x00    /**< 低电平 */

#define STD_ACTIVE      0x01    /**< 激活状态 */
#define STD_IDLE        0x00    /**< 空闲状态 */

#define STD_ON          0x01    /**< 开启 */
#define STD_OFF         0x00    /**< 关闭 */

#define STD_ENABLE      0x01    /**< 使能 */
#define STD_DISABLE     0x00    /**< 禁用 */

/*==================================================================================================
 *                                      空指针定义
==================================================================================================*/
#ifndef NULL_PTR
#define NULL_PTR    ((void*)0)
#endif

/*==================================================================================================
 *                                      编译器抽象
==================================================================================================*/
#define AUTOMATIC           /**< 自动存储类 */
#define TYPEDEF             /**< typedef存储类 */

#define LOCAL_INLINE        static inline    /**< 局部内联函数 */
#define LOCAL               static           /**< 局部函数/变量 */

#define FUNC(rettype, memclass)                     rettype
#define FUNC_P2CONST(rettype, ptrclass, memclass)   const rettype*
#define FUNC_P2VAR(rettype, ptrclass, memclass)     rettype*
#define P2VAR(ptrtype, memclass, ptrclass)          ptrtype*
#define P2CONST(ptrtype, memclass, ptrclass)        const ptrtype*
#define CONSTP2VAR(ptrtype, memclass, ptrclass)     ptrtype* const
#define CONSTP2CONST(ptrtype, memclass, ptrclass)   const ptrtype* const

#define VAR(vartype, memclass)                      vartype
#define CONST(consttype, memclass)                  const consttype

/*==================================================================================================
 *                                      模块ID定义
==================================================================================================*/
#define STD_MODULE_ID_CANIF     0x3C    /**< CAN接口模块 */
#define STD_MODULE_ID_CANTP     0x3D    /**< CAN传输协议模块 */
#define STD_MODULE_ID_PDUR      0x69    /**< PDU路由器模块 */
#define STD_MODULE_ID_COM       0x19    /**< 通信模块 */
#define STD_MODULE_ID_COMM      0x1A    /**< 通信管理模块 */
#define STD_MODULE_ID_DCM       0x21    /**< 诊断通信管理模块 */
#define STD_MODULE_ID_DEM       0x20    /**< 诊断事件管理模块 */
#define STD_MODULE_ID_DET       0x01    /**< 开发和错误追踪模块 */
#define STD_MODULE_ID_ECUM      0x08    /**< ECU管理模块 */

/*==================================================================================================
 *                                      错误代码定义
==================================================================================================*/
#define STD_E_NO_ERROR          0x00    /**< 无错误 */
#define STD_E_PARAM_POINTER     0x01    /**< 参数指针错误 */
#define STD_E_PARAM_DATA        0x02    /**< 参数数据错误 */
#define STD_E_PARAM_RANGE       0x03    /**< 参数范围错误 */
#define STD_E_PARAM_CONFIG      0x04    /**< 参数配置错误 */
#define STD_E_PARAM_LENGTH      0x05    /**< 参数长度错误 */
#define STD_E_UNINIT            0x10    /**< 未初始化错误 */
#define STD_E_ALREADY_INITIALIZED   0x11    /**< 已初始化错误 */
#define STD_E_BUSY              0x12    /**< 忙错误 */
#define STD_E_TIMEOUT           0x13    /**< 超时错误 */
#define STD_E_QUEUE_FULL        0x14    /**< 队列满错误 */

#endif /* STD_TYPES_H */
