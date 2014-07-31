#ifndef __OPENUASL_COMMBD_UAVCTRL_H__
#define __OPENUASL_COMMBD_UAVCTRL_H__

#define UAVCTRL_SERVER_PORT		12345


// functional key message header
#define CTRL_FKREQ_HEADER		"$F<"
#define CTRL_FKREP_HEADER		"$F>"
#define CTRL_FKERR_HEADER		"$F!"

// survivor information message header
#define CTRL_SIREQ_HEADER		"$S<"
#define CTRL_SIREP_HEADER		"$S>"
#define CTRL_SIERR_HEADER		"$S!"


int CTRL_init(char* ip);
int CTRL_start();
int CTRL_run();
void CTRL_end();



#endif /* __OPENUASL_COMMBD_UAVCTRL_H__ */
