/*
 *  FECFG146.H
 *
 *  'C' Structures of FastEcho 1.46
 *  Copyright (c) 1995 by Tobias Burchhardt.  Last update: 30 Jun 1995.
 *  Modified by Aleksandar Ivanisevic 30 Oct 1996.
 *  Minor alterations by Andrew Clarke 22 Dec 1996.
 *  Modfications by Tobias Ernst 04 Oct 1998
 *  Merged changes from fecfg146.h from T. Burchhardt 04 Oct 2000
 */

#ifndef __FECFG146_H__
#define __FECFG146_H__

/*
We don't set pragma pack anymore. The result is that the structures below
are probably larger than their pendants in the binary file. You need to write
portable reader functions for reading them from file. For those structures
that are used by MsgEd, such reader functions can be found in fecfg145.c.

#if defined(PACIFIC) || defined(_MSC_VER) || defined(__EMX__) || defined(__IBMC__) || defined(__HIGHC__) || defined(__UNIX__) || defined(__DJGPP__)
#pragma pack(1)
#endif

*/

/********************************************************
 * FASTECHO.CFG = <CONFIG>                              *
 *                + <optional extensions>               *
 *                + <CONFIG.NodeCnt * Node>             *
 *                + <CONFIG.AreaCnt * Area>             *
 ********************************************************/

#define REVISION        6       /* current revision     */

/*
 *  Note: there is a major change in this revision - the Node records
 *  have no longer a fixed length!
 */

#define MAX_AREAS       4096    /* max # of areas          */
#define MAX_NODES       1024    /* max # of nodes          */
#define MAX_GROUPS      32      /* max # of groups         */
#define MAX_AKAS        32      /* max # of akas           */
#define MAX_ROUTE       15      /* max # of 'vias'         */
#define MAX_ORIGINS     20      /* max # of origins        */
#define MAX_GATES	10	/* max # of Internet gates */

/*
 *  Note: The MAX_AREAS and MAX_NODES are only the absolute maximums as the
 *  handling is flexible. To get the maximums which are used for the config
 *  file you read, you have to examine the CONFIG.MaxAreas and
 *  CONFIG.MaxNodes variables!
 *
 *  Note: The MAX_AREAS and MAX_NODES maximums are subject to change with
 *  any new version, therefore - if possible - make handling as flexible as
 *  possible and use CONFIG.MaxAreas and .MaxNodes whereever possible. But
 *  be aware that you might (under normal DOS and depending on the way you
 *  handle it) hit the 64kB segment limit pretty quickly!
 *
 *  Same goes for the # of AKAs and Groups - use the values found in
 *  CONFIG.AkaCnt and CONFIG.GroupCnt!
 *
 *  Note: Define INC_FE_TYPES, INC_FE_BAMPROCS  and  INC_FE_DATETYPE
 *  to include the typedefs if necessary.
 */

/********************************************************
 * CONFIG.flags                                         *
 ********************************************************/

#define RETEAR                  0x00000001l
#define AUTOCREATE              0x00000002l
#define KILLEMPTY               0x00000004l
#define KILLDUPES               0x00000008l
#define CLEANTEARLINE           0x00001000l
#define IMPORT_INCLUDEUSERSBBS  0x00002000l
#define KILLSTRAYATTACHES       0x00004000l
#define PURGE_PROCESSDATE       0x00008000l
#define MAILER_RESCAN           0x00010000l
#define EXCLUDE_USERS           0x00020000l
#define EXCLUDE_SYSOPS          0x00040000l
#define CHECK_DESTINATION       0x00080000l
#define UPDATE_BBS_CONFIG       0x00100000l
#define KILL_GRUNGED_DATE       0x00200000l
#define NOT_BUFFER_EMS          0x00400000l
#define KEEP_NETMAILS           0x00800000l
#define NOT_UPDATE_MAILER       0x01000000l
#define NOT_CHECK_SEMAPHORES    0x02000000l
#define CREATE_SEMAPHORES       0x04000000l
#define CHECK_COMPLETE          0x08000000l
#define RESPOND_TO_RRQ          0x10000000l
#define TEMP_OUTB_HARDDISK      0x20000000l
#define FORWARD_PACKETS         0x40000000l
#define UNPACK_UNPROTECTED      0x80000000l

/********************************************************
 * CONFIG.mailer                                        *
 ********************************************************/

