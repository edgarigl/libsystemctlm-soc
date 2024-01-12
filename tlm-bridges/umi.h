#ifndef TLM_BRIDGES_UMI_H__
#define TLM_BRIDGES_UMI_H__

// ref: umi/rtl/umi_messages.vh
enum UMI_CMD {
    UMI_INVALID = 0x00,

    // Requests (host -> device)
    UMI_REQ_READ = 0x01,
    UMI_REQ_WRITE = 0x03,
    UMI_REQ_POSTED = 0x05,
    UMI_REQ_RDMA = 0x07,
    UMI_REQ_ATOMIC = 0x09,
    UMI_REQ_USER0 = 0x0B,
    UMI_REQ_FUTURE0 = 0x0D,
    UMI_REQ_ERROR = 0x0F,
    UMI_REQ_LINK = 0x2F,

    // Response (device -> host)
    UMI_RESP_READ = 0x02,
    UMI_RESP_WRITE = 0x04,
    UMI_RESP_USER0 = 0x06,
    UMI_RESP_USER1 = 0x08,
    UMI_RESP_FUTURE0 = 0x0A,
    UMI_RESP_FUTURE1 = 0x0C,
    UMI_RESP_LINK = 0x0E
};

#define UMI_MAP_OPC_STR(opc) case UMI_ ## opc: return #opc
static inline const char *umi_opc_str(unsigned int opc)
{
    switch (opc) {
        UMI_MAP_OPC_STR(REQ_READ);
        UMI_MAP_OPC_STR(REQ_WRITE);
        UMI_MAP_OPC_STR(REQ_POSTED);
        UMI_MAP_OPC_STR(RESP_READ);
        UMI_MAP_OPC_STR(RESP_WRITE);
        default: return "INVALID";
    };
};

enum UMI_ATOMIC {
    UMI_REQ_ATOMICADD = 0x00,
    UMI_REQ_ATOMICAND = 0x01,
    UMI_REQ_ATOMICOR = 0x02,
    UMI_REQ_ATOMICXOR = 0x03,
    UMI_REQ_ATOMICMAX = 0x04,
    UMI_REQ_ATOMICMIN = 0x05,
    UMI_REQ_ATOMICMAXU = 0x06,
    UMI_REQ_ATOMICMINU = 0x07,
    UMI_REQ_ATOMICSWAP = 0x08
};

#define UMI_RX_PORT(prefix, DW)                        \
        sc_in<bool> prefix ## _valid;                  \
        sc_in<sc_bv<32> > prefix ## _cmd;              \
        sc_in<sc_bv<64> > prefix ## _srcaddr;          \
        sc_in<sc_bv<64> > prefix ## _dstaddr;          \
        sc_in<sc_bv<DW> > prefix ## _data;             \
        sc_out<bool> prefix ## _ready

#define UMI_TX_PORT(prefix, DW)                        \
        sc_out<bool> prefix ## _valid;                 \
        sc_out<sc_bv<32> > prefix ## _cmd;             \
        sc_out<sc_bv<64> > prefix ## _srcaddr;         \
        sc_out<sc_bv<64> > prefix ## _dstaddr;         \
        sc_out<sc_bv<DW> > prefix ## _data;            \
        sc_in<bool> prefix ## _ready

#define UMI_PORT_NAME(prefix)                          \
        prefix ## _valid(#prefix "_valid"),            \
        prefix ## _cmd(#prefix "_cmd"),                \
        prefix ## _srcaddr(#prefix "_srcaddr"),        \
        prefix ## _dstaddr(#prefix "_dstaddr"),        \
        prefix ## _data(#prefix "_data"),              \
        prefix ## _ready(#prefix "_ready")


#define UMI_DECLARE_FIELD(name, pos, size) \
    unsigned int name : size;              \
    enum { UMI_FIELD_ ## name ## _pos = pos, UMI_FIELD_ ## name ## _size = size }

#define UMI_SET_FIELD(cmd, name, val)   \
    setf(cmd, val, UMI_FIELD_ ## name ## _pos, UMI_FIELD_ ## name ## _size)

#define UMI_GET_FIELD(cmd, name)   \
    getf(cmd, UMI_FIELD_ ## name ## _pos, UMI_FIELD_ ## name ## _size)

class umi_fields {
    public:
    UMI_DECLARE_FIELD(opc,     0, 5);
    UMI_DECLARE_FIELD(size,    5, 3);
    UMI_DECLARE_FIELD(len,     8, 8);
    UMI_DECLARE_FIELD(atype,   8, 8);
    UMI_DECLARE_FIELD(qos,    16, 4);
    UMI_DECLARE_FIELD(prot,   20, 2);
    UMI_DECLARE_FIELD(eom,    22, 1);
    UMI_DECLARE_FIELD(eof,    23, 1);
    UMI_DECLARE_FIELD(ex,     24, 1);

    umi_fields(void) :
        opc(0),
        size(0),
        len(0),
        atype(0),
        qos(0),
        prot(0),
        eom(0),
        eof(0),
        ex(0)
        { }

    void setf(uint32_t &cmd, unsigned int v, unsigned int pos, unsigned int size) {
        uint64_t mask;

        mask = (1ULL << size) - 1;
        assert((v & (~mask)) == 0);
        mask <<= pos;
        cmd &= ~mask;
        cmd |= v << pos;
        //printf("setf cmd=%x v=%x pos=%d mask=%lx\n", cmd, v, pos, mask);
    }

    unsigned int getf(uint32_t cmd, unsigned int pos, unsigned int size) {
        uint64_t mask;

        mask = (1ULL << size) - 1;
        cmd >>= pos;
        cmd &= mask;
        //printf("getf field=%x pos=%d size=%d mask=%lx\n", cmd, pos, size, mask);
        return cmd;
    }

    void unpack(uint32_t cmd) {
        opc = UMI_GET_FIELD(cmd, opc);
        size = UMI_GET_FIELD(cmd, size);
        len = UMI_GET_FIELD(cmd, len);
        qos = UMI_GET_FIELD(cmd, qos);
        prot = UMI_GET_FIELD(cmd, prot);
        eom = UMI_GET_FIELD(cmd, eom);
        eof = UMI_GET_FIELD(cmd, eof);
        ex = UMI_GET_FIELD(cmd, ex);
    }
    uint32_t pack() {
        uint32_t cmd;

        cmd = 0;
        UMI_SET_FIELD(cmd, opc, opc);
        UMI_SET_FIELD(cmd, size, size);
        UMI_SET_FIELD(cmd, len, len);
        UMI_SET_FIELD(cmd, qos, qos);
        UMI_SET_FIELD(cmd, prot, prot);
        UMI_SET_FIELD(cmd, eom, eom);
        UMI_SET_FIELD(cmd, eof, eof);
        UMI_SET_FIELD(cmd, ex, ex);
        //printf("cmd=%x\n", cmd);
        return cmd;
    }
};
#endif
