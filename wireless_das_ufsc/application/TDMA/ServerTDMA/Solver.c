//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <stdint.h>
#include <string.h>
#include "config.h"
#include "Solver.h"

static uint8_t					n_received					= 0;
static SolverMsgType_t			received[N_MOTES_MAX];						// confirma recebimento
static uint8_t					n_colaborative;								// numero de msg de colaboração
static uint8_t					buffer_msg[N_MOTES_MAX][MSG_SIZE_MAX];		// matriz de armazenamento de msg
static uint8_t					n_equations;								// numero de linhas da matriz
static uint8_t					matrix[N_MOTES_MAX][N_MOTES_MAX];			// matriz
static uint8_t					combination[N_MOTES_MAX][MSG_SIZE_MAX];		// mensagens retransmitidas recebidas
#if APP_ENDDEVICE
	static uint8_t				slotNumber					= 0;
#endif

void solver_init(void)
{
#if APP_COORDINATOR
	n_colaborative				= 0;
	n_equations					= 0;
	n_received					= 0;
#else
	slotNumber					= 0;
#endif
}
uint8_t solver_get_n_received(void)
{
	return(n_received);
}
uint8_t solver_get_n_colaborative(void)
{
	return(n_colaborative);
}
/****************************************************************************************
                            FUNÇÕES DE CODIFICAÇÃO A 8 BITS
****************************************************************************************/
//multiplicacao de 2 número num corpo de 8 bits
static uint8_t solver_mult(uint8_t a, uint8_t b)
{
	int i;
	uint16_t result = 0;

	for (i = 0; i < 8; i++)
	{
		if ((a >> i) & 1)
		{
			result ^= ((uint16_t) b) << i;
		}
	}

	for (i = 6; i >= 0; i--)
	{
		if (result & (0x100 << i))
		{
			result ^= (0x1A9 << i);
		}
	}

	return result;
}
#if APP_COORDINATOR
// inverso  do mult
static uint8_t solver_inv(uint8_t a)
{
    int i, j;
    uint16_t result, r[8], p;

    if (a == 0)
		return 0;

    for (i = 0; i < 8; i++)
    {
        r[i] = ((uint16_t) a) << i;

        for (j = 6; j >= 0; j--)
		{
            if (r[i] & (0x100 << j))
			{
				r[i] ^= (0x1A9 << j);
			}
		}
    }

    for (result = 0; result < 0x100; result++)
    {
        p = 0;
        for (i = 0; i < 8; i++)
		{
            if (result & (1 << i))
			{
				p ^= r[i];
			}
		}

        if (p == 1)
			return result;
    }

    return 0;
}
/****************************************************************************************
                        FUNÇÕES PARA DECIFRAR AS MENSAGENS CODIFICADAS
****************************************************************************************/
static void solver_swap_line(uint16_t i1, uint16_t i2)
{
    uint16_t j, k;

    for (j = i1; j <= N_MOTES_MAX; j++)	// Pode ser reduzido para o número de nodos conectados????
    {
        k								= matrix[i1][j];
        matrix[i1][j]					= matrix[i2][j];
        matrix[i2][j]					= k;
    }

    for (j = 0; j < MSG_SIZE_MAX; j++)// Pode ser reduzido para o tamanho da mensagens????
    {
        k								= combination[i1][j];
        combination[i1][j]				= combination[i2][j];
        combination[i2][j]				= k;
    }
}
// parte do escalonamento
static void solver_combine_line(uint8_t c, uint16_t i1, uint16_t i2)
{
    uint16_t j;

    for (j = i1; j <= N_MOTES_MAX; j++)
	{
        matrix[i2][j]					^= solver_mult(c, matrix[i1][j]);
	}

    for (j = 0; j < MSG_SIZE_MAX; j++)
	{
        combination[i2][j]				^= solver_mult(c, combination[i1][j]);
	}
}
// reduz a ordem da matriz, apos a resolucao de uma mensagem
static void solver_clear_column(uint16_t row, uint16_t col)
{
    uint16_t i;

    for (i = row + 1; i < n_equations; i++)
	{
        if (matrix[i][col] > 0)
		{
			solver_combine_line(solver_mult(solver_inv(matrix[row][col]), matrix[i][col]), row, i);
		}
	}
}
//resolve o escalonamento
int solver_solve_system(void)
{
	if(n_colaborative == 0)
		return(0);
		
	n_equations							= n_colaborative;

    // buffer_msg = estrutura: linha: endereco do nodo,coluna: os bytes da msg.

	int8_t i;
    uint8_t j, k, notrec_mote, cont, row, sucesso;
    //Define a listagem de vizinhanca (mensagens recebidas) de determinado nodo cooperante. É uma lista de bits baseada na posicão da transmissão, se o
    //o nodo recebeu a primeira mensagem da tranmissão seta o bit para 1, se não, seta para zero.

    //RETIRA DA MATRIZ OS ELEMENTOS JÁ CONHECIDOS
	//for (j = 0, row = 0; j < N_MOTES_MAX; j++) (NOS TESTES ESTA ASSIM, NO OMNET+ J==2)????
    for (j = 0; j < N_MOTES_MAX; j++)
    {
        if (received[j] != SOLVER_MSG_NONE)
		{
            //EV << "[Conferencia] retirando elemento conhecido da matriz: " << j << endl;
            for (i = 0; i < n_equations; i++)
			{
                if (matrix[i][j] > 0)
                {
                    for (k = 0; k < MSG_SIZE_MAX; k++)
					{
                        combination[i][k]	^= solver_mult(matrix[i][j], buffer_msg[j][k]); //matrix[i][j] = coeficientes: linha = coeficiente, coluna = endereco do nodo.
					}
                    matrix[i][j] = 0;
                }
			}
        }
    }

    //FAZ O ESCALONAMENTO DA MATRIZ
	//for (j = 0, row = 0; j < N_MOTES_MAX; j++) (NOS TESTES ESTA ASSIM, NO OMNET+ J==2)????
    for (j = 0, row = 0; j < N_MOTES_MAX; j++)
    {
        i								= row;
        if (matrix[i][j] > 0)
        {
            solver_clear_column(row, j);
            ++row;
        }
        else
        {
            for (++i; i < n_equations; ++i)
			{
                if (matrix[i][j] > 0)
                {
                    solver_swap_line(row, i);
                    solver_clear_column(row, j);
                    
					++row;
                    break;
                }
			}
        }
    }
	
    //ENCONTRA AS INCÓGNITAS POSSÍVEIS DE DECIFRAR
	sucesso = 0;
    for (i = n_equations - 1; i >= 0; i--)
    {
        cont = 0;
		//for (j = 0; j < N_MESSAGES; j++) (TESTES, OMNET++ j ==2?????)
        for (j = 0; j < N_MOTES_MAX; j++)
		{
            if (matrix[i][j] > 0)
            {
                cont++;
                notrec_mote = j;
            }
		}

        if (cont == 0)
		{
			n_equations--;
		}
        else if (cont == 1)         //RESOLVE A INCÓGNITA ENCONTRADA
        {
            for (k = 0; k < MSG_SIZE_MAX; k++)
			{
				buffer_msg[notrec_mote][k] = solver_mult(solver_inv(matrix[i][notrec_mote]), combination[i][k]);
			}
			
            //EV << "[Conferencia] mensagem decodificada : " << notrec_mote << endl;
            received[notrec_mote] = SOLVER_MSG_DECODED;
            --n_equations;
            ++sucesso;  // Aqui deve-se contabilizar o sucesso na decodificacao das MSG.

            for (i = 0; i < n_equations; i++)
			{
                if (matrix[i][notrec_mote] > 0)
                {
                    for (k = 0; k < MSG_SIZE_MAX; k++)
					{
						combination[i][k] ^= solver_mult(matrix[i][notrec_mote], buffer_msg[notrec_mote][k]);
					}
					
                    matrix[i][notrec_mote] = 0;
                }
			}
        }
        else
		{
			break;
		}
    }

    return sucesso;
}
#else
/****************************************************************************************
                CODIFICAÇÃO E ENVIO DE RETRANSMISSÃO
****************************************************************************************/
//codifica, constroi e transmite mensagens
void solver_encode_messages(AppMessageFrame_t* frame)
{
    // 1° para cada frame de algum vizinho recebido, codificar a msg byte a byte com a funcao mult.
    // Os parametros sao posicao do nodo retransmissor na retransmissao + endereco do vizinho.
    // Cada vizinho tera a sua msg codificada no formato de um vetor de byte.
	//
    // 2° fazer o xor de todas as msgs codificadas.

	memset(frame->collab.coefficients, 0x00, N_MOTES_MAX);
	memset(frame->collab.data_vector, 0x00, MSG_SIZE_MAX);

    uint8_t coef											= COEFICIENT_BASE + slotNumber;
    for (int i = 0; i < N_MOTES_MAX; i++)
    {
	    if (received[i] == SOLVER_MSG_RECEIVED)
	    {
			frame->collab.coefficients[i]					= coef;				// slotNumber == posição de retransmissão,
																				// i == Endereço do Nodo indexado a partir de zero (nodo_1 é 0)
			uint8_t byte;
			for(int j = 0; j < MSG_SIZE_MAX; j++)
			{
				// 1°
				byte										= solver_mult(frame->collab.coefficients[i], buffer_msg[i][j]);

				// 2°
				frame->collab.data_vector[j]				^= byte;
			}

			++coef;
		}
    }
}
void solver_set_collab_device(uint8_t slot)
{
	slotNumber					= slot;
}
#endif

