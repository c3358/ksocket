#ifndef _KS_RPCPROTO_H_
#define _KS_RPCPROTO_H_


#pragma pack(1)

#define KS_RPCHEADER_MAGIC  0x1740
#define KS_RPC_DATA_MAX_LENGTH 0x10000

struct ks_rpcheader
{
	uint16_t magic;
	uint32_t id;
	uint64_t sessionid;
	uint32_t length;
};


enum ks_rpcstatus
{
	KS_RPCSTATUS_SUCCESS,
	KS_RPCSTATUS_FAILED,

	KS_RPCSTATUS_USER = 0x10000
};

#pragma pack()


#endif