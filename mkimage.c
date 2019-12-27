#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#define IH_OS_INVALID		0	/* Invalid OS	*/
#define IH_OS_OPENBSD		1	/* OpenBSD	*/
#define IH_OS_NETBSD		2	/* NetBSD	*/
#define IH_OS_FREEBSD		3	/* FreeBSD	*/
#define IH_OS_4_4BSD		4	/* 4.4BSD	*/
#define IH_OS_LINUX		5	/* Linux	*/
#define IH_OS_SVR4		6	/* SVR4		*/
#define IH_OS_ESIX		7	/* Esix		*/
#define IH_OS_SOLARIS		8	/* Solaris	*/
#define IH_OS_IRIX		9	/* Irix		*/
#define IH_OS_SCO		10	/* SCO		*/
#define IH_OS_DELL		11	/* Dell		*/
#define IH_OS_NCR		12	/* NCR		*/
#define IH_OS_LYNXOS		13	/* LynxOS	*/
#define IH_OS_VXWORKS		14	/* VxWorks	*/
#define IH_OS_PSOS		15	/* pSOS		*/
#define IH_OS_QNX		16	/* QNX		*/
#define IH_OS_U_BOOT		17	/* Firmware	*/
#define IH_OS_RTEMS		18	/* RTEMS	*/
#define IH_OS_ARTOS		19	/* ARTOS	*/
#define IH_OS_UNITY		20	/* Unity OS	*/
#define IH_OS_INTEGRITY		21	/* INTEGRITY	*/
#define IH_OS_OSE		22	/* OSE		*/
#define IH_OS_PLAN9		23	/* Plan 9	*/

/*
 * CPU Architecture Codes (supported by Linux)
 */
#define IH_ARCH_INVALID		0	/* Invalid CPU	*/
#define IH_ARCH_ALPHA		1	/* Alpha	*/
#define IH_ARCH_ARM		2	/* ARM		*/
#define IH_ARCH_I386		3	/* Intel x86	*/
#define IH_ARCH_IA64		4	/* IA64		*/
#define IH_ARCH_MIPS		5	/* MIPS		*/
#define IH_ARCH_MIPS64		6	/* MIPS	 64 Bit */
#define IH_ARCH_PPC		7	/* PowerPC	*/
#define IH_ARCH_S390		8	/* IBM S390	*/
#define IH_ARCH_SH		9	/* SuperH	*/
#define IH_ARCH_SPARC		10	/* Sparc	*/
#define IH_ARCH_SPARC64		11	/* Sparc 64 Bit */
#define IH_ARCH_M68K		12	/* M68K		*/
#define IH_ARCH_MICROBLAZE	14	/* MicroBlaze   */
#define IH_ARCH_NIOS2		15	/* Nios-II	*/
#define IH_ARCH_BLACKFIN	16	/* Blackfin	*/
#define IH_ARCH_AVR32		17	/* AVR32	*/
#define IH_ARCH_ST200	        18	/* STMicroelectronics ST200  */
#define IH_ARCH_SANDBOX		19	/* Sandbox architecture (test only) */
#define IH_ARCH_NDS32	        20	/* ANDES Technology - NDS32  */
#define IH_ARCH_OPENRISC        21	/* OpenRISC 1000  */
#define IH_ARCH_ARM64		22	/* ARM64	*/
#define IH_ARCH_ARC		23	/* Synopsys DesignWare ARC */

