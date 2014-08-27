#ifndef __OPENUASL_PROTOCOL_H__
#define __OPENUASL_PROTOCOL_H__

typedef enum _UaslProtocolHeader{
	uasl_proto_auth = 'A',
	uasl_proto_image = 'I',
	uasl_proto_multiwii ='M',
	uasl_proto_special_function = 'F',
	uasl_proto_survivor_information = 'S'
}UaslProtocolHeader;

// criterion is uav.
typedef enum _UaslProtocolType{
	uasl_proto_type_in = '<',
	uasl_proto_type_out = '>',
	uasl_proto_type_err = '!'
}UaslProtocolType;

/* 1 1 1 4 N
 * [header : 1]
 * [type : 1]
 * [command : 1]
 * [payload length : 4]
 * [payload : N]
 *  N = payload_length
 *  if header is multiiwii
 *  then [header:1][payload:N]
 *  */




#endif /* __OPENUASL_PROTOCOL_H__ */
