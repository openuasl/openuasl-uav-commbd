#ifndef __OPENUASL_COMMBD_UAVCTRL_H__
#define __OPENUASL_COMMBD_UAVCTRL_H__

#define UAVCTRL_SERVER_PORT		12345

#define UAVCTRL_BUF_SIZE		512

#define CTRL_MWREQ_HEADER		0x90
#define CTRL_MWREP_HEADER		0x91
#define CTRL_FKREQ_HEADER		0x92
#define CTRL_FKREP_HEADER		0x93
#define CTRL_SIREQ_HEADER		0x94
#define CTRL_SIREP_HEADER		0x95

#define CTRL_REP_STOP			0x77

int CTRL_init(char* ip);
int CTRL_start();
int CTRL_run();
void CTRL_end();



#endif /* __OPENUASL_COMMBD_UAVCTRL_H__ */