#define FrontDoor               0x0001
#define InterMail               0x0002
#define DBridge                 0x0004
#define Binkley                 0x0010
#define PortalOfPower           0x0020
#define McMail                  0x0040

/********************************************************
 * CONFIG.BBSSoftware                                   *
 ********************************************************/

enum BBSSoft
{
    NoBBSSoft = 0,
    RemoteAccess111,
    QuickBBS,
    SuperBBS,
    ProBoard122 /* Unused */,
    TagBBS,
    RemoteAccess200,
    ProBoard130 /* Unused */,
    ProBoard200,
    ProBoard202,
    Maximus202,
    Maximus300
};

/********************************************************
 * CONFIG.CC.what                                       *
 ********************************************************/

#define CC_FROM                 1
#define CC_TO                   2
#define CC_SUBJECT              3
#define CC_KLUDGE               4

/********************************************************
 * CONFIG.QuietLevel                                    *
 ********************************************************/

#define QUIET_PACK              0x0001
#define QUIET_UNPACK            0x0002
#define QUIET_EXTERN            0x0004

/********************************************************
 * CONFIG.Swapping                                      *
 ********************************************************/

#define SWAP_TO_XMS             0x0001
#define SWAP_TO_EMS             0x0002
#define SWAP_TO_DISK            0x0004

/********************************************************
 * CONFIG.Buffers                                       *
 ********************************************************/

#define BUF_LARGE               0x0000
#define BUF_MEDIUM              0x0001
#define BUF_SMALL               0x0002

/********************************************************
 * CONFIG.arcext.inb/outb                               *
 ********************************************************/

enum ARCmailExt
{
    ARCDigits = 0, ARCHex, ARCAlpha
};

/********************************************************
 * CONFIG.AreaFixFlags                                  *
 ********************************************************/

#define ALLOWRESCAN             0x0001
#define KEEPREQUEST             0x0002
#define KEEPRECEIPT             0x0004
#define ALLOWREMOTE             0x0008
#define DETAILEDLIST            0x0010
#define ALLOWPASSWORD           0x0020
#define ALLOWPKTPWD             0x0040
#define ALLOWCOMPRESS           0x0080
#define SCANBEFORE              0x0100
#define ADDRECEIPTLIST          0x0200
#define NOTIFYPASSWORDS         0x0400
#define SENDCONFERENCERULES     0x0800

/********************************************************
 * Area.board (1-200 = Hudson)                          *
 ********************************************************/

#define NO_BOARD        0x4000u /* JAM/Sq/Passthru etc. */
#define AREA_DELETED    0x8000u /* usually never written */

/********************************************************
 * Area.flags.storage                                   *
 ********************************************************/

#define FE_QBBS                 0
#define FE_FIDO                 1  /* to avoid confusion with Msged's defs */
#define FE_SQUISH               2
#define FE_JAM                  3
#define FE_PASSTHRU             7

/********************************************************
 * Area.flags.atype                                     *
 ********************************************************/

#define AREA_ECHOMAIL           0
#define AREA_NETMAIL            1
#define AREA_LOCAL              2
#define AREA_BADMAILBOARD       3
#define AREA_DUPEBOARD          4

/********************************************************
 * GateDef.flags                                        *
 ********************************************************/
#define GATE_KEEPMAILS	0x0001

/********************************************************
 * Types and other definitions                          *
 ********************************************************/

enum ARCers
{
    ARC_Unknown = -1, ARC_SeaArc, ARC_PkArc, ARC_Pak,
    ARC_ArcPlus, ARC_Zoo, ARC_PkZip, ARC_Lha, ARC_Arj,
    ARC_Sqz, ARC_RAR, ARC_UC2
};  /* for Unpackers */

enum NetmailStatus
{
    NetNormal = 0, NetHold, NetCrash, NetImm
};

enum AreaFixType
{
    NoAreaFix = 0, NormalAreaFix, FSC57AreaFix
};
enum AreaFixSendTo
{
    AreaFix = 0, AreaMgr, AreaLink, EchoMgr
};

/********************************************************
 * Structures                                           *
 ********************************************************/

typedef struct
{
    unsigned short zone, net, node, point;
}
FEAddress;
#define FE_ADDRESS_SIZE 8

#define _MAXPATH 56

