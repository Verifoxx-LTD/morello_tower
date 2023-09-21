/*
 * Copyright (c) 2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <string>
#include <vector>
#include <sys/queue.h>

#include "compartment_interface.h"
#include "tec_client_api.h"

#define TEC_SUCCESS                0x00000000
#define TEC_ERROR_GENERIC          0xFFFF0000

typedef uint32_t TEC_Result;

// typedef struct {
// 	uint32_t timeLow;
// 	uint16_t timeMid;
// 	uint16_t timeHiAndVersion;
// 	uint8_t clockSeqAndNode[8];
// } TEC_UUID;

struct tec_ta_ctx {
	TEC_UUID uuid;
	// uint32_t flags;		/* TA_FLAGS from TA header */
	TAILQ_ENTRY(tec_ta_ctx) link;
};

struct tec_ta_session {
	TAILQ_ENTRY(tec_ta_session) link;
	TAILQ_ENTRY(tec_ta_session) link_tsd;
	struct tec_ta_ctx *ctx;	/* TA context */
	// void *user_ctx;
};

TAILQ_HEAD(tec_ta_session_head, tec_ta_session);
TAILQ_HEAD(tec_ta_ctx_head, tec_ta_ctx);


void CompartmentManagerInit();

// Add a compartment to the manager and initialize it (run it until main()).
// Arguments:
// - id: compartment ID, must be less than MAX_COMPARTMENTS and not allocated to an existing
//       compartment.
// - path: path to the compartment ELF file
// - args: arguments to pass to the compartment when initializing it
// - memory_range_length: size of the range reserved to the compartment
TEC_Result CompartmentAdd(CompartmentId id, const std::string& path, const std::vector<std::string>& args,
                    size_t memory_range_length);