#define IH_TYPE_INVALID		0	/* Invalid Image		*/
#define IH_TYPE_STANDALONE	1	/* Standalone Program		*/
#define IH_TYPE_KERNEL		2	/* OS Kernel Image		*/
#define IH_TYPE_RAMDISK		3	/* RAMDisk Image		*/
#define IH_TYPE_MULTI		4	/* Multi-File Image		*/
#define IH_TYPE_FIRMWARE	5	/* Firmware Image		*/
#define IH_TYPE_SCRIPT		6	/* Script file			*/
#define IH_TYPE_FILESYSTEM	7	/* Filesystem Image (any type)	*/
#define IH_TYPE_FLATDT		8	/* Binary Flat Device Tree Blob	*/
#define IH_TYPE_KWBIMAGE	9	/* Kirkwood Boot Image		*/
#define IH_TYPE_IMXIMAGE	10	/* Freescale IMXBoot Image	*/
#define IH_TYPE_UBLIMAGE	11	/* Davinci UBL Image		*/
#define IH_TYPE_OMAPIMAGE	12	/* TI OMAP Config Header Image	*/
#define IH_TYPE_AISIMAGE	13	/* TI Davinci AIS Image		*/
#define IH_TYPE_KERNEL_NOLOAD	14	/* OS Kernel Image, can run from any load address */
#define IH_TYPE_PBLIMAGE	15	/* Freescale PBL Boot Image	*/
#define IH_TYPE_MXSIMAGE	16	/* Freescale MXSBoot Image	*/
#define IH_TYPE_GPIMAGE		17	/* TI Keystone GPHeader Image	*/
#define IH_TYPE_ATMELIMAGE	18	/* ATMEL ROM bootable Image	*/

/*
 * Compression Types
 */
#define IH_COMP_NONE		0	/*  No	 Compression Used	*/
#define IH_COMP_GZIP		1	/* gzip	 Compression Used	*/
#define IH_COMP_BZIP2		2	/* bzip2 Compression Used	*/
#define IH_COMP_LZMA		3	/* lzma  Compression Used	*/
#define IH_COMP_LZO		4	/* lzo   Compression Used	*/

#define IH_MAGIC	0x27051956	/* Image Magic Number		*/
#define IH_NMLEN		32	/* Image Name Length		*/

struct image_header {
	unsigned int magic;
	unsigned int hcrc;
	unsigned int time;
	unsigned int size;
	unsigned int load;
	unsigned int ep;
	unsigned int dcrc;
	unsigned char os;
	unsigned char arch;
	unsigned char type;
	unsigned char comp;
	unsigned char name[IH_NMLEN];
};

struct table_entry {
	int	id;
	char	*sname;		/* short (input) name to find table entry */
	char	*lname;		/* long (output) name to print for messages */
};