typedef struct CONFIGURATION
{
    unsigned short revision;
    unsigned long flags;
    unsigned short NodeCnt, AreaCnt, unused1;
    char NetMPath[_MAXPATH];
    char MsgBase[_MAXPATH];
    char InBound[_MAXPATH];
    char OutBound[_MAXPATH];
    char Unpacker[_MAXPATH];    /* DOS default decompression program */
    char LogFile[_MAXPATH];
    char unused2[280];
    char RulesDir[_MAXPATH];
    char Unpacker2[_MAXPATH];   /* OS/2 default decompression program */
    char UnprotInBound[_MAXPATH];
    char StatFile[_MAXPATH];
    char SwapPath[_MAXPATH];
    char SemaphorePath[_MAXPATH];
    char BBSConfigPath[_MAXPATH];
    char QueuePath[_MAXPATH];   /* DBQueuePath */
    char RulesPrefix[32];
    char RetearTo[40];
    char LocalInBound[_MAXPATH];
    char ExtAfter[_MAXPATH - 4];
    char ExtBefore[_MAXPATH - 4];
    unsigned char unused3[480];
    struct
    {
        unsigned char what;
        char object[31];
        unsigned short conference;
    }
    CC[10];
    unsigned char security, loglevel;
    unsigned short def_days, def_messages;
    unsigned char unused4[462];
    unsigned short autorenum;
    unsigned short def_recvdays;
    unsigned char openQQQs, Swapping;
    unsigned short compressafter;
    unsigned short afixmaxmsglen;
    unsigned short compressfree;
    char TempPath[_MAXPATH];
    unsigned char graphics, BBSSoftware;
    char AreaFixHelp[_MAXPATH];
    unsigned char unused5[504];
    unsigned short AreaFixFlags;
    unsigned char QuietLevel, Buffers;
    unsigned char FWACnt;  /* # of ForwardAreaFix records */
    unsigned char GDCnt;   /* # of Group Default records */
    struct
    {
        unsigned short flags;
        unsigned short days[2];
        unsigned short msgs[2];
    }
    rescan_def;
    unsigned long duperecords;
    struct
    {
        unsigned char inb;
        unsigned char outb;
    }
    arcext;
    unsigned short AFixRcptLen;
    unsigned char AkaCnt, resv;  /* # of Aka records stored */
    unsigned short maxPKT;
    unsigned char sharing, sorting;
    struct
    {
        char name[36];
        unsigned long resv;
    }
    sysops[11];
    char AreaFixLog[_MAXPATH];
    char TempInBound[_MAXPATH];
    unsigned short maxPKTmsgs;
    unsigned short RouteCnt;              /* # of PackRoute records */
    unsigned char maxPACKratio;
    unsigned char SemaphoreTimer;
    unsigned char PackerCnt, UnpackerCnt; /* # of Packer + Unpacker records */
    unsigned char GroupCnt, OriginCnt;    /* # of GroupName + Origin records */
    unsigned short mailer;
    unsigned short maxarcsize, maxarcdays;
    unsigned short minInbPKTSize;
    char reserved[804];

    unsigned short AreaRecSize, GrpDefRecSize;
                                       /*
                                        *  Size of Area and GroupDefaults
                                        *  records stored in this file
                                        */

    unsigned short MaxAreas, MaxNodes; /* Current max values for this config */
    unsigned short NodeRecSize;        /* Size of each stored Node record */

    unsigned long offset;              /*
                                        *  This is the offset from the current
                                        *  file-pointer to the 1st Node
                                        */
}
CONFIG;
#define FE_CONFIG_SIZE 4644

/*
 *  To directly access the 'Nodes' and/or 'Areas' while bypassing the
 *  Extensions, perform an absolute (from beginning of file) seek to
 *  sizeof(CONFIG) + CONFIG.offset.
 *
 *  If you want to access the 'Areas', you have to add the following value
 *  to the above formula: CONFIG.NodeCnt * CONFIG.NodeRecSize.
 */

