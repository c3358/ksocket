#ifndef _KS_RPCPROTO_H_
#define _KS_RPCPROTO_H_


#pragma pack(1)

struct ks_rpcproto
{
	uint16_t magic;
	uint32_t id;
	uint64_t sessionid;
	uint32_t datalength;
};


enum ks_rpcstatus
{
	KS_RPCSTATUS_SUCCESS,
	KS_RPCSTATUS_FAILED,
	KS_RPCSTATUS_CONNECTION_GONE,
	KS_RPCSTATUS_PACKET_ERROR,

	KS_RPCSTATUS_USER = 0x10000
};

#pragma pack()


#endif