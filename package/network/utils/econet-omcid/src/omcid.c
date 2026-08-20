/* omcid.c — native OpenWrt OMCI daemon for the econet-xpon GPON port.
 *
 * Replaces the fragile stock omci_v2 chroot. Pure userspace: it speaks baseline
 * OMCI to the live OLT over the driver's "omci" netdev (raw OMCI PDU, no L2
 * header), maintains a MIB, and programs the datapath via /proc/econet_xpon_*.
 *
 * Transport (proven from xpondrv.c / gpon_wan.c):
 *   RX: recvfrom(AF_PACKET, "omci") -> [0:1]TCI [2]MT(bit6=AR) [3]devID=0x0a
 *       [4:5]class [6:7]instance [8:39]content.
 *   TX: send() a 44-byte baseline PDU; driver stamps gem=GPON_OMCC_ID, HW adds MIC.
 *
 * Usage: omcid [ifname] [serial]
 *   ifname  OMCI netdev            (default "omci")
 *   serial  ONU serial "VVVVXXXXXXXX" = 4 ASCII vendor chars + 8 hex digits
 *           (or via $OMCID_SERIAL). NOT hardcoded: no subscriber identity is
 *           compiled into the binary.
 *
 * Build: mipsel-openwrt-linux-gcc -O2 -static -o omcid omcid.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <arpa/inet.h>

/* ---- OMCI baseline constants ---- */
#define OMCI_LEN        44          /* bytes we transmit (header8 + content32 + trailer4) */
#define DEV_ID          0x0a
#define AR_BIT          0x40
#define AK_BIT          0x20
#define MT_MASK         0x1f
/* message types (5-bit) */
#define MT_CREATE       0x04
#define MT_DELETE       0x06
#define MT_SET          0x08
#define MT_GET          0x09
#define MT_GET_ALL_ALARM 0x0b
#define MT_GET_ALL_ALARM_NEXT 0x0c
#define MT_MIB_UPLOAD   0x0d
#define MT_MIB_UPLOAD_NEXT 0x0e
#define MT_MIB_RESET    0x0f
#define MT_TEST         0x12
#define MT_REBOOT       0x17
#define MT_SYNC_TIME    0x18
#define MT_GET_NEXT     0x1a
#define MT_GET_CUR_DATA 0x1b
#define MT_SET_TABLE    0x1c
/* OMCI result codes */
#define RC_OK           0x00
#define RC_PROC_ERR     0x01
#define RC_NOT_SUPPORTED 0x03
#define RC_PARAM_ERR    0x04
#define RC_UNKNOWN_ME   0x05
#define RC_UNKNOWN_INST 0x06

/* ---- MIB store ---- */
#define MAX_ME 512
struct me {
    uint16_t cls;
    uint16_t inst;
    uint8_t  attr[128];  /* concatenated attribute values (some MEs exceed the 32B msg area) */
    uint8_t  in_use;
    uint8_t  pre;        /* 1 = pre-instantiated (survives MIB-Reset, reported on upload) */
};
static struct me mib[MAX_ME];
static int mib_n;
static uint8_t mib_data_sync;      /* ONU-data (class 2) MIB data sync counter */

static uint64_t now_ms(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return (uint64_t)t.tv_sec*1000 + t.tv_nsec/1000000; }
static uint64_t t0;
#define LOG(...) do{ fprintf(stderr,"[%6llu] ",(unsigned long long)(now_ms()-t0)); fprintf(stderr,__VA_ARGS__); fputc('\n',stderr); fflush(stderr);}while(0)

static struct me *mib_find(uint16_t cls,uint16_t inst){
    for(int i=0;i<mib_n;i++) if(mib[i].in_use && mib[i].cls==cls && mib[i].inst==inst) return &mib[i];
    return NULL;
}
static struct me *mib_add(uint16_t cls,uint16_t inst,int pre){
    struct me *m = mib_find(cls,inst);
    if(m) return m;
    if(mib_n>=MAX_ME) return NULL;
    m=&mib[mib_n++]; memset(m,0,sizeof *m);
    m->cls=cls; m->inst=inst; m->in_use=1; m->pre=pre?1:0;
    return m;
}
static void mib_del(uint16_t cls,uint16_t inst){
    for(int i=0;i<mib_n;i++) if(mib[i].in_use && mib[i].cls==cls && mib[i].inst==inst){ mib[i].in_use=0; return; }
}

