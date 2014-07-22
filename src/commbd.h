/*
 * protocol_def.h
 *
 *  Created on: 2014. 7. 4.
 *      Author: aroom1
 */

#ifndef PROTOCOL_DEF_H_
#define PROTOCOL_DEF_H_

#define BE_PROTO_AUTH		0x01
#define BE_PROTO_CONTROL		0x02
#define BE_PROTO_SERVINFO	0x03

// buf size = 20
int BE_get_disk_id(char* buf);
int BE_get_uav_id(char* diskid, char* uavid);

#endif /* PROTOCOL_DEF_H_ */
