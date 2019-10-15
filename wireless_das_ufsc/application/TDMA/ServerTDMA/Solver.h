/*
 * Solver.h
 *
 * Created: 22/09/2014 19:22:11
 *  Author: nando
 */ 


#ifndef SOLVER_H_
#define SOLVER_H_

#include <stdint.h>
#include <stdbool.h>
#include "nwk.h"

#define MSG_SIZE_MAX					64
#define N_MOTES_MAX						15						// Número de nodos maximo (incluindo o coordenador).
#define N_MOTES_COLLAB_MAX				N_MOTES_MAX - 1
#define N_COLLAB_VECTOR					((N_MOTES_COLLAB_MAX + 2) * 2)
#define BROADCAST						0xFFFF
#define COEFICIENT_BASE					0x30

COMPILER_PACK_SET(1)
typedef enum MsgState_t {
	MSG_STATE_BEACON,
	MSG_STATE_DATA,
	MSG_STATE_ENCODED_DATA,
	MSG_STATE_CONNECTION,
} MsgState_t;

typedef enum SolverMsgType_t {
	SOLVER_MSG_NONE,
	SOLVER_MSG_RECEIVED,
	SOLVER_MSG_DECODED,
	SOLVER_MSG_MAX,
} SolverMsgType_t;

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND_PREPARE,
	APP_STATE_SEND,
	APP_STATE_SEND_COLLAB,
	APP_STATE_SEND_BUSY_DATA,
	APP_STATE_SEND_BUSY_COLLAB,
	APP_STATE_SLEEP_PREPARE,
	APP_STATE_SLEEP,
	APP_STATE_WAKEUP_AND_WAIT,
	APP_STATE_WAKEUP_AND_COLLAB,
	APP_STATE_WAKEUP_AND_SEND,
	APP_STATE_WAKEUP_AND_SEND_COLLAB,
	APP_STATE_RECEIVE_COLLAB,
	APP_STATE_DO_COMPRESS,
	APP_STATE_SERVER_STATISTICS,
} AppState_t;

typedef struct  AppMessageBeacon_t
{
	uint8_t		collab_vector[N_COLLAB_VECTOR];
} AppMessageBeacon_t;

typedef struct  AppMessageData_t
{
	uint8_t		data_vector[MSG_SIZE_MAX];
} AppMessageData_t;

typedef struct  AppMessageCollab_t
{
	uint8_t		coefficients[N_MOTES_MAX];
	uint8_t		data_vector[MSG_SIZE_MAX];
} AppMessageCollab_t;

typedef struct  AppMessageFrame_t
{
	MsgState_t	frameType;
	union
	{
		AppMessageBeacon_t	beacon;
		AppMessageData_t	data;
		AppMessageCollab_t	collab;
	};
} AppMessageFrame_t;

COMPILER_PACK_RESET()

void solver_init(void);
void solver_set_collab_device(uint8_t slot);
uint8_t solver_get_n_received(void);
uint8_t solver_get_n_colaborative(void);
void solver_received_data_frame(NWK_DataInd_t *ind);

#if APP_COORDINATOR
	void solver_prepare_next_turn(void);
	int solver_solve_system(void);
	uint8_t* solver_get_data(uint8_t index, SolverMsgType_t msgType);
#else
	void solver_encode_messages(AppMessageFrame_t* frame);
	void solver_set_data_frame(uint8_t address, AppMessageFrame_t *frame_struct);
#endif

#endif /* SOLVER_H_ */