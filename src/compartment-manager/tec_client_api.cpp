#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tec_client_api.h"
#include "tec.h"
#include "compartment_interface.h"

#include <archcap.h>

// extern int32_t tec_manager_controller(uint32_t cmd, struct tec_open_session_arg* buf_data);
extern int32_t tec_manager_controller(uint32_t cmd, struct tec_buf_data* buf_data);
extern int32_t tec_manager_open_compartment_session(struct tec_open_session_arg* arg);
extern int32_t tec_manager_compartment_invoke(struct tec_invoke_arg* arg);
extern int32_t tec_manager_close_compartment_session(struct tec_close_session_arg* arg);
// extern int32_t tec_manager_controller(uint32_t cmd, uintcap_t buf_data);



static void uuid_to_octets(uint8_t d[16], const TEC_UUID *s)
{
	d[0] = s->timeLow >> 24;
    std::cout << "d[0] - d[7]" << std::endl;
    std::cout << std::hex << (d[0] >> 4) << (d[0] & 0xf);
	d[1] = s->timeLow >> 16;
    std::cout << std::hex << (d[1] >> 4) << (d[1] & 0xf);
	d[2] = s->timeLow >> 8;
    std::cout << std::hex << (d[2] >> 4) << (d[2] & 0xf);
	d[3] = s->timeLow;
    std::cout << std::hex << (d[3] >> 4) << (d[3] & 0xf);
	d[4] = s->timeMid >> 8;
    std::cout << std::hex << (d[4] >> 4) << (d[4] & 0xf);
	d[5] = s->timeMid;
    std::cout << std::hex << (d[5] >> 4) << (d[5] & 0xf);
	d[6] = s->timeHiAndVersion >> 8;
    std::cout << std::hex << (d[6] >> 4) << (d[6] & 0xf);
	d[7] = s->timeHiAndVersion;
    std::cout << std::hex << (d[7] >> 4) << (d[7] & 0xf) << std::endl;
	memcpy(d + 8, s->clockSeqAndNode, sizeof(s->clockSeqAndNode));
}

TEC_Result TEC_InitializeContext(const char *name, TEC_Context *ctx)
{
//	char devname[PATH_MAX];
	int fd;
	size_t n;

	(void)name;
	if (!ctx)
		return TEC_ERROR_BAD_PARAMETERS;
	
    fd = 1;
    if (fd >= 0) {
      ctx->fd = fd;
      //      ctx->reg_mem = gen_caps & TEE_GEN_CAP_REG_MEM;
      ctx->reg_mem = TEC_GEN_CAP_GP | TEC_GEN_CAP_REG_MEM;
      std::cout << "[TEC Client API] TEC_InitializeContext Success" << ctx->reg_mem << std::endl;
      return TEC_SUCCESS;
    }

	return TEC_ERROR_ITEM_NOT_FOUND;
}