typedef struct
{
    FEAddress addr;               /* Main address */
    FEAddress arcdest;            /* ARCmail fileattach address */
    unsigned char aka, autopassive, newgroup, resv1;
    struct
    {
        unsigned passive          : 1;
        unsigned dddd             : 1; /*Type 2+/4D */
        unsigned arcmail060       : 1;
        unsigned tosscan          : 1;
        unsigned umlautnet        : 1;
        unsigned exportbyname     : 1;
        unsigned allowareacreate  : 1;
        unsigned disablerescan    : 1;
        unsigned arc_status       : 2; /*NetmailStatus for ARCmail attaches */
        unsigned arc_direct       : 1; /*Direct flag for ARCmail attaches   */
        unsigned noattach         : 1; /*don't create a ARCmail file attach */
        unsigned mgr_status       : 2; /*NetMailStatus for AreaFix receipts */
        unsigned mgr_direct       : 1; /*Direct flag for ... */
        unsigned not_help         : 1;
        unsigned not_notify       : 1;
        unsigned packer           : 4;
        unsigned packpriority     : 1;
        unsigned resv             : 2;
    }
    flags;                      /* 24 bits total! */
    union
    { struct{
        unsigned areafixtype      : 2;  /* Type of AreaFix: None (human),       */
                                        /* Normal or Advanced (FSC-57)          */
        unsigned forward          : 1;  /* Forward AreaFix requests             */
        unsigned allowremote      : 1;  /*                                      */
        unsigned allowdelete      : 1;  /* flags for different FSC-57 requests  */
        unsigned allowrename      : 1;  /* all 3 reserved for future use        */
        unsigned binarylist       : 1;  /* Forward changes (FSC-57)             */
        unsigned addplus          : 1;  /* add '+' when requesting new area     */
        unsigned addtear          : 1;  /* add tearline to the end of requests  */
        unsigned sendto           : 3;  /* name of this systems's AreaFix robot */
        unsigned nosendrules      : 1;  /* Send rules or not                    */
        unsigned resv             : 3;  /* Reserved (unused space)              */
      }bits;
      unsigned short afixflags;
    }
    afixflags;
    unsigned short resv2;
    char password[9];          /* .PKT password    */
    char areafixpw[9];         /* AreaFix password */
    unsigned short sec_level;
    unsigned long groups;      /* Bit-field, UCHAR 0/Bit 7 = 'A' etc. */
    /* FALSE means group is active */
    unsigned long resv3;
    unsigned short resv4;
    unsigned short maxarcsize;
    char name[36];             /* Name of sysop */
    unsigned char *areas;      /*
                                *  Bit-field with CONFIG.MaxAreas / 8
                                *  bits, Unsigned Char 0/Bit 7 is conference #0
                                *
                                *  Pointer will be alloc'ed by read_fe_node;
                                *  call free_fe_node to release it!
                                */
}
Node;                          /*
                                *  Total size of each record is stored in
                                *  CONFIG.NodeRecSize.
                                */
#define FE_NODE_SIZE (79 + (2 * FE_ADDRESS_SIZE))
                                /* FE_NODE_SIZE is size of the record except
                                   for the variable length "areas" field */


typedef struct
{
    char name[52];
    unsigned short board;        /* 1-200 Hudson, others reserved/special */
    unsigned short conference;   /* 0 ... CONFIG.MaxAreas-1 */
    unsigned short read_sec, write_sec;
    struct
    {
        unsigned aka    : 8;     /* 0 ... CONFIG.AkaCnt */
        unsigned group  : 8;     /* 0 ... CONFIG.GroupCnt */
    }
    info;
    struct
    {
        unsigned storage: 4;
        unsigned atype  : 4;
        unsigned origin : 5;     /* # of origin line */
        unsigned resv   : 3;
    }
    flags;
    struct
    {
        unsigned autoadded  : 1;
        unsigned tinyseen   : 1;
        unsigned cpd        : 1;
        unsigned passive    : 1;
        unsigned keepseen   : 1;
        unsigned mandatory  : 1;
        unsigned keepsysop  : 1;
        unsigned killread   : 1;
        unsigned disablepsv : 1;
        unsigned keepmails  : 1;
        unsigned hide       : 1;
        unsigned manual     : 1;
        unsigned umlaut     : 1;
        unsigned resv       : 3;
    }
    advflags;
    unsigned short resv1;
    unsigned long seenbys;              /* LSB = Aka0, MSB = Aka31        */
    unsigned long resv2;
    short days;
    short messages;
    short recvdays;
    char path[_MAXPATH];
    char desc[52];
}
Area;

#define FE_AREA_SIZE 190