/* ---- persist / restore the provisioned MIB ("copy provisioned data from stock") ----
 * Dump every ME (class inst pre + 128B attr hex) so a later run can pre-load the exact
 * provisioned state the OLT expects -> the OLT reconciles instead of re-provisioning. */
static const char *MIB_FILE = "/tmp/mib.dump";
static void mib_save(void){
    FILE*f=fopen(MIB_FILE,"w"); if(!f) return;
    fprintf(f,"MDS %u\n",mib_data_sync);
    for(int i=0;i<mib_n;i++){
        if(!mib[i].in_use) continue;
        fprintf(f,"%u %u %u ",mib[i].cls,mib[i].inst,mib[i].pre);
        for(int j=0;j<128;j++) fprintf(f,"%02x",mib[i].attr[j]);
        fputc('\n',f);
    }
    fclose(f);
}
static void mib_load(void){
    FILE*f=fopen(MIB_FILE,"r"); if(!f){ LOG("mib_load: no %s (fresh MIB)",MIB_FILE); return; }
    char line[512]; int n=0; unsigned a,b,c; char hx[300];
    while(fgets(line,sizeof line,f)){
        if(sscanf(line,"MDS %u",&a)==1){ mib_data_sync=(uint8_t)a; continue; }
        if(sscanf(line,"%u %u %u %256s",&a,&b,&c,hx)==4){
            struct me*m=mib_add((uint16_t)a,(uint16_t)b,(int)c);
            if(m){ int L=strlen(hx)/2; if(L>128)L=128; for(int j=0;j<L;j++){ unsigned v=0; sscanf(hx+2*j,"%2x",&v); m->attr[j]=(uint8_t)v; } n++; }
        }
    }
    fclose(f);
    LOG("mib_load: %d provisioned MEs from %s (mds=%u)",n,MIB_FILE,mib_data_sync);
}

/* ---- datapath programming ---- */
static void proc_write(const char*path,const char*val){
    FILE*f=fopen(path,"w"); if(!f){ LOG("  proc_write %s FAIL (%s)",path,strerror(errno)); return; }
    fputs(val,f); fclose(f);
    LOG("  datapath: %s <- '%s'",path,val);
}
/* GEM Port Network CTP (class 268): attr1=port-id(2), attr2=T-CONT ptr(2), attr3=direction,
 * attr6=T-CONT ptr / traffic mgmt ptr depending on rev. We map port-id + alloc to the driver. */
static void datapath_create_gem(struct me*m){
    uint16_t portid = (m->attr[0]<<8)|m->attr[1];
    /* T-CONT pointer -> find its allocId (the T-CONT ME we stored) */
    uint16_t tcont_ptr = (m->attr[2]<<8)|m->attr[3];
    struct me*tc = mib_find(262, tcont_ptr);
    uint16_t alloc = tc ? ((tc->attr[0]<<8)|tc->attr[1]) : 0;
    char v[32]; snprintf(v,sizeof v,"%u %u",portid,alloc);
    LOG("  GEM-CTP create: portId=%u tcontPtr=0x%x alloc=%u",portid,tcont_ptr,alloc);
    proc_write("/proc/econet_xpon_mkgem",v);
}

/* ---- attribute-size tables for Get (identity MEs the OLT reads) ----
 * Each class: list of attribute byte-sizes in attribute order (1..N). 0-terminated. */
