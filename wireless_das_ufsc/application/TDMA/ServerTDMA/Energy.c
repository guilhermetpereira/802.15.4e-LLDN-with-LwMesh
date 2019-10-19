/*
 * Energy.c
 *
 * Created: 27/09/2014 17:10:18
 *  Author: nando
 */ 

#include <math.h>
#include <string.h>

#include "Energy.h"
#include "Solver.h"

static uint8_t					n_collab					= 0;
static int8_t					SamLoss						= 0;				// N�mero de Amostras Perdidas.
static float					alpha						= 0.2;
static float					betha						= 0.2;
static float					ganho						= 2.0;
static float					EstLoss						= 0.0;
static float					DevLoss						= 0.0;
static EnergyStatistics_t		motes						[N_MOTES_COLLAB_MAX];
static EnergyStatistics_t		motes_aux					[N_MOTES_COLLAB_MAX];
static uint8_t					motes_connected_n			= 0;
static uint8_t					motes_connected_vector		[N_MOTES_COLLAB_MAX];
static uint8_t					collab_vector				[N_COLLAB_VECTOR];

								// Add 1 byte to size,
								// Add 1 byte to count,
								// Mul by 2 (actual list, next list)

// Rotina de ordena��o do algoritmo quicksort. � chamado recursivamente at�
// ordenar o vetor de dados.
static int cmp_EnergyStatistics_t_func(const void * a, const void * b)
{
	// Se a difer�n�a da pot�ncia do sinal for maior que 10% [25 de 256] ordenar
	// ignorando a taxa de transmiss�o.
	//
	// Retorno:
	// value_a > value_b		>=1
	// value_a = value_b		0
	// value_a < value_b		<=-1

	EnergyStatistics_t*			value_a						= (EnergyStatistics_t*) a;
	EnergyStatistics_t*			value_b						= (EnergyStatistics_t*) b;
	
	int							diff						= (value_b->rssi - value_a->rssi);

	// Condi��o de Ordena��o.
	// Se a diferen�a absoluta for >= 25, significa que independente das transmiss�es � necess�rio
	// ordenar pela qualidade de sinal de transmiss�o, ou seja, n�o utilizar nodos com LQI baixo como colaboradores.
	if(abs(diff) >= 25)
	{
		return(diff);
	}

	// Condi��o de Ordena��o.
	// Se o nodo A e B alcan�am pelo menos n% das vezes o coordenador, ent�o pode-se utilizar a regra de mensagens.
	// Isto � de suma import�ncia para garantir que os nodos que nunca alcan�am o coordenador, ou est�o com interfer�ncia,
	// n�o sejam indicados como colaboradores. Assim, somente quem tem um certo n�vel de participa��o pode ser colaborador.
	// Exemplo, Acabou a bateria de um nodo. O n�mero de colabora��o dele pode ser 0. Sem esta regra este nodo passaria a
	// ser o colaborador e nunca colaboraria com o sistema.
	if(value_a->reach_coord && value_b->reach_coord)
	{	
		// Caso contr�rio (pouca diferen�a entre o LQI dos nodos), organizar nodos cooperantes da seginte forma:
		// Selecionar o nodo cooperante pelo n�mero de mensagens transmitidas alternando (rotacionar) os cooperantes.
		// Neste caso, a ordena��o ser� decrescente (quem transmitiu menos ser� o primeiro da lista)
		// Isto visa economizar bateria por alguns ciclos conforme as condi��es da rede.
		int64_t					diff_collab					= (value_a->n_collab_msg - value_b->n_collab_msg);

		if(diff_collab > 0)									// A transmitiu > que B
			return(-1);										// Indica que A < B para reordenar
		else if(diff_collab < 0)							// A transmitiu < que B
			return(1);										// Indica que A > B para reordenar
		else
			return(0);										// A transmitiu == B, n�o importa
	}
	// Se apenas o nodo A alcan�a o coordenador, deixar ele antes de B
	else if(value_a->reach_coord)
	{
		return(1);
	}
	// Se apenas o nodo B alcan�a o coordenador, deixar ele antes de A
	else if(value_b->reach_coord)
	{
		return(-1);
	}
	// Se nenhuma alcan�a o coordenador, n�o deixar como esta
	else
	{
		return(0);
	}
}
static void energy_Calc_NumMotes(void)
{
	// taxa de perda
//	SamLoss								= N_MOTES_COLLAB_MAX - solver_get_n_received();	// Exclude Coordinator Node
	SamLoss								= motes_connected_n - solver_get_n_received();	// Exclude Coordinator Node
	if(SamLoss < 0)
	{
		SamLoss							= 0;
	}

	//vectortaxaperda.recordWithTimestamp(simTime(),SamLoss);
	//
	//if (alpha > limiaralpha)
	//alpha							= alpha - 0.1;
	//
	//if (betha > limiarbetha)
	//betha							= betha - 0.1;

	EstLoss								= (1.0 - alpha) * EstLoss + alpha * SamLoss;
	DevLoss								= (1.0 - betha) * DevLoss + betha * abs(SamLoss - EstLoss);
	n_collab							= ceil(ganho * EstLoss + DevLoss);
	
	if(n_collab > N_MOTES_COLLAB_MAX)
	{
		n_collab						= N_MOTES_COLLAB_MAX;
	}
}
static void energy_check_each_coordinator(void)
{
	for (int i = 0; i <= N_MOTES_COLLAB_MAX; ++i)
	{
		// 50 %
		motes[i].reach_coord			= ((motes[i].n_recv_msg_turn + motes[i].n_collab_msg_turn) >= COLLAB_ALG_TURN) ? true : false;
		
		// Reset to calc again after next COLLAB_ALG_TURN.
		motes[i].n_recv_msg_turn		= 0;
		motes[i].n_collab_msg_turn		= 0;
	}
}
// M�todo do PAN
static void energy_generate_collab_vector(void)
{
	// L�gica para escolher os nodos cooperantes.
	// Utilizar SNR m�dio (m�dia ou (pior+melhor)/n� de nodos).
	// Verifica��o simples, melhorar depois. Est� pegando os nodos cooperantes com SNR acima da m�dia, mas apenas
	// os N primeiros da lista, sendo N= n� de nodos cooperantes.
	// O n�mero m�ximo de nodos cooperantes est� limitado a 20, pela classe Ieee802154BeaconFrame.msg
	// na propriedade listanodoscooperantes

	uint32_t snrmedio					= 0.0;
	memcpy(motes_aux, motes, sizeof(motes));		// Copia toda a estrutura motes para motes_aux

	// Calcula o SNR m�dio para ponto de corte...
	for (int i = 0; i <= N_MOTES_COLLAB_MAX; ++i)
	{
		// Estabelece um SNR bem baixo (23) para os nodos que n�o tiveram sucesso ao enviar msg para o PAN (PAN n�o recebeu!).
		if(motes_aux[i].rssi == 0)
		{
			motes_aux[i].rssi			= 23;
		}
		snrmedio						+= motes_aux[i].rssi;
	}
	snrmedio							/= N_MOTES_COLLAB_MAX;
	
	// For�a um valor m�nimo de SNR para nodos colaboradores. Isto remove os nodos com baixa taxa de sucesso.
	if(snrmedio <= 23)
	{
		snrmedio						= 25;
	}
	
	// Ordena motes_aux da MAIOR pot�ncia para a MENOR
	qsort(motes_aux, N_MOTES_COLLAB_MAX, sizeof(EnergyStatistics_t), cmp_EnergyStatistics_t_func);

	// Diversidade temporal
	// 1� Copia o next vector para o first
	// 2� Cria next vector com no m�ximo n_collab

	// 1�
	uint8_t			n_collab_next_index	= collab_vector[0] + 2;						// Header (number os collabs + count == 2)
	memcpy(collab_vector, collab_vector + n_collab_next_index, collab_vector[n_collab_next_index] + 2);
	n_collab_next_index					= collab_vector[0] + 2;

	// 2�
	uint8_t			n_collab_cycle		= 0;
	for(uint8_t i = 0; i < n_collab; ++i)
	{
		// Verifica se os poss�veis nodos colaborantes tenham condi��es de atender. Se a rede esta muito ruim
		// os nodos devem ser ignorados independente da ordena��o gerada. 
		if(motes_aux[i].rssi < snrmedio)
		{
			continue;
		}

		collab_vector[n_collab_next_index + n_collab_cycle + 2]	= motes_aux[n_collab_cycle].address;
		++n_collab_cycle;
	}
	collab_vector[n_collab_next_index]	= n_collab_cycle;
	collab_vector[n_collab_next_index + 1] = COLLAB_ALG_TURN;
}
void energy_init(void)
{
	n_collab					= 0;
	motes_connected_n			= 0;
	for(uint8_t i = 0; i < N_MOTES_COLLAB_MAX; ++i)
	{
		motes[i].address				= i + 1;	// 0 is coordinator
		motes[i].rssi					= 0;
		motes[i].n_recv_msg				= 0;
		motes[i].n_collab_msg			= 0;
		motes[i].n_recv_msg_turn		= 0;
		motes[i].n_collab_msg_turn		= 0;
		motes[i].reach_coord			= 0;
		
		motes_connected_vector[i]		= 0;
	}
	memset(collab_vector, 0x00, N_COLLAB_VECTOR);
}
void energy_receive_statistics(NWK_DataInd_t *ind)
{
	// Sanity check!
	if(ind->srcAddr == 0 || ind->srcAddr >= N_MOTES_MAX)
		return;

	AppMessageFrame_t*	frame_struct= (AppMessageFrame_t*) ind->data;

	motes[ind->srcAddr - 1].rssi	= ind->rssi;

	if(frame_struct->frameType == MSG_STATE_DATA)
	{
		motes[ind->srcAddr - 1].n_recv_msg++;
		motes[ind->srcAddr - 1].n_recv_msg_turn++;
	}
#if APP_COORDINATOR
	else if(frame_struct->frameType == MSG_STATE_ENCODED_DATA)
	{
		motes[ind->srcAddr - 1].n_collab_msg++;
		motes[ind->srcAddr - 1].n_collab_msg_turn++;
	}
	else if(frame_struct->frameType == MSG_STATE_CONNECTION)
	{
		if(motes_connected_vector[ind->srcAddr - 1] == 0)
		{
			motes_connected_vector[ind->srcAddr - 1]	= 1;
			motes_connected_n++;
		}
	}
#endif
}
void energy_prepare_next_turn(void)
{
#if APP_ENDDEVICE
	return;
#endif
	
	energy_Calc_NumMotes();
	if(collab_vector[1] > 0)
	{
		collab_vector[1]--;
	}

	if(collab_vector[1] == 0)
	{
		energy_check_each_coordinator();
		energy_generate_collab_vector();
	}

	for(uint8_t i = 0; i < N_MOTES_COLLAB_MAX; ++i)
	{
		motes[i].rssi			= 0;
	}
}
void energy_get_collab_vector(uint8_t* vector)
{
	memcpy(vector, collab_vector, N_COLLAB_VECTOR);
}
uint8_t energy_get_connected_vector(uint8_t* vector)
{
	memcpy(vector, motes_connected_vector, N_MOTES_COLLAB_MAX);

	return(motes_connected_n);
}
EnergyStatistics_t* energy_get_statistics(uint8_t index)
{
	return(&motes[index]);
}