#include "thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define LIB "./librpi.so"

void* led_thread(void* arg)
{
	void *handle = NULL;
	void (*led_func)(char *);
	char* err_msg;

	handle = dlopen(LIB, RTLD_LAZY | RTLD_NODELETE);
	if (handle == NULL) {
		goto err;
	}

	dlerror();

	led_func = (void (*)(char *))dlsym(handle, "led_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		goto err;
	}

	led_func((char *)arg);

	dlclose(handle);

	free(arg);
	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	free(arg);
	return NULL;
}

void* buzzer_thread(void* arg)
{
	void *handle = NULL;
	void (*buzzer_func)(char *);
	char* err_msg;

	handle = dlopen(LIB, RTLD_LAZY | RTLD_NODELETE);
	if (handle == NULL) {
		goto err;
	}

	dlerror();

	buzzer_func = (void (*)(char *))dlsym(handle, "buzzer_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		goto err;
	}

	buzzer_func((char *)arg);

	dlclose(handle);

	free(arg);
	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	free(arg);
	return NULL;
}

void* cds_thread(void* arg)
{
	void *handle = NULL;
	void (*cds_func)(int *);
	char* err_msg;

	handle = dlopen(LIB, RTLD_LAZY | RTLD_NODELETE);
	if (!handle) {
		goto err;
	}

	dlerror();

	cds_func = (void (*)(int *))dlsym(handle, "cds_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		goto err;
	}

	cds_func((int *)arg);

	dlclose(handle);

	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	return NULL;
}

void* segment_thread(void* arg)
{
	void *handle = NULL;
	void (*segment_func)(int *);
	char* err_msg;

	handle = dlopen(LIB, RTLD_LAZY | RTLD_NODELETE);
	if (!handle) {
		goto err;
	}

	dlerror();

	segment_func = (void (*)(int *))dlsym(handle, "segment_function");

	err_msg = dlerror();

	if (err_msg != NULL) {
		goto err;
	}

	segment_func((int *)arg);

	dlclose(handle);

	free(arg);
	return NULL;
err:
	if (handle) {
		dlclose(handle);
	}

	free(arg);
	return NULL;
}