/****************************************************************************************
                IMPLEMENTAÇÃO REAL COM NODOS
****************************************************************************************/
void solver_prepare_next_turn(void)
{
	n_colaborative				= 0;
	n_equations					= 0;
	n_received					= 0;

	memset(received, SOLVER_MSG_NONE, sizeof(received));
	memset(matrix, 0x00, sizeof(matrix));
	memset(combination, 0x00, sizeof(combination));
}

/*
 * Para cada frame recebido, deve-se copiar os dados em buffer_msg e sinalizar em received
 * qual o endereço do nodos que enviou os dados. Endereços são:
 *	0 - Coordenador
 *	1~N - Nodos
 */
void solver_received_data_frame(NWK_DataInd_t *ind)
{
	// Sanity check!
	if(ind->srcAddr == 0 || ind->srcAddr >= N_MOTES_MAX)
		return;

	AppMessageFrame_t*	frame_struct	= (AppMessageFrame_t*) ind->data;

	if(frame_struct->frameType == MSG_STATE_DATA)
	{
#if APP_COORDINATOR
		//if(ind->srcAddr == 1)
		//{
			++n_received;
			received[ind->srcAddr - 1]	= SOLVER_MSG_RECEIVED;
			memcpy(buffer_msg[ind->srcAddr - 1], frame_struct->data.data_vector, MSG_SIZE_MAX);			
		//}
#else
		++n_received;
		received[ind->srcAddr - 1]		= SOLVER_MSG_RECEIVED;
		memcpy(buffer_msg[ind->srcAddr - 1], frame_struct->data.data_vector, MSG_SIZE_MAX);
#endif
	}
#if APP_COORDINATOR
	else if(frame_struct->frameType == MSG_STATE_ENCODED_DATA)
	{
		// Montar a matrix de coeficientes, a cada retransmissao e uma linha.
		// Montar o combination1 a cada retransmissao e uma linha. (composicao: payload msg codificadas).
		// Deve-se chamar Codificador::solve_system() a cada recebimento de retranmissao, ja que, o sistema vai tentando verificar
		// se pode ou nao extrair uma das incognitas.

		memcpy(matrix[n_colaborative], frame_struct->collab.coefficients, N_MOTES_MAX);      
		memcpy(combination[n_colaborative], frame_struct->collab.data_vector, MSG_SIZE_MAX);
		++n_colaborative;
	}
#endif
}
void solver_set_data_frame(uint8_t address, AppMessageFrame_t *frame_struct)
{
	++n_received;
	received[address - 1]				= SOLVER_MSG_RECEIVED;
	memcpy(buffer_msg[address - 1], frame_struct->data.data_vector, MSG_SIZE_MAX);	
}
#if APP_COORDINATOR
uint8_t* solver_get_data(uint8_t index, SolverMsgType_t msgType)
{
	if(index < N_MOTES_MAX && received[index] == msgType)
	{
		return(buffer_msg[index]);
	}
	else
	{
		return(NULL);
	}
}
#endif