struct attrtab { uint16_t cls; uint8_t size[16]; };
static const struct attrtab attrtabs[] = {
  /* ONU-G (256): vendorId(4) version(14) serial(8) tradmgmt(1) dep(1) batt(1) admstate(1)
   * opstate(1) survival(1) logicalOnuId(24) logicalPassword(12) credStatus(1) extTcOpts(2) */
  { 256, {4,14,8,1,1,1,1,1,1,24,12,1,2,0} },
  /* vendor-proprietary 0xFFFA (65530): DASAN/DZS ME the OLT polls before provisioning.
   * attr sizes unknown -> model a few small attrs so responses are well-formed (values TBD). */
  { 65530, {4,4,4,4,4,4,4,4,0} },
  /* ONU2-G (257): equipId(20) omccVer(1) vendorProdCode(2) secCap(1) secMode(1) totPQ(2) ... */
  { 257, {20,1,2,1,1,2,1,1,1,2,2,0} },
  /* ONU-data (2): mib_data_sync(1) */
  { 2, {1,0} },
  /* Software image (7): version(14) isCommitted(1) isActive(1) isValid(1) productCode(25) hash(16) */
  { 7, {14,1,1,1,25,16,0} },
  /* ANI-G (263): sr_ind(1) tot_tcont(2) gem_blk_len(2) piggyback(1) ... optical levels(2 each) */
  { 263, {1,1,2,1,1,1,1,1,1,2,1,1,2,2,1,1} },
  /* Priority queue (277): queueCfgOpt(1) maxQsize(2) allocQsize(2) discBlkRstInt(2)
   * discThresh(2) relatedPort(4) trafSchedPtr(2) weight(1) bpOp(2) bpTime(4) bpOccur(2)
   * bpClear(2) = 26B, mask 0xfff0 (attrs 1..12). Stock reports this full set; the OLT reads
   * relatedPort + trafSchedPtr to reconcile a bidirectional GEM-CTP's US traffic-mgmt pointer. */
  { 277, {1,2,2,2,2,4,2,1,2,4,2,2,0} },
  /* Traffic scheduler (278): tcontPtr(2) trafSchedPtr(2) policy(1) priorityWeight(1) = 6B,
   * mask 0xf000 (attrs 1..4). */
  { 278, {2,2,1,1,0} },
  /* ---- 2026-07-21 (#71): full attr tables for the OLT-provisioned datapath MEs, so a REAL
   * OLT MibUpload reports them with the correct mask + values instead of the attr1-only
   * fallback (omcid.c:224). Same bug-class #62 fixed for 277/278, one layer up. G.988 sizes. ---- */
  /* T-CONT (262): allocId(2) deprecated(1) policy(1) — mask 0xe000 */
  { 262, {2,1,1,0} },
  /* GEM-IW-TP (266): gemCtpConnPtr(2) iwOpt(1) svcProfPtr(2) iwTpPtr(2) pptpCtr(1)
   * operState(1) galProfPtr(2) galLoopbackCfg(1) — mask 0xff00 */
  { 266, {2,1,2,2,1,1,2,1,0} },
  /* GEM-CTP (268): portId(2) tcontPtr(2) direction(1) trafMgmtPtrUS(2) trafDescUS(2)
   * uniCtr(1) priQueuePtrDS(2) encState(1) trafDescDS(2) encKeyRing(1) — mask 0xffc0 */
  { 268, {2,2,1,2,2,1,2,1,2,1,0} },
  /* MAC-bridge-service-profile (45): stpInd(1) learnInd(1) portBridgeInd(1) prio(2) maxAge(2)
   * helloTime(2) fwdDelay(2) unknownMacDiscard(1) macLearnDepth(1) dynFiltAgeTime(4) — mask 0xffc0 */
  { 45, {1,1,1,2,2,2,2,1,1,4,0} },
  /* MAC-bridge-port-config (47): bridgeIdPtr(2) portNum(1) tpType(1) tpPtr(2) portPrio(2)
   * pathCost(2) stpInd(1) dep(1) dep(1) portMac(6) outTdPtr(2) inTdPtr(2) macLearnDepth(1) — mask 0xfff8 */
  { 47, {2,1,1,2,2,2,1,1,1,6,2,2,1,0} },
  /* VLAN-tagging-filter-data (84): filterList(24) fwdOp(1) numEntries(1) — mask 0xe000 */
  { 84, {24,1,1,0} },
  /* GAL-Ethernet-profile (272): maxGemPayloadSize(2) — mask 0x8000 */
  { 272, {2,0} },
  /* Multicast-GEM-IW-TP (281): gemCtpPtr(2) iwOpt(1) svcProfPtr(2) notUsed(2) pptpCtr(1)
   * operState(1) galProfPtr(2) notUsed2(1) — first 8 attrs, mask 0xff00 (table attrs omitted) */
  { 281, {2,1,2,2,1,1,2,1,0} },
  /* VEIP (329): adminState(1) operState(1) interdomainName(25) tcpUdpPtr(2) ianaPort(2) — mask 0xf800 */
  { 329, {1,1,25,2,2,0} },
  /* PPTP-Eth-UNI (11): expType(1) sensedType(1) autoDetect(1) loopback(1) adminState(1)
   * operState(1) configInd(1) maxFrame(2) dteDce(1) pauseTime(2) bridgedIp(1) arc(1)
   * arcInt(1) pppoeFilter(1) powerControl(1) — mask 0xfffe */
  { 11, {1,1,1,1,1,1,1,2,1,2,1,1,1,1,1,0} },
  { 0, {0} }
};
static const struct attrtab *find_attrtab(uint16_t cls){
    for(const struct attrtab*a=attrtabs;a->cls;a++) if(a->cls==cls) return a;
    return NULL;
}

