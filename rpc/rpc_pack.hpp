#ifndef _KS_RPC_PACK_HPP_
#define _KS_RPC_PACK_HPP_


#pragma pack(1)

#define KS_RPCHEADER_MAGIC  0x0EAC
#define KS_RPC_DATA_MAX_LENGTH 0x10000

struct ks_rpcheader
{
	uint16_t magic;
	uint32_t id;
	uint64_t sessionid;
	uint32_t length;
	uint32_t status;
};

enum ks_rpcstatus
{
	KS_RPCSTATUS_SUCCESS,
	KS_RPCSTATUS_FAILED,
	KS_RPCSTATUS_PARSE_PACKET,

	KS_RPCSTATUS_USER = 0x1000
};

#pragma pack()



#endif