static int get_os_id(const char * name)
{
	static const struct table_entry uimage_os[] = {
		{	IH_OS_INVALID,	NULL,		"Invalid OS",		},
		{	IH_OS_LINUX,	"linux",	"Linux",		},
		{	IH_OS_LYNXOS,	"lynxos",	"LynxOS",		},
		{	IH_OS_NETBSD,	"netbsd",	"NetBSD",		},
		{	IH_OS_OSE,	"ose",		"Enea OSE",		},
		{	IH_OS_PLAN9,	"plan9",	"Plan 9",		},
		{	IH_OS_RTEMS,	"rtems",	"RTEMS",		},
		{	IH_OS_U_BOOT,	"u-boot",	"U-Boot",		},
		{	IH_OS_VXWORKS,	"vxworks",	"VxWorks",		},
		{	IH_OS_QNX,	"qnx",		"QNX",			},
		{	IH_OS_INTEGRITY,"integrity",	"INTEGRITY",		},
		{	IH_OS_4_4BSD,	"4_4bsd",	"4_4BSD",		},
		{	IH_OS_DELL,	"dell",		"Dell",			},
		{	IH_OS_ESIX,	"esix",		"Esix",			},
		{	IH_OS_FREEBSD,	"freebsd",	"FreeBSD",		},
		{	IH_OS_IRIX,	"irix",		"Irix",			},
		{	IH_OS_NCR,	"ncr",		"NCR",			},
		{	IH_OS_OPENBSD,	"openbsd",	"OpenBSD",		},
		{	IH_OS_PSOS,	"psos",		"pSOS",			},
		{	IH_OS_SCO,	"sco",		"SCO",			},
		{	IH_OS_SOLARIS,	"solaris",	"Solaris",		},
		{	IH_OS_SVR4,	"svr4",		"SVR4",			},
		{	-1,		"",		"",			},
	};
	struct table_entry *table;

	for (table = (struct table_entry *)uimage_os; table->id >= 0; table++) {
		if (table->sname && strcmp(table->sname, name) == 0)
			return table->id;
	}
	return -1;
}
static int get_type_id(const char * name)
{
	static const struct table_entry uimage_type[] = {
		{	IH_TYPE_AISIMAGE,   "aisimage",   "Davinci AIS image",},
		{	IH_TYPE_FILESYSTEM, "filesystem", "Filesystem Image",	},
		{	IH_TYPE_FIRMWARE,   "firmware",	  "Firmware",		},
		{	IH_TYPE_FLATDT,     "flat_dt",    "Flat Device Tree",	},
		{	IH_TYPE_GPIMAGE,    "gpimage",    "TI Keystone SPL Image",},
		{	IH_TYPE_KERNEL,	    "kernel",	  "Kernel Image",	},
		{	IH_TYPE_KERNEL_NOLOAD, "kernel_noload",  "Kernel Image (no loading done)", },
		{	IH_TYPE_KWBIMAGE,   "kwbimage",   "Kirkwood Boot Image",},
		{	IH_TYPE_IMXIMAGE,   "imximage",   "Freescale i.MX Boot Image",},
		{	IH_TYPE_INVALID,    NULL,	  "Invalid Image",	},
		{	IH_TYPE_MULTI,	    "multi",	  "Multi-File Image",	},
		{	IH_TYPE_OMAPIMAGE,  "omapimage",  "TI OMAP SPL With GP CH",},
		{	IH_TYPE_PBLIMAGE,   "pblimage",   "Freescale PBL Boot Image",},
		{	IH_TYPE_RAMDISK,    "ramdisk",	  "RAMDisk Image",	},
		{	IH_TYPE_SCRIPT,     "script",	  "Script",		},
		{	IH_TYPE_STANDALONE, "standalone", "Standalone Program", },
		{	IH_TYPE_UBLIMAGE,   "ublimage",   "Davinci UBL image",},
		{	IH_TYPE_MXSIMAGE,   "mxsimage",   "Freescale MXS Boot Image",},
		{	IH_TYPE_ATMELIMAGE, "atmelimage", "ATMEL ROM-Boot Image",},
		{	-1,		    "",		  "",			},
	};
	struct table_entry *table;

	for (table = (struct table_entry *)uimage_type; table->id >= 0; table++) {
		if (table->sname && strcmp(table->sname, name) == 0)
			return table->id;
	}
	return -1;
}
static int get_comp_id(const char * name)
{
	static const struct table_entry uimage_comp[] = {
		{	IH_COMP_NONE,	"none",		"uncompressed",		},
		{	IH_COMP_BZIP2,	"bzip2",	"bzip2 compressed",	},
		{	IH_COMP_GZIP,	"gzip",		"gzip compressed",	},
		{	IH_COMP_LZMA,	"lzma",		"lzma compressed",	},
		{	IH_COMP_LZO,	"lzo",		"lzo compressed",	},
		{	-1,		"",		"",			},
	};
	struct table_entry *table;

	for (table = (struct table_entry *)uimage_comp; table->id >= 0; table++) {
		if (table->sname && strcmp(table->sname, name) == 0)
			return table->id;
	}
	return -1;
}

