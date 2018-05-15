#ifndef __DEFINE_DBGENG_LINKER__
#define __DEFINE_DBGENG_LINKER__

#include <interface.h>

class __declspec(uuid("60BB225D-A06F-11D1-9138-0020AF715AF0")) engine_linker : public dbg::api
{
private:
	void *debug_client_;
	void *debug_data_space_;
	void *debug_data_space_2_;
	void *debug_advanced_;
	void *debug_system_objects_;
	void *debug_control_3_;
	void *debug_register_;
	void *debug_symbol_;

public:
	engine_linker();
	~engine_linker();

	virtual void get_uuid(uuid_type *iid);
	virtual void *get_instance(unsigned long id);

	virtual bool open(char *path);
	virtual bool open(unsigned long pid);

	virtual bool query_virtual_memory(unsigned long long virtual_address, void *out_memory_info);
	virtual unsigned long __stdcall read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size);

	virtual bool __stdcall get_thread_context(cpu_context_type *context);
	virtual bool get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp);
};

#endif