/********************************************************
 * Optional Extensions                                  *
 ********************************************************
 *
 *  These are the variable length extensions between CONFIG and the first
 *  Node record. Each extension has a header which contains the info about
 *  the type and the length of the extension. You can read the fields using
 *  the following algorithm:
 *
 *    offset = 0;
 *    while (offset < CONFIG.offset)
 *    {
 *        read_header();
 *        if (header.type == EH_abc)
 *        {
 *            read_and_process_data();
 *        }
 *        else if (header.type == EH_xyz)
 *        {
 *            read_and_process_data();
 *        }
 *        else if (header.type == ...)
 *        {
 *            ...
 *        }
 *        else
 *        {
 *            */ /*  unknown or unwanted extension found */ /*
 *        }
 *        seek_forward(header.offset);  */ /*  Seek to next header */ /*
 *        offset += header.offset + sizeof(header);
 *    }
 *
 */

typedef struct
{
    unsigned short type;                /* EH_...                           */
    unsigned long offset;               /* length of field excluding header */
}
ExtensionHeader;
#define FE_EXTHEADER_SIZE 6

                                /* note: original .h file defined this constant
                                   to 0x0001, but this obviously did not work
                                 */
#define EH_AREAFIX      0x000d          /* CONFIG.FWACnt * <ForwardAreaFix> */

enum AreaFixAreaListFormat
{
    Areas_BBS = 0, Area_List
};

typedef struct
{
    unsigned short nodenr;
    struct
    {
/*
        unsigned newgroup : 8;
        unsigned active   : 1;
        unsigned valid    : 1;
        unsigned uncond   : 1;
        unsigned format   : 3;
        unsigned resv     : 2;
*/
        unsigned short flags;
    }
    flags;
    char file[_MAXPATH];
    char resv0[56];
    unsigned short sec_level;
    unsigned short resv1;
    unsigned char resv3[3];
    unsigned long groups;
    char resv2[33];              /* THIS LOOKS LIKE BULLSHIT FIXME */
}
ForwardAreaFix;

#define FE_FORWARD_AREAFIX_SIZE 160

#define EH_GROUPS       0x000C  /* CONFIG.GroupCnt * <GroupNames> */

typedef struct
{
    char name[36];
}
GroupNames;
#define FE_GROUPNAMES_LEN 36

#define EH_GRPDEFAULTS  0x0006  /* CONFIG.GDCnt * <GroupDefaults> */

/* Size of each full GroupDefault record is CONFIG.GrpDefResSize */

typedef struct
{
    unsigned char group;
    unsigned char resv[15];
    Area area;
    unsigned char *nodes;       /*
                                 * variable, c.MaxNodes / 8 bytes
                                 *
                                 *  Pointer will be malloc'ed by
                                 *  read_fe_groupdefaults,
                                 *  call free_fe_groupdefaults to release it!
                                 */

}
GroupDefaults;
#define FE_GROUPDEFAULTS_SIZE (16 + FE_AREA_SIZE)
                                /* This is the length of the record except for
                                   the variable length "nodes" field */

#define EH_AKAS         0x0007  /* CONFIG.AkaCnt * <SysAddress> */

typedef struct
{
    FEAddress main;
    char domain[28];
    unsigned short pointnet;
    unsigned long flags;        /* unused */
}
SysAddress;
#define FE_SYS_ADDRESS_SIZE (FE_ADDRESS_SIZE + 34)

#define EH_ORIGINS      0x0008  /* CONFIG.OriginCnt * <OriginLines> */

typedef struct
{
    char line[62];
}
OriginLines;

#define EH_PACKROUTE    0x0009  /* CONFIG.RouteCnt * <PackRoute> */

typedef struct
{
    FEAddress dest;
    FEAddress routes[MAX_ROUTE];
}
PackRoute;

#define EH_PACKERS      0x000A  /* CONFIG.Packers * <Packers> (DOS)  */
#define EH_PACKERS2     0x100A  /* CONFIG.Packers * <Packers> (OS/2) */

typedef struct
{
    char tag[6];
    char command[_MAXPATH];
    char list[4];
    unsigned char ratio;
    unsigned char resv[7];
}
Packers;
#define FE_PACKERS_SIZE 74