static int get_arch_id(const char * name)
{
	static const struct table_entry uimage_arch[] = {
		{	IH_ARCH_INVALID,	NULL,		"Invalid ARCH",	},
		{	IH_ARCH_ALPHA,		"alpha",	"Alpha",	},
		{	IH_ARCH_ARM,		"arm",		"ARM",		},
		{	IH_ARCH_I386,		"x86",		"Intel x86",	},
		{	IH_ARCH_IA64,		"ia64",		"IA64",		},
		{	IH_ARCH_M68K,		"m68k",		"M68K",		},
		{	IH_ARCH_MICROBLAZE,	"microblaze",	"MicroBlaze",	},
		{	IH_ARCH_MIPS,		"mips",		"MIPS",		},
		{	IH_ARCH_MIPS64,		"mips64",	"MIPS 64 Bit",	},
		{	IH_ARCH_NIOS2,		"nios2",	"NIOS II",	},
		{	IH_ARCH_PPC,		"powerpc",	"PowerPC",	},
		{	IH_ARCH_PPC,		"ppc",		"PowerPC",	},
		{	IH_ARCH_S390,		"s390",		"IBM S390",	},
		{	IH_ARCH_SH,		"sh",		"SuperH",	},
		{	IH_ARCH_SPARC,		"sparc",	"SPARC",	},
		{	IH_ARCH_SPARC64,	"sparc64",	"SPARC 64 Bit",	},
		{	IH_ARCH_BLACKFIN,	"blackfin",	"Blackfin",	},
		{	IH_ARCH_AVR32,		"avr32",	"AVR32",	},
		{	IH_ARCH_NDS32,		"nds32",	"NDS32",	},
		{	IH_ARCH_OPENRISC,	"or1k",		"OpenRISC 1000",},
		{	IH_ARCH_SANDBOX,	"sandbox",	"Sandbox",	},
		{	IH_ARCH_ARM64,		"arm64",	"AArch64",	},
		{	IH_ARCH_ARC,		"arc",		"ARC",		},
		{	-1,			"",		"",		},
	};
	struct table_entry *table;

	for (table = (struct table_entry *)uimage_arch; table->id >= 0; table++) {
		if (table->sname && strcmp(table->sname, name) == 0)
			return table->id;
	}
	return -1;
}

void hexdump(unsigned int *buf, int len, int offset)
{
	int i, j, quotient, remainder;

	if (!buf)
		return;
	quotient = len / 8;
	remainder = len % 8;
	for (i = 0; i < quotient; i++) {
		printf("0x%08x:\t", offset);
		for (j = 0; j < 8; j++)
			printf("0x%08x ", buf[i * 8 + j]);
		printf("\n");
		offset += 8;
	}
	if (remainder) {
		printf("0x%08x:\t", offset);
		for (i = 0; i < remainder; i++)
			printf("0x%08x ", buf[quotient * 8 + i]);
		printf("\n");
	}
}

unsigned int *get_or_make_crc32_table(void)
{
	static int mkflag = 0;
	static char p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};
	static unsigned int crc32_table[256];
	unsigned int i, j, poly, crc32;

	if (!mkflag){
		poly = 0;
		for (i = 0; i < sizeof(p)/sizeof(p[0]); i++)
			poly |= 1 << (31 - p[i]);
		for (i = 0; i < 256; i++) {
			crc32 = i;
			for (j = 0; j < 8; j++)
				crc32 = (crc32 & 0x1) ? (crc32 >> 1) ^ poly : (crc32 >> 1);
			crc32_table[i] = htole32(crc32);
		}
		mkflag = 1;
	}
	return crc32_table; 
}

unsigned int crc32_no_comp(unsigned int crc32, const char *buf, unsigned int len)
{
	unsigned int i, *table;

	table = get_or_make_crc32_table();

	for (i = 0; i < len; i++)
#if __BYTE_ORDER == __LITTLE_ENDIAN
		crc32 = (crc32 >> 8) ^ table[(crc32 ^ buf[i]) & 0xff];
#else
		crc32 = (crc32 << 8) ^ table[((crc32 >> 24)^ buf[i]) & 0xff];
#endif
	return crc32;
}

static void usage(void)
{
	printf("usage: mkimage [ops] <image>\n");
}