/* ---- build + send a response ---- */
static int sock;
static void send_resp(uint8_t*req,uint8_t*content,int clen){
    uint8_t r[OMCI_LEN]; memset(r,0,sizeof r);
    r[0]=req[0]; r[1]=req[1];                 /* echo TCI */
    r[2]=(req[2]&MT_MASK)|AK_BIT;             /* AK response, same MT */
    r[3]=DEV_ID;
    r[4]=req[4]; r[5]=req[5];                 /* echo class */
    r[6]=req[6]; r[7]=req[7];                 /* echo instance */
    if(content && clen>0){ if(clen>32)clen=32; memcpy(r+8,content,clen); }
    /* baseline trailer at 40..43 */
    r[40]=0x00; r[41]=0x00; r[42]=0x00; r[43]=0x28;
    int n=send(sock,r,OMCI_LEN,0);
    if(n<0) LOG("  send FAIL %s",strerror(errno));
}

/* ---- handlers ---- */
static void h_mib_reset(uint8_t*req){
    int kept=0;
    for(int i=0;i<mib_n;i++){ if(mib[i].in_use && !mib[i].pre) mib[i].in_use=0; else if(mib[i].in_use) kept++; }
    mib_data_sync=0;
    LOG("MIB-RESET -> cleared (kept %d pre-instantiated)",kept);
    uint8_t c[32]={0}; c[0]=RC_OK; send_resp(req,c,1);
}
static int upload_count = -1;   /* 0 = report blank MIB so OLT provisions from scratch; -1 = report all */
static int no_selfcreate = 0;   /* env OMCID_NOSELFCREATE=1 -> disable 266->268 GEM-CTP self-create
                                 * (so any Create 268/225 seen is unambiguously OLT-originated) */
