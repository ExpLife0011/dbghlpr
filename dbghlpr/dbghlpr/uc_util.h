#ifndef __DEFINE_UNICORN_UTIL__
#define __DEFINE_UNICORN_UTIL__

#define _CRT_SECURE_NO_WARNINGS
#include <unicorn/unicorn.h>

#include <interface.h>
#include <Windows.h>
#include <set>

class __declspec(uuid("4840847A-9432-4589-B809-8D782A7301EC")) uc_util : public dbg::util
{
private:
	typedef struct _tag_uc_callback_data
	{
		dbg::api *api;
		dbg::util *util;
		void *user_context;
		analyze_callback_type cb;
	}callback_data;

public:
	uc_util();
	~uc_util();

	virtual void get_uuid(uuid_type *iid);

	virtual bool disasm(unsigned long long address, unsigned char *table, dbg::PROCESSOR_TYPE type, x86_disasm_context_type *context);
	virtual unsigned long long mnemonic_str(void *handle, unsigned long long address, unsigned char *dump, dbg::PROCESSOR_TYPE type, char *output, size_t output_size);

	virtual bool find_reference_value(dbg::api *api_handle, unsigned long long base, unsigned long long size, dbg::PROCESSOR_TYPE type, std::multimap<unsigned long long, unsigned long long> &ref_map);

	virtual bool calc_segment(void *handle, unsigned long long ptr, unsigned long long *base, unsigned long long *end);
	virtual unsigned long trace(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, dbg::util::code &b, bool is_range);

	virtual unsigned long browse(void *handle, unsigned long long ptr, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set, bool is_safe);
	virtual unsigned long run(void *handle, analyze_callback_type cb, void *cb_context, unsigned long long base, unsigned long long end, dbg::PROCESSOR_TYPE type, std::set<unsigned long long> &entry_point_set);

	static void code_callback(uc_engine *uc, uint64_t address, uint32_t size, void *user_data);
	static void rw_unmap_callback(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data);
	static void fetch_unmap_callback(uc_engine *uc, uc_mem_type type, uint64_t address, int size, int64_t value, void *user_data);
};

#endif