int main(int argc, char *argv[])
{
	char name[IH_NMLEN], sfilename[512], dfilename[512], *buf;
	int os, arch, type, comp, rlen, wlen;
	FILE *sfp, *dfp;
	struct image_header header;
	struct stat st;
	unsigned int addr, ep, crc32;


	while (--argc > 0 && **++argv == '-') {
		while (*++*argv) {
			printf("argv: %s\t", *argv);
			switch (**argv) {
			case 'a':
				if (--argc < 0) {
					usage();
					return -1;
				}
				addr = strtoul(*++argv, NULL, 16);
				printf("addr: 0x%x\n", addr);
				goto NEXTARG;
			case 'e':
				if (--argc < 0) {
					usage();
					return -1;
				}
				ep = strtoul(*++argv, NULL, 16);
				printf("ep: 0x%x\n", ep);
				goto NEXTARG;
			case 'O':
				if (--argc < 0) {
					usage();
					return -1;
				}
				os = get_os_id(*++argv);
				printf("os: 0x%x\n", os);
				goto NEXTARG;
			case 'A':
				if (--argc < 0) {
					usage();
					return -1;
				}
				arch = get_arch_id(*++argv);
				printf("arch: 0x%x\n", arch);
				goto NEXTARG;
			case 'T':
				if (--argc < 0) {
					usage();
					return -1;
				}
				type = get_type_id(*++argv);
				printf("type: 0x%x\n", type);
				goto NEXTARG;
			case 'C':
				if (--argc < 0) {
					usage();
					return -1;
				}
				comp = get_comp_id(*++argv);
				printf("comp: 0x%x\n", comp);
				goto NEXTARG;
			case 'n':
				if (--argc < 0) {
					usage();
					return -1;
				}
				memset(name, 0, sizeof(name));
				strncat(name, *++argv, sizeof(name));
				printf("name: %s\n", name);
				goto NEXTARG;
			case 'd':
				if (--argc < 0) {
					usage();
					return -1;
				}
				memset(sfilename, 0, sizeof(sfilename));
				strncat(sfilename, *++argv, sizeof(sfilename));
				if (--argc < 0) {
					usage();
					return -1;
				}
				memset(dfilename, 0, sizeof(dfilename));
				strncat(dfilename, *++argv, sizeof(dfilename));
				printf("sfilename:  %s\tdfilename: %s\n", sfilename, dfilename);
				goto NEXTARG;	
			}
		}
	NEXTARG: ;
	}

	sfp = fopen(sfilename, "rb");
	if (!sfp) {
		printf("failed to open %s\n", sfilename);
		return -1;
	}
	if (fstat(fileno(sfp), &st) < 0) {
		printf("failed to stat %s\n", sfilename);
		return -1;
	}
	memset(&header, 0, sizeof(struct image_header));
	header.magic = htobe32(IH_MAGIC);
	header.hcrc = htobe32(0);
	header.time = htobe32(time(NULL));
	header.size = htobe32(st.st_size);
	header.load = htobe32(addr);
	header.ep = htobe32(ep);
	buf = (char *)malloc(1024 *32);
	if (!buf) {
		printf("failed to malloc\n");
		return -1;
	}
	crc32 = 0xffffffffu;
	do {
		rlen = fread(buf, 1, 1024 * 32, sfp);
		crc32 = crc32_no_comp(crc32, buf, rlen);
	} while (rlen == 1024 * 32);
	crc32 ^= 0xffffffffu;
	header.dcrc = htobe32(crc32);
	header.os = os;
	header.arch = arch;
	header.type = type;
	header.comp = comp;
	memcpy(header.name, name, IH_NMLEN);
	crc32 = 0xffffffffu;
	crc32 = crc32_no_comp(crc32, (char *)(&header), sizeof(struct image_header));
	crc32 ^= 0xffffffffu;
	header.hcrc = htobe32(crc32);

	dfp = fopen(dfilename, "wb+");
	if (!dfp) {
		printf("failed to open %s\n", dfilename);
		free(buf);
		return -1;
	}
	wlen = fwrite(&header, 1, sizeof(struct image_header), dfp);
	if (wlen != sizeof(struct image_header)) {
		printf("failed to write %s %d\n", dfilename, wlen);
		free(buf);
		return -1;
	}
	if (fseek(sfp, 0, SEEK_SET) < 0) {
		printf("failed to seek %s\n", sfilename);
		free(buf);
		return -1;
	}
	do {
		rlen = fread(buf, sizeof(char), 1024 * 32, sfp);
		wlen = fwrite(buf, sizeof(char), rlen, dfp);
		if (wlen != rlen) {
			printf("failed to write %s %d %d\n", dfilename, rlen, wlen);
			free(buf);
			return -1;
		}
	}while(wlen == 1024 * 32);
	fsync(fileno(dfp));
	fclose(dfp);
	fclose(sfp);
	printf("finish\n");
	return 0;
}