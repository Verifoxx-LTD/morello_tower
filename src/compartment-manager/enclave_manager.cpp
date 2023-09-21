// #include <arm_cmse.h>
#include <filesystem>
#include <stdio.h>
#include <string>
#include <iostream>

/* GP TEE client API */
// #include "tee_types.h"
#include "tec_client_api.h"
#include "tec.h"
// #include "tee/uuid.h"

// #include <kernel/tee_ta_manager.h>
#include "compartment_config.h"
#include "compartment_manager.h"
#include <unistd.h>


#include <archcap.h>

#define SHIFT_U32(v, shift)	((uint32_t)(v) << (shift))

struct tec_ta_session_head tec_open_compartment_sessions = TAILQ_HEAD_INITIALIZER(tec_open_compartment_sessions);
struct tec_ta_ctx_head tec_ctxes = TAILQ_HEAD_INITIALIZER(tec_ctxes);

namespace {

std::string DefaultEnclavePath(const std::string& dirname) {
  //change this here
  return dirname + "compartments/enclave";
}

void Usage(const std::string& progname, const std::string& dirname) {
  std::cout << "Usage: " << progname << " [compartment_path]\n";
  std::cout << "Default compartment paths (if not specified):\n";
  std::cout << "    enclave_path: " << DefaultEnclavePath(dirname) << "\n";
}

}


void tec_uuid_from_octets(TEC_UUID *d, const uint8_t *s)
{
	d->timeLow = SHIFT_U32(s[0], 24) | SHIFT_U32(s[1], 16) |
		     SHIFT_U32(s[2], 8) | s[3];
  std::cout << std::hex << (d->timeLow >> 4) << (d->timeLow & 0xf);
	d->timeMid = SHIFT_U32(s[4], 8) | s[5];
  std::cout << std::hex << (d->timeMid >> 4) << (d->timeMid & 0xf);
	d->timeHiAndVersion = SHIFT_U32(s[6], 8) | s[7];
  std::cout << std::hex << (d->timeHiAndVersion >> 4) << (d->timeHiAndVersion & 0xf);
	memcpy(d->clockSeqAndNode, s + 8, sizeof(d->clockSeqAndNode));
  std::cout << std::hex << (d->clockSeqAndNode) << std::endl;
//   d->uniqueId = 1;
}


static struct tec_ta_session *find_session(uint32_t id, struct tec_ta_session_head *open_compartment_sessions)
{
	struct tec_ta_session *s;

	TAILQ_FOREACH(s, open_compartment_sessions, link) {
    if ((uint32_t)(size_t)s == id) {
		return s;
    }
    }
	return NULL;
}

TEC_Result tec_ta_open_session(struct tec_ta_session **sess, struct tec_ta_session_head *open_compartment_sessions, const TEC_UUID *uuid)
{
  // std::cout << "[E Manager] tec_ta_open_session uuid - " << uuid->timeMid << uuid->timeLow << uuid->timeHiAndVersion << uuid->clockSeqAndNode << std::endl;
  
  TEC_Result res;

  struct tec_ta_session *s = (tec_ta_session*)malloc(sizeof(struct tec_ta_session));
  memset(s,0,sizeof(struct tec_ta_session));
  // TAILQ_FOREACH(s, open_compartment_sessions, link) {
  //   std::cout << "*** !! - " << s << std::endl;
	// }
  
  TAILQ_INSERT_TAIL(open_compartment_sessions, s, link);

  // TAILQ_FOREACH(s, open_compartment_sessions, link) {
  //   std::cout << "*** !! - " << s <<std::endl;
	// }
  // std::cout << sizeof(open_compartment_sessions) << std::endl;

  // std::cout << "TAILQ_INSERT_TAIL - " << s->ctx << std::endl;

  struct tec_ta_ctx *ctx = (tec_ta_ctx*)malloc(sizeof(struct tec_ta_ctx));

  //instead of simply setting uuid, compare with some place and set here
//   if (memcpy(&ctx->uuid, uuid, sizeof(TEC_UUID))) {break;}
  memcpy(&ctx->uuid, uuid, sizeof(TEC_UUID));
  // std::cout << "[E Manager] ctx->uuid  - " << ctx->uuid.timeMid << ctx->uuid.timeLow << ctx->uuid.timeHiAndVersion << ctx->uuid.clockSeqAndNode << std::endl;

  // TAILQ_FOREACH(ctx, &tec_ctxes, link) {
  //   std::cout << "--------- " << ctx << std::endl;
	// }
  // std::cout << ctx->uuid << std::endl;
  
  s->ctx = ctx;
  // std::cout << "[E Manager] s->ctx->uuid - " << s->ctx->uuid.timeMid << s->ctx->uuid.timeLow << s->ctx->uuid.timeHiAndVersion << s->ctx->uuid.clockSeqAndNode << std::endl;

  TAILQ_INSERT_TAIL(&tec_ctxes, ctx, link);

  // TAILQ_FOREACH(ctx, &tec_ctxes, link) {
  //   std::cout << "------- " << ctx << std::endl;
	// }
  *sess = s;

  // if (res == TEC_SUCCESS) {
	// 	*sess = s;
	// } else {
	// 	TAILQ_REMOVE(open_compartment_sessions, s, link);
	// 	free(s);
	// }

  return TEC_SUCCESS;
  

}