static void h_mib_upload(uint8_t*req){
    int cnt; if(upload_count>=0) cnt=upload_count; else { cnt=0; for(int i=0;i<mib_n;i++) if(mib[i].in_use) cnt++; }
    LOG("MIB-UPLOAD -> %d MEs",cnt);
    uint8_t c[32]={0}; c[0]=(cnt>>8)&0xff; c[1]=cnt&0xff; send_resp(req,c,2);
}
static void h_mib_upload_next(uint8_t*req){
    uint16_t seq=(req[8]<<8)|req[9];
    /* return the seq-th in-use ME: {class(2) inst(2) attrmask(2) attrs...} — the mask
     * and attribute sizes MUST be correct or the OLT can't parse (e.g. the ONU-G serial
     * for its PLOAM-vs-OMCI serial check). Report the attributes that fit in one msg. */
    int k=0; struct me*m=NULL;
    for(int i=0;i<mib_n;i++) if(mib[i].in_use){ if(k==seq){ m=&mib[i]; break;} k++; }
    uint8_t c[32]={0};
    if(m){
        c[0]=(m->cls>>8)&0xff; c[1]=m->cls&0xff;
        c[2]=(m->inst>>8)&0xff; c[3]=m->inst&0xff;
        const struct attrtab*t=find_attrtab(m->cls);
        uint16_t mask=0; int off=6, src=0;
        if(m->cls==2){ mask=0x8000; c[6]=mib_data_sync; off=7; }
        else if(t){
            for(int a=0;a<16 && t->size[a];a++){
                int sz=t->size[a];
                if(off+sz<=32 && src+sz<=(int)sizeof(m->attr)){ mask|=(0x8000>>a); memcpy(c+off,m->attr+src,sz); off+=sz; }
                src+=sz;
            }
        } else { mask=0x8000; memcpy(c+6,m->attr,20); off=26; }   /* unknown ME: best-effort attr1 */
        c[4]=(mask>>8)&0xff; c[5]=mask&0xff;
        LOG("MIB-UPLOAD-NEXT seq=%u -> class=%u inst=%u mask=0x%04x %dB",seq,m->cls,m->inst,mask,off-6);
    } else LOG("MIB-UPLOAD-NEXT seq=%u -> (none)",seq);
    send_resp(req,c,32);
}
static void h_create(uint8_t*req,uint16_t cls,uint16_t inst){
    struct me*m=mib_add(cls,inst,0);
    if(m){ memcpy(m->attr,req+8,32); mib_data_sync++; }   /* Create changes the MIB -> increment */
    LOG("CREATE class=%u inst=%u -> %s (mds=%u)",cls,inst,m?"ok":"FULL",mib_data_sync);
    /* baseline Create response: result(1) + attribute-execution-mask(2), all 0 = success */
    uint8_t c[32]={0}; c[0]=m?RC_OK:RC_PROC_ERR;
    send_resp(req,c,3);
    if(m && cls==268) datapath_create_gem(m);
    if(m && cls==266 && !no_selfcreate){
        /* PORT-SIDE GAP FIX: the OLT withholds the DATA GEM-CTP(268) Create for this (unauthorized)
         * identity — it creates only the null CTP(4095). The GEM-IW-TP's connectivity ptr (attr1)
         * names the data GEM-CTP -> self-create it so the port MIB + datapath match stock exactly.
         * Attrs per stock golden Create GEM-CTP 225 (00e1 8000 03 8000): portId=ptr, TcontPtr=0x8000,
         * direction=bidir(3), trafMgmtPtr=0x8000. Datapath (re)bound on the T-CONT Set below. */
        uint16_t ctp=(m->attr[0]<<8)|m->attr[1];
        if(ctp && ctp!=0x0fff && !mib_find(268,ctp)){
            struct me*g=mib_add(268,ctp,0);
            if(g){
                g->attr[0]=(ctp>>8)&0xff; g->attr[1]=ctp&0xff;   /* GEM port-id */
                g->attr[2]=0x80; g->attr[3]=0x00;                /* T-CONT pointer = 0x8000 */
                g->attr[4]=0x03;                                 /* direction = bidirectional */
                g->attr[5]=0x80; g->attr[6]=0x00;                /* traffic-mgmt-option ptr */
                mib_data_sync++;
                LOG("SELF-CREATE GEM-CTP(268) inst=%u (OLT withheld it) -> MIB+datapath",ctp);
                datapath_create_gem(g);
            }
        }
    }
    if(m) mib_save();
}
static void h_delete(uint8_t*req,uint16_t cls,uint16_t inst){
    mib_del(cls,inst); mib_data_sync++;                   /* Delete changes the MIB -> increment */
    LOG("DELETE class=%u inst=%u",cls,inst);
    uint8_t c[32]={0}; c[0]=RC_OK; send_resp(req,c,1);
    mib_save();
}
static void h_set(uint8_t*req,uint16_t cls,uint16_t inst){
    struct me*m=mib_find(cls,inst);
    if(!m) m=mib_add(cls,inst,0);              /* tolerate Set-before-Create for pre-instantiated */
    uint16_t mask=(req[8]<<8)|req[9];
    /* MASKED write: copy ONLY the attributes the OLT actually set (per the attr-size table), at
     * their correct offsets. The old blind memcpy(req+10,22) clobbered UNSET attrs -- an OLT
     * security-mode Set on ONU2-G(257) wiped equipId+OMCC. Mirror h_get's mask walk. */
    if(m){
        const struct attrtab*st=find_attrtab(cls);
        if(st){
            int moff=0, roff=0;
            for(int a=0;a<16 && st->size[a];a++){
                int sz=st->size[a];
                if(mask & (0x8000>>a)){
                    if(moff+sz<=(int)sizeof(m->attr) && roff+sz<=22) memcpy(m->attr+moff, req+10+roff, sz);
                    roff+=sz;
                }
                moff+=sz;
            }
        } else {
            memcpy(m->attr,req+10,22);
        }
    }
    /* ONU-data(2) MIB-data-sync SET is the OLT RESYNCING the counter -> store its value, do NOT
     * increment (per G.988 a change to MIB-data-sync itself doesn't increment). Any other Set
     * changes the MIB -> increment, so the ONU's counter tracks the OLT's after its Create batch
     * (this is what stops the endless re-provision loop). */
    if(cls==2 && (mask&0x8000)) mib_data_sync=req[10];
    else mib_data_sync++;
    LOG("SET class=%u inst=%u mask=0x%04x (mds=%u)",cls,inst,mask,mib_data_sync);
    if(cls==262){
        /* T-CONT alloc just set -> (re)bind any self-created GEM-CTP pointing at this T-CONT,
         * now that its allocId is known (handles Set-after-Create ordering). */
        for(int i=0;i<mib_n;i++) if(mib[i].in_use && mib[i].cls==268){
            uint16_t tp=(mib[i].attr[2]<<8)|mib[i].attr[3];
            if(tp==inst){ LOG("  rebind GEM-CTP %u after T-CONT %u set",mib[i].inst,inst); datapath_create_gem(&mib[i]); }
        }
    }
    uint8_t c[32]={0}; c[0]=m?RC_OK:RC_UNKNOWN_INST; send_resp(req,c,1);
    if(m) mib_save();
}
static void h_get(uint8_t*req,uint16_t cls,uint16_t inst){
    struct me*m=mib_find(cls,inst);
    uint16_t mask=(req[8]<<8)|req[9];
    uint8_t c[32]={0};
    if(!m){ c[0]=RC_UNKNOWN_INST; LOG("GET class=%u inst=%u -> unknown inst",cls,inst); send_resp(req,c,3); return; }
    c[0]=RC_OK; c[1]=(mask>>8)&0xff; c[2]=mask&0xff;
    /* fill attributes per the size table, walking the requested mask (bit15=attr1) */
    const struct attrtab*t=find_attrtab(cls);
    int off=3, src=0;
    if(cls==2){ /* ONU-data: only attr = mib_data_sync */
        if(mask&0x8000){ c[off++]=mib_data_sync; }
    } else if(t){
        for(int a=0;a<16 && t->size[a];a++){
            int sz=t->size[a];
            if(mask & (0x8000>>a)){
                if(off+sz<=32 && src+sz<=(int)sizeof(m->attr)) memcpy(c+off,m->attr+src,sz);
                off+=sz;
            }
            src+=sz;
        }
    } else {
        memcpy(c+off,m->attr,26); off=29;
    }
    LOG("GET class=%u inst=%u mask=0x%04x -> %d bytes",cls,inst,mask,off-3);
    send_resp(req,c,off);
}
static void h_generic_ok(uint8_t*req,const char*name){
    LOG("%s -> ok",name);
    uint8_t c[32]={0}; c[0]=RC_OK; send_resp(req,c,1);
}