TEC_Result TEC_OpenSession(TEC_Context *ctx, TEC_Session *session, const TEC_UUID *destination, 
            uint32_t connection_method, const void *connection_data, uint32_t *ret_origin)
{
    std::cout << "[TEC Client API] OpenSession Started ... " << std::endl;
    uint32_t buf[(sizeof(struct tec_open_session_arg)) / sizeof(uint32_t)] = { 0 };
    struct tec_open_session_arg* arg;
    struct tec_buf_data buf_data;
    TEC_Result res;
    uint32_t rc;

    buf_data.buf_ptr = (uintptr_t)buf;
	buf_data.buf_len = sizeof(buf);
    // std::cout << "[TEC Client API] CTX-FD " << ctx->fd << std::endl;

    arg = (struct tec_open_session_arg *)buf;

    std::cout << "[TEC Client API] Incoming morelloTower UUID " << destination->timeMid << destination->timeLow << destination->timeHiAndVersion << destination->clockSeqAndNode << std::endl;

    // uuid_to_octets(arg->uuid, destination);
    memcpy(&arg->uuid, destination, sizeof(destination));
    arg->clnt_login = connection_method;

    // std::cout << "[TEC Client API] Arg UUID Login - " << arg->uuid.timeMid << arg->uuid.timeLow << arg->uuid.timeHiAndVersion << arg->uuid.clockSeqAndNode << std::endl;

    // struct tec_buf_data* __capability buf_data_cap = archcap_c_ddc_cast(&buf_data);
    // buf_data_cap = archcap_c_perms_set(buf_data_cap, ARCHCAP_PERM_GLOBAL | ARCHCAP_PERM_LOAD);

    // rc = tec_manager_controller(TEC_MANAGER_OPEN_COMPARTMENT_SESSION, (&buf_data));
    rc = tec_manager_open_compartment_session(arg);
    // rc = tec_manager_controller(TEC_MANAGER_OPEN_COMPARTMENT_SESSION, (buf_data_cap));

    std::cout << "[TEC Client API] Successfully Opened Enclave Session" << std::endl;
    std::cout << "[TEC Client API] Returned Enclave Session ID from E-Manager " << arg->session_original << std::endl;

    res = arg->ret;

    if (res == TEC_SUCCESS) {
        // std::cout << "[TEC Client API] Return Session ID - " <<  arg->session << std::endl;
		session->ctx = ctx;
		session->session_id = arg->session_original; // arg->session;
	}

    // std::cout << "[TEC Client API] Return Session ID - " <<  session->session_id << std::endl;

    return res;
}

TEC_Result TEC_InvokeCommand(TEC_Session *session, uint32_t cmd_id, uint32_t *param, uint32_t *error_origin) {

    std::cout << "[TEC Client API] ECall Input (ID - " << session->session_id << ") " << &param << std::endl;
    
    TEC_Result res;
    struct tec_buf_data buf_data;
    uint32_t buf[(sizeof(struct tec_invoke_arg)) / sizeof(uint32_t)] = { 0 };
    struct tec_invoke_arg* arg;
    uint32_t rc;

    buf_data.buf_ptr = (uintptr_t)buf;
	buf_data.buf_len = sizeof(buf);

    arg = (struct tec_invoke_arg *)buf;

    arg->session = session->session_id;
	arg->func = cmd_id;
    // arg->param = (uint32_t)(size_t)&param;
    memcpy(&arg->param, param, sizeof(arg->param));

    // std::cout << "[TEC Client API] TEC_InvokeCommand  " <<  arg->session << " - " << arg->func << " - " << arg->param << std::endl;

    // rc = tec_manager_controller(TEC_MANAGER_COMPARTMENT_INVOKE, (&buf_data));
    rc = tec_manager_compartment_invoke(arg);
    
    res = arg->ret;
    if (res == TEC_SUCCESS) {
        memcpy(param, &arg->param, sizeof(arg->param));
        std::cout << "[TEC Client API] ECall Invoke Success - Result " << arg->param << std::endl;
		
	}

    return res;

}

void TEC_CloseSession(TEC_Session *session)
{
    struct tec_buf_data buf_data;
    struct tec_close_session_arg arg;

    std::cout << "[TEC Client API] Closing Open Enclave Session " << session->session_id << std::endl;


	if (!session)
		return;

    // buf_data.buf_ptr = (uintptr_t)&arg;
    // buf_data.buf_len = sizeof(arg);

	arg.session = session->session_id;

    // std::cout << "[TEC Client API] arg Session " << arg.session << std::endl;

	// if (tec_manager_controller(TEC_MANAGER_CLOSE_COMPARTMENT_SESSION, &buf_data))
    if (tec_manager_close_compartment_session(&arg))
		printf("[TEC Client API] Failed to close session 0x%x\n", session->session_id);
	else {
		printf("[TEC Client API] Session successfully closed 0x%x\n", session->session_id);
        session->session_id = 0;
	}
}

void TEC_FinalizeContext(TEC_Context *ctx)
{
	if (ctx){
	  ctx->fd = -1;
      std::cout << "[TEC Client API] TEC Context Reset" << std::endl;
    }
}