#define EH_UNPACKERS    0x000B  /* CONFIG.Unpackers * <Unpackers> (DOS)  */
#define EH_UNPACKERS2   0x100B  /* CONFIG.Unpackers * <Unpackers> (OS/2) */

/* Calling convention:                                      *
 * 0 = change path to inbound directory, 1 = <path> *.PKT,  *
 * 2 = *.PKT <path>, 3 = *.PKT #<path>, 4 = *.PKT -d <path> */

typedef struct
{
    char command[_MAXPATH];
    unsigned char callingconvention;
    unsigned char resv[7];
}
Unpackers;

#define FE_UNPACKERS_SIZE sizeof(Unpackers)

#define EH_RA111_MSG    0x0100  /* Original records of BBS systems */
#define EH_QBBS_MSG     0x0101
#define EH_SBBS_MSG     0x0102
#define EH_TAG_MSG      0x0104
#define EH_RA200_MSG    0x0105
#define EH_PB200_MSG    0x0106  /* See BBS package's documentation */
#define EH_PB202_MSG    0x0107  /* for details                     */
#define EH_MAX202_MSG   0x0108


/********************************************************
 * Routines to access Node.areas, Node.groups           *
 ********************************************************/

#ifdef INC_FE_BAMPROCS

unsigned short AddBam(unsigned char * bam, unsigned short nr)
{
    unsigned char c, d;
    c = (unsigned char)(1 << (7 - (nr & 7)));
    d = bam[nr / 8] & c;
    bam[nr / 8] |= c;
    return d;
}

void FreeBam(unsigned char * bam, unsigned short nr)
{
    bam[nr / 8] &= ~(1 << (7 - (nr & 7)));
}

unsigned short GetBam(unsigned char * bam, unsigned short nr)
{
    if (bam[nr / 8] & (1 << (7 - (nr & 7))))
    {
        return TRUE;
    }
    return FALSE;
}

#define IsActive(nr,area)      GetBam(Node[nr].areas,area)
#define SetActive(nr,area)     AddBam(Node[nr].areas,area)
#define SetDeActive(nr,area)   FreeBam(Node[nr].areas,area)

#endif

/********************************************************
 * FASTECHO.DAT = <STATHEADER>                          *
 *                + <STATHEADER.NodeCnt * StatNode>     *
 *                + <STATHEADER.AreaCnt * StatArea>     *
 ********************************************************/

#define STAT_REVISION       3   /* current revision */

struct fe_date                  /* Used in FASTECHO.DAT */
{
    unsigned short year;
    unsigned char day;
    unsigned char month;
};

typedef struct
{
    char signature[10];                /* contains 'FASTECHO\0^Z' */
    unsigned short revision;
    struct fe_date lastupdate;         /* last time file was updated */
    unsigned short NodeCnt, AreaCnt;
    unsigned long startnode, startarea;/* unix timestamp of last reset */
    unsigned short NodeSize, AreaSize; /* size of StatNode and StatArea records */
    char resv[32];
}
STATHEADER;

typedef struct
{
    FEAddress adr;
    unsigned long n_import, n_export;
    struct fe_date lastimport, lastexport;
    unsigned long dupes;
    unsigned long importbytes, exprotbytes;
}
StatNode;

typedef struct
{
    unsigned short conference;         /* conference # of area */
    unsigned long tagcrc;              /* CRC32 of area tag    */
    unsigned long n_import, n_export;
    struct fe_date lastimport, lastexport;
    unsigned long dupes;
}
StatArea;

/* ======================================================================= */
/* Functions for reading some of these structs in a portable manner        */
/* ======================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

int read_fe_config(CONFIG *c, FILE *fp);
int read_fe_extension_header(ExtensionHeader *h, FILE *fp);
int read_fe_sysaddress(SysAddress *a, FILE *fp);
int read_fe_area(Area *a, FILE *fp);

                                /* length is the length as announced by the
                                   extension header */
int read_fe_node(Node *n, FILE *fp, size_t length);
void free_fe_node(Node *n);
int read_fe_groupdefaults(GroupDefaults *g, FILE *fp, size_t length);
void free_fe_groupdefaults(GroupDefaults *g);

int read_fe_packers(Packers *p, FILE *fp);
int read_fe_unpackers(Unpackers *p, FILE *fp);
int read_fe_frequest(ForwardAreaFix *f, FILE *fp);

#ifdef __cplusplus
}
#endif

#endif
