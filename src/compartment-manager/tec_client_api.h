#ifndef TEC_CLIENT_API_H
#define TEC_CLIENT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>



#define TEC_CONFIG_COMPARTMENTS_REF_COUNT 4

#define TEC_NONE                   0x00000000
#define TEC_VALUE_INPUT            0x00000001
#define TEC_VALUE_OUTPUT           0x00000002
#define TEC_VALUE_INOUT            0x00000003

#define TEC_SUCCESS                0x00000000
#define TEC_ERROR_GENERIC          0xFFFF0000
#define TEC_ERROR_ACCESS_DENIED    0xFFFF0001
#define TEC_ERROR_CANCEL           0xFFFF0002
#define TEC_ERROR_ACCESS_CONFLICT  0xFFFF0003
#define TEC_ERROR_EXCESS_DATA      0xFFFF0004
#define TEC_ERROR_BAD_FORMAT       0xFFFF0005
#define TEC_ERROR_BAD_PARAMETERS   0xFFFF0006
#define TEC_ERROR_BAD_STATE        0xFFFF0007
#define TEC_ERROR_ITEM_NOT_FOUND   0xFFFF0008
#define TEC_ERROR_NOT_IMPLEMENTED  0xFFFF0009
#define TEC_ERROR_NOT_SUPPORTED    0xFFFF000A
#define TEC_ERROR_NO_DATA          0xFFFF000B
#define TEC_ERROR_OUT_OF_MEMORY    0xFFFF000C
#define TEC_ERROR_BUSY             0xFFFF000D
#define TEC_ERROR_COMMUNICATION    0xFFFF000E
#define TEC_ERROR_SECURITY         0xFFFF000F
#define TEC_ERROR_SHORT_BUFFER     0xFFFF0010
#define TEC_ERROR_EXTERNAL_CANCEL  0xFFFF0011
#define TEC_ERROR_TARGET_DEAD      0xFFFF3024

#define TEC_LOGIN_PUBLIC       0x00000000
#define TEC_LOGIN_USER         0x00000001
#define TEC_LOGIN_GROUP        0x00000002
#define TEC_LOGIN_APPLICATION  0x00000004
#define TEC_LOGIN_USER_APPLICATION  0x00000005
#define TEC_LOGIN_GROUP_APPLICATION  0x00000006

typedef size_t CompartmentId;

typedef uint32_t TEC_Result;

/**
 * struct TEEC_Context - Represents a connection between a client application
 * and a TEC.
 */
typedef struct {
	/* Implementation defined */
	int fd;
	bool reg_mem;
} TEC_Context;

/**
 * This type contains a Universally Unique Resource Identifier (UUID) type as
 * defined in RFC4122. These UUID values are used to identify Trusted
 * Applications.
 */
typedef struct {
	uint32_t timeLow;
	uint16_t timeMid;
	uint16_t timeHiAndVersion;
	uint8_t clockSeqAndNode[8];
} TEC_UUID;


/**
 * struct TEEC_Session - Represents a connection between a client application
 * and a trusted application.
 */
typedef struct {
	/* Implementation defined */
	TEC_Context *ctx;
	uint32_t session_id;  //equivalent to compartment_id
} TEC_Session;

TEC_Result TEC_InitializeContext(const char *name, TEC_Context *context);

TEC_Result TEC_OpenSession(TEC_Context *context, TEC_Session *session, const TEC_UUID *destination,
			     uint32_t connectionMethod, const void *connectionData, uint32_t *returnOrigin);

TEC_Result TEC_InvokeCommand(TEC_Session *session, uint32_t commandID, uint32_t *param,
			       uint32_t *returnOrigin);

void TEC_CloseSession(TEC_Session *session);

void TEC_FinalizeContext(TEC_Context *context);


#ifdef __cplusplus
}
#endif

#endif