/* ---- ONU serial number (runtime, NOT hardcoded) ----
 * The 8-byte GPON serial = 4 ASCII vendor bytes + 4 binary bytes. It is supplied
 * at runtime (argv[2] or $OMCID_SERIAL) in the textual form "VVVVXXXXXXXX" = 4
 * ASCII vendor chars + 8 hex digits (the same form the kernel module's gpon_sn=
 * parameter takes). No subscriber identity is compiled into the binary; if none
 * is supplied the
 * serial stays zeroed and the ONU will not register (a warning is logged). */
static uint8_t g_onu_sn[8] = {0};

static int parse_onu_sn(const char *s){
    if(!s || strlen(s)!=12) return 0;   /* 4 ASCII vendor + 8 hex digits */
    uint8_t sn[8];
    memcpy(sn,s,4);                      /* vendor id (ASCII) */
    for(int i=0;i<4;i++){
        char h[3]={s[4+i*2],s[5+i*2],0}, *end=NULL;
        unsigned long v=strtoul(h,&end,16);
        if(end!=h+2) return 0;           /* not two hex digits */
        sn[4+i]=(uint8_t)v;
    }
    memcpy(g_onu_sn,sn,8);
    return 1;
}

/* ---- pre-instantiated baseline MIB (H660GM-A, VEIP/HGU) ----
 * Minimal starting set; iterate against the live OLT. */
