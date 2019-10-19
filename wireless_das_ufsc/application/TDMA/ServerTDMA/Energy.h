/*
 * Energy.h
 *
 * Created: 27/09/2014 17:10:28
 *  Author: nando
 */ 


#ifndef ENERGY_H_
#define ENERGY_H_

#include <stdbool.h>
#include <stdint.h>
#include "Solver.h"
#include "nwk.h"

#define			COLLAB_ALG_TURN			4

COMPILER_PACK_SET(1)

typedef struct  EnergyStatistics_t
{
	uint8_t		address;
	uint8_t		rssi;
	uint64_t	n_recv_msg;
	uint8_t		n_recv_msg_turn;
	uint64_t	n_collab_msg;
	uint8_t		n_collab_msg_turn;
	bool		reach_coord;
} EnergyStatistics_t;

COMPILER_PACK_RESET()

void energy_init(void);
void energy_receive_statistics(NWK_DataInd_t *ind);
void energy_prepare_next_turn(void);
void energy_get_collab_vector(uint8_t* vector);
uint8_t energy_get_connected_vector(uint8_t* vector);
EnergyStatistics_t* energy_get_statistics(uint8_t index);

#endif /* ENERGY_H_ */