// TEC_Result tec_manager_open_compartment_session(struct tec_buf_data* buf_data)
TEC_Result tec_manager_open_compartment_session(struct tec_open_session_arg* arg)
{

  TEC_Result res;
  // TEC_ErrorOrigin err;
  struct tec_ta_session *sess = NULL;

  // struct tec_open_session_arg *arg;
  TEC_UUID uuid;

  // arg = (struct tec_open_session_arg *)&buf_data->buf_ptr;
  // arg = reinterpret_cast<struct tec_open_session_arg*>(buf_data->buf_ptr);
  // arg = reinterpret_cast<struct tec_open_session_arg*>(&buf_data->buf_ptr);
  
  // std::cout << "[E Manager] Client Login UUID " << arg->clnt_login << " - Session " << arg->session <<  std::endl;

  // tec_uuid_from_octets(&uuid, arg->uuid);
  memcpy(&uuid, &arg->uuid, sizeof(uuid));

  // std::cout << "[E Manager] morelloTower UUID - " << uuid.timeMid << uuid.timeLow << uuid.timeHiAndVersion << uuid.clockSeqAndNode << std::endl;
  
  // CompartmentManagerInit();

  std::cout << "[E Manager] CompartmentManagerInit Done ... " << std::endl;

  res = tec_ta_open_session(&sess, &tec_open_compartment_sessions, &uuid);

  std::cout << "[E Manager] Post Open Session (morelloTower UUID) " << sess->ctx->uuid.timeMid << sess->ctx->uuid.timeLow << sess->ctx->uuid.timeHiAndVersion << sess->ctx->uuid.clockSeqAndNode << std::endl;
  
  if(res == TEC_SUCCESS){
    // std::cout << "This --------" << (uint32_t)(size_t)sess << " - " << (uint16_t)(size_t)sess << " - " << (uint8_t)(size_t)sess << std::endl;
    // std::cout << "This --------" << (uint32_t)(((uint32_t)(size_t)sess) / 1000000) << std::endl;
    
    arg->session = (uint32_t)(((uint32_t)(size_t)sess) / 1000000);
    arg->session_original = ((uint32_t)(size_t)sess);
  }
  size_t enclaveId = arg->session;
  
  std::cout << "[E Manager] Generated EnclaveId  " << arg->session << std::endl;

  //need to hardcode this path
//   std::string progname{argv[0]};
  
  std::string progname{"/data/nativetest64/morello-tower/"};

  // Get our dirname.
  std::string dirname{progname};
  size_t pos = dirname.find_last_of('/');
  // If there is no '/', then erase all the string (that's good enough for constructing relative
  // paths).
  dirname.erase(pos == std::string::npos ? 0 : pos + 1);
  // std::cout << "[E Manager] dirname" << dirname << std::endl;
  
  std::string enclave_path = DefaultEnclavePath(dirname);
  // std::cout << "[E Manager] enclave_path " << enclave_path << std::endl;

  res = CompartmentAdd(enclaveId, enclave_path, {}, kCompartmentMemoryRangeLength);
  std::cout << "[E Manager] Compartmentalised Enclave Added with eSession ID " << enclaveId << std::endl;

  arg->ret = res;
  // std::cout << "[E Manager] " << arg->session << " - " << arg->clnt_login << " - " << arg->ret << std::endl;
  return res;
 
}

TEC_Result tec_ta_get_session(uint32_t id, struct tec_ta_session_head *open_compartment_sessions, struct tec_ta_session **sess){

  struct tec_ta_session *s;
  s = find_session(id, open_compartment_sessions);

  if(s == NULL){
    return TEC_ERROR_GENERIC;
  }

  *sess = s;
  return TEC_SUCCESS;
}

