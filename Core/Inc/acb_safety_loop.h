#ifndef ACB_SAFETY_LOOP_H
#define ACB_SAFETY_LOOP_H



typedef enum LOOP_STATUS{
	SAFETY_LOOP_OPEN = 0,
	SAFETY_LOOP_CLOSED = 1
}loop_status_t;


loop_status_t check_safety_loop();


#endif