static void seed_mib(void){
    /* ONU-G (256/0): vendorId + version + serial. The serial is supplied at
     * runtime (see g_onu_sn / parse_onu_sn); NOTHING is hardcoded here. */
    struct me*m=mib_add(256,0,1);
    memcpy(m->attr, g_onu_sn, 4);                   /* vendor id = ASCII prefix of the runtime serial */
    memset(m->attr+4,0x20,14); memcpy(m->attr+4,"RP0201",6); /* version RP0201 (H660 stock romfile) */
    memcpy(m->attr+18, g_onu_sn, 8);                /* serial number (8B): 4 ASCII vendor + 4 binary */
    /* ONU-G secondaries the stock romfile reports (romfile.xml #71): traffic-mgmt-option=2,
     * onu-survival-time=10, extended-TC-layer-options=3 (port previously left these 0).
     * attrtab {4,14,8,1,1,1,1,1,1,24,12,1,2}: attr4 @26, attr9 @31, attr13 @69-70. */
    m->attr[26]=0x02;                    /* attr4 traffic-management-option */
    m->attr[31]=0x0a;                    /* attr9 ONU-survival-time = 10 */
    m->attr[69]=0x00; m->attr[70]=0x03;  /* attr13 extended-TC-layer-options = 3 */
    /* ONU2-G (257/0): equipment-id(20)=blank, omcc version 0xA1 (DSNW-provisioned, per omcifix RE) */
    m=mib_add(257,0,1); m->attr[20]=0xA1; m->attr[21]=0x01; m->attr[22]=0x54; m->attr[23]=0x01; /* omccVer A1, vendorProdCode 340 (0x0154), secCap 1 (stock romfile) */
    /* ONU-data (2/0) */
    mib_add(2,0,1);
    /* ANI-G (263/0x8001) */
    mib_add(263,0x8001,1);
    /* 8 T-CONTs (262/0x8000..0x8007) pre-instantiated, alloc unset until OLT Sets */
    for(int i=0;i<8;i++) mib_add(262,0x8000+i,1);
    /* VEIP (329/0x0A01=2561) pre-instantiated for HGU (matches stock romfile ME-ID) */
    mib_add(329,0x0A01,1);
    /* PPTP Ethernet UNI x4 (11/0x0101..0x0104) */
    for(int i=0;i<4;i++) mib_add(11,0x0101+i,1);
    /* software images (7/0,7/1): version(14) committed(1) active(1) valid(1) */
    m=mib_add(7,0,1); memcpy(m->attr,"V3.2.2120054  ",14); m->attr[14]=1; m->attr[15]=1; m->attr[16]=1;
    m=mib_add(7,1,1); memcpy(m->attr,"V3.2.2120040  ",14); m->attr[14]=0; m->attr[15]=0; m->attr[16]=1;
    /* vendor-proprietary ME 0xFFFA (65530) the OLT polls before provisioning */
    mib_add(65530,0,1);
    LOG("seeded baseline MIB: %d MEs",mib_n);
}