// TEC_Result tec_manager_compartment_invoke(struct tec_buf_data* buf_data)
TEC_Result tec_manager_compartment_invoke(struct tec_invoke_arg* arg)
{
  TEC_Result res;
  struct tec_ta_session *sess;
  // struct tec_invoke_arg* arg;
  // arg = (struct tec_invoke_arg *)(size_t)buf_data->buf_ptr;
  // arg = reinterpret_cast<struct tec_invoke_arg*>(&buf_data->buf_ptr);

  std::cout << "[E Manager] ECall Invoke ID (" << arg->session << ") - " << arg->func << " - " << arg->param << std::endl;
  res = tec_ta_get_session(arg->session, &tec_open_compartment_sessions, &sess);

  if(res == TEC_SUCCESS){
    size_t compartmentId = (uint32_t)(((uint32_t)(size_t)arg->session) / 1000000); //arg->session;
    std::cout << "[E Manager] (E) Active Session Retrieved: " << compartmentId << std::endl;
    
    uint32_t input_param = arg->param;
    // std::cout << "[E Manager] Input Param: " << input_param << std::endl;
    uint32_t* __capability param_cap = archcap_c_ddc_cast(&input_param);
    param_cap = archcap_c_perms_set(param_cap, ARCHCAP_PERM_GLOBAL | ARCHCAP_PERM_STORE | ARCHCAP_PERM_LOAD);
    
    uintcap_t ret = CompartmentCall(compartmentId, AsUintcap((CCallType)arg->func), AsUintcap(param_cap));
    auto enclave_result = reinterpret_cast<const uint32_t* __capability>(ret);
    std::cout << "[E Manager] Result from ECall : " << input_param << std::endl;

    if(ret == 0){
      arg->param = (uint32_t)(size_t)input_param;
      // std::cout << "[E Manager] Arg->Param : " << arg->param << std::endl;
      arg->ret = TEC_SUCCESS;
      return TEC_SUCCESS;
    }
    else {
      return TEC_ERROR_GENERIC;
    }

    
  }
  else {
    return TEC_ERROR_GENERIC;
  }

}

// TEC_Result tec_ta_close_session(struct tec_ta_session *csess, struct tec_ta_session_head *open_compartment_sessions)
TEC_Result tec_ta_close_session(uint32_t csess, struct tec_ta_session_head *open_compartment_sessions)
{
  struct tec_ta_session *sess;
	struct tec_ta_ctx *ctx;

  if (!csess)
		return TEC_ERROR_ITEM_NOT_FOUND;
  
  // std::cout << "[E Manager] Close Session csess " << csess->ctx->uuid.timeMid << csess->ctx->uuid.timeLow << csess->ctx->uuid.timeHiAndVersion << csess->ctx->uuid.clockSeqAndNode << std::endl;

  // sess = find_session((uint32_t)(size_t)csess, open_compartment_sessions);
  sess = find_session(csess, open_compartment_sessions);
    
  if (!sess)
		return TEC_ERROR_ITEM_NOT_FOUND;
  
  std::cout << "[E Manager] UUID match found - " << sess->ctx->uuid.timeMid << sess->ctx->uuid.timeLow << sess->ctx->uuid.timeHiAndVersion << sess->ctx->uuid.clockSeqAndNode << std::endl;
  
  ctx = sess->ctx;
  // std::cout << "[E Manager] CTX Session UUID " << ctx->uuid.timeMid << ctx->uuid.timeLow << ctx->uuid.timeHiAndVersion << ctx->uuid.clockSeqAndNode << std::endl;

  TAILQ_REMOVE(open_compartment_sessions, sess, link);
  free(sess);
  TAILQ_REMOVE(&tec_ctxes, ctx, link);
  std::cout << "[E Manager] Session successfully closed" << std::endl;
  return TEC_SUCCESS;
}

// TEC_Result tec_manager_close_compartment_session(struct tec_buf_data *buf_data)
TEC_Result tec_manager_close_compartment_session(struct tec_close_session_arg *arg)
{
  TEC_Result res;
  struct tec_ta_session *sess;
  // struct tec_close_session_arg *arg;
  // arg = (struct tec_close_session_arg *)(size_t)buf_data->buf_ptr;

  std::cout << "[E Manager] Enclave Session ID " << arg->session << std::endl;

  // sess = (struct tec_ta_session *)(size_t)arg->session;
  // std::cout << "[E Manager] Session " << sess->ctx->uuid.timeMid << std::endl;

  // res = tec_ta_close_session(sess, &tec_open_compartment_sessions);
  res = tec_ta_close_session(arg->session, &tec_open_compartment_sessions);


  return res;
}

int32_t tec_manager_controller(uint32_t cmd, struct tec_buf_data*  buf_data)
{

  switch (cmd) {
    
    // case TEC_MANAGER_OPEN_COMPARTMENT_SESSION:
    //   std::cout << "[E Manager] tec_manager_controller TEC_MANAGER_OPEN_COMPARTMENT_SESSION" << std::endl;
    //   return tec_manager_open_compartment_session(buf_data);
    
    // case TEC_MANAGER_COMPARTMENT_INVOKE:
    //   std::cout << "tec_manager_controller TEC_MANAGER_COMPARTMENT_INVOKE" << std::endl;
    //   return tec_manager_compartment_invoke(buf_data);

    // case TEC_MANAGER_CLOSE_COMPARTMENT_SESSION:
    //   return tec_manager_close_compartment_session(buf_data);

    default:
      return TEC_ERROR_ITEM_NOT_FOUND;
  }

}