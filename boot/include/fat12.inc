BS_OEMNAME			DB	'HelloKOS'		;
BPB_BytsPerSec		DW	512				;一个扇区多少字节
BPB_SecPerClus		DB	1				;一个簇多少扇区
BPB_RsvdSecCnt		DW	1				;Boot记录占多少扇区
BPB_NumFATs			DB	2				;一共有多少FAT表
BPB_RootEntCnt		DW	224				;根目录文件最大数
BPB_TotSec16		DW	2880			;逻辑扇区总数
BPB_Media			DB	0xF0			;媒体描述符号
BPB_FATSz16			DW	9				;每个FAT所占扇区数
BPB_SecPerTrk		DW	18				;每个磁道扇区数
BPB_NumHeads		DW	2				;磁头数
BPB_HiddSec			DD	0				;隐藏扇区数
BPB_TotSec32		DD	0				;
BS_DrvNum			DB	0
BS_Reservedl		DB	0
BS_BootSig			DB	29h				;
BS_VolID			DD	0				;
BS_VolLab			DB 	'KinderRiven'	;卷标
BS_FileSysType		DB	'FAT12   '		;文件系统类型

FATSz					equ		9
RootDirSectors			equ		14
SectorNoOfRootDirectory	equ		19
SectorNoOfFAT1			equ		1
DeltaSectorNo			equ		17
