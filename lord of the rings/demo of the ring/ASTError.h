#ifndef _AST_ERROR_H_
#define _AST_ERROR_H_
#include "common.h"

//#define USE_POSITIVE_ERROR_VALUE
#ifdef USE_POSITIVE_ERROR_VALUE
#define _r(x) (x)
#else
#define _r(x) (-(x))
#endif

#undef DEF_ERROR
#define DEF_ERROR(code,str) __##code
enum E_ERR_CODE
{
#include "error.h"
};

#define ERR_NONE                    0
#define ERR_GENERIC                 _r(__ERR_GENERIC)
#define ERR_SEM_CREATE_FAILED       _r(__ERR_SEM_CREATE_FAILED)
#define ERR_THREAD_CREATE_FAILED    _r(__ERR_THREAD_CREATE_FAILED)
#define ERR_PROCESS_FAILED          _r(__ERR_PROCESS_FAILED)
#define ERR_IF_RESET                _r(__ERR_IF_RESET)
#define ERR_IF_ALREADY_EXIST        _r(__ERR_IF_ALREADY_EXIST)
#define ERR_IF_SETUP_FAILED         _r(__ERR_IF_SETUP_FAILED)
#define ERR_IF_CONN_FAILED          _r(__ERR_IF_CONN_FAILED)
#define ERR_IF_REQUEST_FAILED       _r(__ERR_IF_REQUEST_FAILED)
#define ERR_FILE_IO                 _r(__ERR_FILE_IO)
#define ERR_PATH_NOT_EXIST          _r(__ERR_PATH_NOT_EXIST)
#define ERR_PATH_ALREADY_EXIST      _r(__ERR_PATH_ALREADY_EXIST)
#define ERR_NOT_AVAIL_ON_FILE       _r(__ERR_NOT_AVAIL_ON_FILE)
#define ERR_NOT_AVAIL_ON_DIR        _r(__ERR_NOT_AVAIL_ON_DIR)
#define ERR_NOT_AVAIL_ON_SUB_DIR    _r(__ERR_NOT_AVAIL_ON_SUB_DIR)
#define ERR_NOT_AVAIL_ON_ROOT_DIR   _r(__ERR_NOT_AVAIL_ON_ROOT_DIR)
#define ERR_OPEN_FILE_FAILED        _r(__ERR_OPEN_FILE_FAILED)
#define ERR_CREATE_FILE_FAILED      _r(__ERR_CREATE_FILE_FAILED)
#define ERR_CREATE_DIR_FAILED       _r(__ERR_CREATE_DIR_FAILED)
#define ERR_LOAD_LIBRARY_FAILED     _r(__ERR_LOAD_LIBRARY_FAILED)
#define ERR_BAD_CONFIG_FORMAT       _r(__ERR_BAD_CONFIG_FORMAT)
#define ERR_BUFFER_OVERFLOW         _r(__ERR_BUFFER_OVERFLOW)
#define ERR_EXEC_NOT_FOUND          _r(__ERR_EXEC_NOT_FOUND)
#define ERR_EXEC_INFO_NOT_FOUND     _r(__ERR_EXEC_INFO_NOT_FOUND)
#define ERR_EXEC_INFO_NOT_VALID     _r(__ERR_EXEC_INFO_NOT_VALID)
#define ERR_CUR_DIR_NOT_FOUND       _r(__ERR_CUR_DIR_NOT_FOUND)
#define ERR_INVALID_PATH            _r(__ERR_INVALID_PATH)
#define ERR_DUP_PROCESS             _r(__ERR_DUP_PROCESS)
#define ERR_INVALID_TYPE            _r(__ERR_INVALID_TYPE)
#define ERR_INSUFFICIENT_BUFSIZE    _r(__ERR_INSUFFICIENT_BUFSIZE)
#define ERR_BUSY                    _r(__ERR_BUSY)
#define ERR_TIMEOUT                 _r(__ERR_TIMEOUT)
#define ERR_MODULE_NOT_FOUND        _r(__ERR_MODULE_NOT_FOUND)
#define ERR_MODULE_NOT_INITED       _r(__ERR_MODULE_NOT_INITED)
#define ERR_INVALID_PARAM           _r(__ERR_INVALID_PARAM)
#define ERR_INVALID_CMD             _r(__ERR_INVALID_CMD)
#define ERR_INVALID_MPATTERN        _r(__ERR_INVALID_MPATTERN)
#define ERR_INVALID_ENV_NAME        _r(__ERR_INVALID_ENV_NAME)
#define ERR_PARSE_ENV_FAILED        _r(__ERR_PARSE_ENV_FAILED)
#define ERR_ENV_NOT_FOUND           _r(__ERR_ENV_NOT_FOUND)
#define ERR_FS_NO_ACCESS            _r(__ERR_FS_NO_ACCESS)
#define ERR_FS_INVALID_HANDLE       _r(__ERR_FS_INVALID_HANDLE)
#define ERR_FS_NEGATIVE_POSITION    _r(__ERR_FS_NEGATIVE_POSITION)
#define ERR_FS_DEV_NOT_FOUND        _r(__ERR_FS_DEV_NOT_FOUND)
#define ERR_FS_DEV_MOUNT_FAILED     _r(__ERR_FS_DEV_MOUNT_FAILED)
#define ERR_FS_DEV_FORMAT_FAILED    _r(__ERR_FS_DEV_FORMAT_FAILED)
#define ERR_FS_DEV_MOUNT_FAILED_NOT_EXIST       _r(__ERR_FS_DEV_MOUNT_FAILED_NOT_EXIST)
#define ERR_FS_DEV_MOUNT_FAILED_ALREADY_MOUNTED _r(__ERR_FS_DEV_MOUNT_FAILED_ALREADY_MOUNTED)
#define ERR_FS_DEV_FORMAT_FAILED_ALREADY_INUSE  _r(__ERR_FS_DEV_FORMAT_FAILED_ALREADY_INUSE)
#define ERR_FS_FILE_DIR_LOCKED      _r(__ERR_FS_FILE_DIR_LOCKED)

#endif