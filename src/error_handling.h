#ifndef __OPENUASL_COMMBD_ERROR_HANDLING_H__
#define __OPENUASL_COMMBD_ERROR_HANDLING_H__

typedef struct _Error_t{
	int module_number;
	char* module_name;
	int error_number;
	char* error_string;
}Error_t;

typedef struct _ErrorDef_t{
	char* module_name;
	int number_of_ed;
	char** error_strings;

}ErrorDef_t;

void ERRH_init_error_def(ErrorDef_t* ed, int );



void ERRH_register_module(int mod_num, ErrorDef_t* ed);

void ERRH_push_error(int mod, int err);

void ERRH_print_error();



#endif /* __OPENUASL_COMMBD_ERROR_HANDLING_H__ */