int main(int argc,char**argv){
    const char*ifn = argc>1?argv[1]:"omci";
    { const char*s = argc>2 ? argv[2] : getenv("OMCID_SERIAL");
      if(!parse_onu_sn(s))
          LOG("WARNING: no/invalid ONU serial (pass argv[2] or $OMCID_SERIAL as \"VVVVXXXXXXXX\": 4 vendor chars + 8 hex); ONU will NOT register until provisioned"); }
    { const char*e=getenv("OMCID_NOSELFCREATE"); if(e && e[0]=='1') no_selfcreate=1; }
    t0=now_ms();
    sock=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sock<0){ perror("socket"); return 1; }
    struct ifreq ifr; memset(&ifr,0,sizeof ifr); strncpy(ifr.ifr_name,ifn,IFNAMSIZ-1);
    if(ioctl(sock,SIOCGIFINDEX,&ifr)<0){ perror("ifindex"); return 1; }
    struct sockaddr_ll sll; memset(&sll,0,sizeof sll);
    sll.sll_family=AF_PACKET; sll.sll_protocol=htons(ETH_P_ALL); sll.sll_ifindex=ifr.ifr_ifindex;
    if(bind(sock,(struct sockaddr*)&sll,sizeof sll)<0){ perror("bind"); return 1; }
    LOG("omcid up on %s (ifindex %d) no_selfcreate=%d",ifn,ifr.ifr_ifindex,no_selfcreate);
    seed_mib();
    mib_load();   /* overlay the saved provisioned MIB, if any, so the OLT reconciles */

    uint8_t buf[2048];
    for(;;){
        struct sockaddr_ll from; socklen_t fl=sizeof from;
        int n=recvfrom(sock,buf,sizeof buf,0,(struct sockaddr*)&from,&fl);
        if(n<40) continue;   /* baseline OMCI: 8B header + 32B content */
        if(from.sll_pkttype==PACKET_OUTGOING) continue;   /* ignore our own TX */
        if(buf[3]!=DEV_ID) { /* not baseline OMCI (extended 0x0b unhandled for now) */ continue; }
        uint8_t mt=buf[2]&MT_MASK; uint8_t ar=(buf[2]&AR_BIT)!=0;
        uint16_t cls=(buf[4]<<8)|buf[5]; uint16_t inst=(buf[6]<<8)|buf[7];
        if(!ar) continue;   /* no response requested (autonomous/ack) */
        switch(mt){
            case MT_MIB_RESET:      h_mib_reset(buf); break;
            case MT_MIB_UPLOAD:     h_mib_upload(buf); break;
            case MT_MIB_UPLOAD_NEXT:h_mib_upload_next(buf); break;
            case MT_CREATE:         h_create(buf,cls,inst); break;
            case MT_DELETE:         h_delete(buf,cls,inst); break;
            case MT_SET:            h_set(buf,cls,inst); break;
            case MT_GET:            h_get(buf,cls,inst); break;
            case MT_GET_ALL_ALARM:  { uint8_t c[32]={0}; LOG("GET-ALL-ALARMS -> 0"); send_resp(buf,c,2);} break;
            case MT_SYNC_TIME:      h_generic_ok(buf,"SYNC-TIME"); break;
            case MT_TEST:           h_generic_ok(buf,"TEST"); break;
            case MT_REBOOT:         h_generic_ok(buf,"REBOOT(ignored)"); break;
            case MT_GET_NEXT:       { uint8_t c[32]={0}; c[0]=RC_OK; LOG("GET-NEXT class=%u inst=%u (stub)",cls,inst); send_resp(buf,c,29);} break;
            default:                LOG("UNHANDLED mt=0x%02x class=%u inst=%u",mt,cls,inst);
                                    h_generic_ok(buf,"generic"); break;
        }
    }
    return 0;
}
