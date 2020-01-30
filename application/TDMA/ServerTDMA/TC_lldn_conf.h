/*
 * TC_lldn_conf.h
 *
 * Created: 21/01/2020 17:36:41
 *  Author: guilh
 */ 


#ifndef TC_LLDN_CONF_H_
#define TC_LLDN_CONF_H_

#define GENCLK_SRC_RC1M		6

#define TMR TC1
#define TMR_CHANNEL_ID (0)

typedef void (*tmr_callback_t)(void);

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
} AppState_t;

#endif /* TC_LLDN_CONF_H_ */