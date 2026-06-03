/*******************************************************************************
 * File Name    : <thread.c>
 * Description  : <비동기 하드웨어 제어를 위한 워커 스레드 구현부>
 * Author       : <김명준>
 * Date Created : <2026.06.02>
 *
 * History:
 * - <2026.06.02> : 최초 작성 (<김명준>)
 *******************************************************************************/

#include "state.h"
#include "thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define LIB "./librpi.so"

static void* execute_device_function(const char* func_name, void * const arg)
{
	void *handle = NULL;
	void (*device_func)(struct device_state* const);
	char* err_msg;

	handle = dlopen(LIB, RTLD_LAZY | RTLD_NODELETE);
	if (handle == NULL) {
		return NULL;
	}

	dlerror();

	device_func = (void (*)(struct device_state* const))dlsym(handle, func_name);

	err_msg = dlerror();

	if (err_msg != NULL) {
		dlclose(handle);
		return NULL;
	}

	device_func((struct device_state* const)arg);

	dlclose(handle);
	return NULL;
}

void* led_thread(void* arg)
{
	return execute_device_function("led_function", arg);
}

void* buzzer_thread(void* arg)
{
	return execute_device_function("buzzer_function", arg);
}

void* cds_thread(void* arg)
{
	return execute_device_function("cds_function", arg);
}

void* segment_thread(void* arg)
{
	return execute_device_function("segment_function", arg);
}