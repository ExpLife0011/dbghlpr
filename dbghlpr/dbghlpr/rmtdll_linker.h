#ifndef __DEFINE_RMTDLL_LINKER__
#define __DEFINE_RMTDLL_LINKER__

#include <interface.h>
#include <rmt_data.h>
#include <internal.h>

class __declspec(uuid("395BAF1E-3046-4EFB-A340-97EF9C494B29")) rmtdll_linker : public dbg::api
{
private:
	void *rmt_data_handle_;
	void *rmt_signal_event_;
	void *rmt_return_event_;
	unsigned long *rmt_control_iid_;

	void *process_handle_;
	void *current_thread_handle_;
	unsigned long current_tid_;

public:
	rmtdll_linker();
	~rmtdll_linker();

	virtual void get_uuid(uuid_type *iid);
	virtual void *get_object(unsigned long id);

	virtual bool open(char *path);
	virtual bool open(unsigned long pid);

	virtual bool close();

	virtual bool get_thread_id_list(std::list<unsigned long> &tid_list);
	virtual bool select_thread(unsigned long tid);
	virtual unsigned long current_thread_id();

	virtual bool query_virtual_memory(unsigned long long virtual_address, void *out_memory_info);
	virtual void * virtual_alloc(unsigned long long base, unsigned long size, unsigned long allocation_type, unsigned long protect_type);
	virtual int virtual_free(void *ptr, unsigned long size, unsigned long free_type);

	virtual unsigned long __stdcall read_virtual_memory(unsigned long long virtual_address, unsigned char *out_memory, unsigned long read_size);
	virtual unsigned long __stdcall write_virtual_memory(unsigned long long virtual_address, unsigned char *input_memory, unsigned long write_size);

	virtual bool __stdcall get_thread_context(cpu_context_type *context);
	virtual bool __stdcall set_thread_context(cpu_context_type *context);

	virtual bool get_symbol_name(unsigned long long offset, char *buffer, unsigned long size_of_buffer, unsigned long *size_of_name, unsigned long long *disp);
	virtual bool stack_trace(unsigned long tid, stack_frame_type_ptr stack_frame, unsigned long size_of_stack_frame, unsigned long *stack_count);

	virtual bool write_dump(char *path);

	//
	// 
	//
	virtual bool close_handle(void *handle);

	virtual bool resume(unsigned long tid);
	virtual bool suspend(unsigned long tid);
};

#endif