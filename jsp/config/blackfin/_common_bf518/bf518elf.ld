/*
* ADSP-BF518 Rev 0.0ではアノーマリ05000444の影響でこのファイルを使用できない。
* Rev 0.0 に対しては bf518elf_00.ldを使用すること。
*/
OUTPUT_FORMAT("elf32-bfin","elf32-bfin","elf32-bfin")
OUTPUT_ARCH(bfin)

PROVIDE (ldf_stack_end = 0xffb00FFC ) ;
PROVIDE (hardware_init_hook = 0) ;
PROVIDE (software_init_hook = 0) ;
PROVIDE (software_term_hook = 0) ;
PROVIDE (__kernel_boot_for_gdb = 0);

ENTRY (start);

MEMORY
  {
    MEM_SDRAM (XW)    : ORIGIN = 0x00000000, LENGTH = 128M
    MEM_ASYNC_0 (XW)  : ORIGIN = 0x20000000, LENGTH = 1M
    MEM_ASYNC_1 (XW)  : ORIGIN = 0x20100000, LENGTH = 1M
    MEM_ASYNC_2 (XW)  : ORIGIN = 0x20200000, LENGTH = 1M
    MEM_ASYNC_3 (XW)  : ORIGIN = 0x20300000, LENGTH = 1M

    MEM_L1_DATA_A (W!X)  : ORIGIN = 0xff800000, LENGTH = 32K
    MEM_L1_DATA_B (W!X)  : ORIGIN = 0xff900000, LENGTH = 16K	/* BF518のバンクBは32kBだが、キャッシュ用に16kBあけておく */
    MEM_L1_CODE (XR)   : ORIGIN = 0xffa00000, LENGTH = 32K
/*  MEM_L1_INST_C (XR)   : ORIGIN = 0xffa10000, LENGTH = 16K	BF518のバンクCは16kBだが、キャッシュ用にあけておく */
    MEM_L1_SCRATCH (W!X) : ORIGIN = 0xffb00000, LENGTH = 4K

    MEM_L2				: ORIGIN = 0xffe00000, LENGTH = 0	/* DUMMY */
    
    MEM_SYSMMR (W!X)  : ORIGIN = 0xffc00000, LENGTH = 2M
    MEM_COREMMR (W!X) : ORIGIN = 0xffe00000, LENGTH = 2M
  }

SECTIONS
{

/* Read-only sections, merged into text segment: */
  .interp         : { *(.interp) }
  .hash           : { *(.hash) }
  .dynsym         : { *(.dynsym) }
  .dynstr         : { *(.dynstr) }
  .gnu.version    : { *(.gnu.version) }
  .gnu.version_d  : { *(.gnu.version_d) }
  .gnu.version_r  : { *(.gnu.version_r) }
  .rel.init       : { *(.rel.init) }
  .rela.init      : { *(.rela.init) }
  .rel.text       : { *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*) }
  .rela.text      : { *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) }
  .rel.fini       : { *(.rel.fini) }
  .rela.fini      : { *(.rela.fini) }
  .rel.rodata     : { *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*) }
  .rela.rodata    : { *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) }
  .rel.data.rel.ro   : { *(.rel.data.rel.ro* .rel.gnu.linkonce.d.rel.ro.*) }
  .rela.data.rel.ro   : { *(.rela.data.rel.ro* .rela.gnu.linkonce.d.rel.ro.*) }
  .rel.data       : { *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*) }
  .rela.data      : { *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) }
  .rel.tdata	  : { *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*) }
  .rela.tdata	  : { *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) }
  .rel.tbss	  : { *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*) }
  .rela.tbss	  : { *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) }
  .rel.ctors      : { *(.rel.ctors) }
  .rela.ctors     : { *(.rela.ctors) }
  .rel.dtors      : { *(.rel.dtors) }
  .rela.dtors     : { *(.rela.dtors) }
  .rel.got        : { *(.rel.got) }
  .rela.got       : { *(.rela.got) }
  .rel.sdata      : { *(.rel.sdata .rel.sdata.* .rel.gnu.linkonce.s.*) }
  .rela.sdata     : { *(.rela.sdata .rela.sdata.* .rela.gnu.linkonce.s.*) }
  .rel.sbss       : { *(.rel.sbss .rel.sbss.* .rel.gnu.linkonce.sb.*) }
  .rela.sbss      : { *(.rela.sbss .rela.sbss.* .rela.gnu.linkonce.sb.*) }
  .rel.sdata2     : { *(.rel.sdata2 .rel.sdata2.* .rel.gnu.linkonce.s2.*) }
  .rela.sdata2    : { *(.rela.sdata2 .rela.sdata2.* .rela.gnu.linkonce.s2.*) }
  .rel.sbss2      : { *(.rel.sbss2 .rel.sbss2.* .rel.gnu.linkonce.sb2.*) }
  .rela.sbss2     : { *(.rela.sbss2 .rela.sbss2.* .rela.gnu.linkonce.sb2.*) }
  .rel.bss        : { *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*) }
  .rela.bss       : { *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) }
  .rel.plt        : { *(.rel.plt) }
  .rela.plt       : { *(.rela.plt) }
  .l2             :
  {
    *(.l2 .l2.*)
  } >MEM_SDRAM
 
  .start           :
  {
    KEEP (*(.start))
  } >MEM_L1_CODE =0
 
  .text           :
  {
    *(.text .stub .text.* .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
  } >MEM_L1_CODE =0
  
  .init           :
  {
    KEEP (*(.init))
  } >MEM_L1_CODE =0
 
  .plt            : { *(.plt) } >MEM_L1_CODE
 
  .fini           :
  {
    KEEP (*(.fini))
  } >MEM_L1_CODE =0
  
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  
  .rodata         : { *(.rodata .rodata.* .gnu.linkonce.r.*) } >MEM_L1_DATA_A
  .rodata1        : { *(.rodata1) } >MEM_L1_DATA_A
  
  .sdata2         :
  {
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
  } >MEM_L1_DATA_A
  
  .sbss2          : { *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*) } >MEM_L1_DATA_A
  .eh_frame_hdr : { *(.eh_frame_hdr) } >MEM_L1_DATA_A
  .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) } >MEM_L1_DATA_A
  .gcc_except_table   : ONLY_IF_RO { *(.gcc_except_table .gcc_except_table.*) } >MEM_L1_DATA_A
  
  /* Adjust the address for the data segment.  We want to adjust up to
     the same address within the page on the next page up.  */
  . = ALIGN(0x1000) + (. & (0x1000 - 1));
  /* Exception handling  */
  .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) } >MEM_L1_DATA_A
  .gcc_except_table   : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) } >MEM_L1_DATA_A
  /* Thread Local Storage sections  */
  .tdata	  : { *(.tdata .tdata.* .gnu.linkonce.td.*) } >MEM_L1_DATA_A
  .tbss		  : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) } >MEM_L1_DATA_A
  .preinit_array     :
  {
    PROVIDE_HIDDEN (___preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (___preinit_array_end = .);
  } >MEM_L1_DATA_A
  .init_array     :
  {
     PROVIDE_HIDDEN (___init_array_start = .);
     KEEP (*(SORT(.init_array.*)))
     KEEP (*(.init_array))
     PROVIDE_HIDDEN (___init_array_end = .);
  } >MEM_L1_DATA_A
  .fini_array     :
  {
    PROVIDE_HIDDEN (___fini_array_start = .);
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE_HIDDEN (___fini_array_end = .);
  } >MEM_L1_DATA_A

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin*.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*crtend*.o(.ctors))
  } >MEM_L1_DATA_A
  .dtors          :
  {
    KEEP (*crtbegin*.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*crtend*.o(.dtors))
  } >MEM_L1_DATA_A
  .jcr            : { KEEP (*(.jcr)) } >MEM_L1_DATA_A
  .data.rel.ro : { *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*) *(.data.rel.ro* .gnu.linkonce.d.rel.ro.*) } >MEM_L1_DATA_A
  .dynamic        : { *(.dynamic) } >MEM_L1_DATA_A
  .data           :
  {
    *(.data .data.* .gnu.linkonce.d.*)
    KEEP (*(.gnu.linkonce.d.*personality*))
    SORT(CONSTRUCTORS)
  } >MEM_L1_DATA_A
  .data1          : { *(.data1) } >MEM_L1_DATA_A
  .got            : { *(.got.plt) *(.got) } >MEM_L1_DATA_A
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata          :
  {
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } >MEM_L1_DATA_A
  __edata = .; PROVIDE (_edata = .);
  .sbss           :
  {
    __bss_start = .;
    *(.dynsbss)
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  } >MEM_L1_DATA_A
  .bss            :
  {
    *(.dynbss)
    *(.bss .bss.* .gnu.linkonce.b.*)
    *(COMMON)
    /* Align here to ensure that the .bss section occupies space up to
       _end.  Align after .bss to ensure correct alignment even if the
       .bss section disappears because there are no input sections.
       FIXME: Why do we need it? When there is no .bss section, we don't
       pad the .data section.  */
    . = ALIGN(. != 0 ? 32 / 8 : 1);
    __bss_end = .;
  } >MEM_L1_DATA_A
  . = ALIGN(32 / 8);
  . = ALIGN(32 / 8);
  __end = .; PROVIDE (_end = .);
  /* Stabs debugging sections.  */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .stab.excl     0 : { *(.stab.excl) }
  .stab.exclstr  0 : { *(.stab.exclstr) }
  .stab.index    0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment       0 : { *(.comment) }
  /* DWARF debug sections.
     Symbols in the DWARF debugging sections are relative to the beginning
     of the section so we begin them at 0.  */
  /* DWARF 1 */
  .debug          0 : { *(.debug) }
  .line           0 : { *(.line) }
  /* GNU DWARF 1 extensions */
  .debug_srcinfo  0 : { *(.debug_srcinfo) }
  .debug_sfnames  0 : { *(.debug_sfnames) }
  /* DWARF 1.1 and DWARF 2 */
  .debug_aranges  0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  /* DWARF 2 */
  .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev   0 : { *(.debug_abbrev) }
  .debug_line     0 : { *(.debug_line) }
  .debug_frame    0 : { *(.debug_frame) }
  .debug_str      0 : { *(.debug_str) }
  .debug_loc      0 : { *(.debug_loc) }
  .debug_macinfo  0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames  0 : { *(.debug_varnames) }

  __stack_end = ORIGIN(MEM_L1_SCRATCH) + LENGTH(MEM_L1_SCRATCH);

  __heap_start = ORIGIN(MEM_L1_DATA_B);
  __heap_end = ORIGIN(MEM_L1_DATA_B) + LENGTH(MEM_L1_DATA_B);


    PROVIDE (end = .) ;

/* ADSP-BF518依存のSYSTEM MMR
	.bss.sysmmr (NOLOAD) : { . = 0x0000; chip_mmr.o(SYSMMR0)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0100; chip_mmr.o(SYSMMR1)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x010C; chip_mmr.o(SYSMMR2)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x014C; chip_mmr.o(SYSMMR3)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0200; chip_mmr.o(SYSMMR4)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0300; chip_mmr.o(SYSMMR5)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0400; chip_mmr.o(SYSMMR6)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x041C; chip_mmr.o(SYSMMR7)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0424; chip_mmr.o(SYSMMR8)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0500; chip_mmr.o(SYSMMR9)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0600; chip_mmr.o(SYSMMR10)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0700; chip_mmr.o(SYSMMR11)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0800; chip_mmr.o(SYSMMR12)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0818; chip_mmr.o(SYSMMR13)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0820; chip_mmr.o(SYSMMR14)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0900; chip_mmr.o(SYSMMR15)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0918; chip_mmr.o(SYSMMR16)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0920; chip_mmr.o(SYSMMR17)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0A00; chip_mmr.o(SYSMMR18)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0A10; chip_mmr.o(SYSMMR19)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0B0C; chip_mmr.o(SYSMMR20)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C00; chip_mmr.o(SYSMMR21)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C10; chip_mmr.o(SYSMMR22)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C38; chip_mmr.o(SYSMMR23)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C40; chip_mmr.o(SYSMMR24)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C50; chip_mmr.o(SYSMMR25)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C78; chip_mmr.o(SYSMMR26)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C80; chip_mmr.o(SYSMMR27)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0C90; chip_mmr.o(SYSMMR28)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0CB8; chip_mmr.o(SYSMMR29)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0CC0; chip_mmr.o(SYSMMR30)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0CD0; chip_mmr.o(SYSMMR31)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0CF8; chip_mmr.o(SYSMMR32)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D00; chip_mmr.o(SYSMMR33)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D10; chip_mmr.o(SYSMMR34)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D38; chip_mmr.o(SYSMMR35)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D40; chip_mmr.o(SYSMMR36)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D50; chip_mmr.o(SYSMMR37)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D78; chip_mmr.o(SYSMMR38)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D80; chip_mmr.o(SYSMMR39)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0D90; chip_mmr.o(SYSMMR40)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0DB8; chip_mmr.o(SYSMMR41)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0DC0; chip_mmr.o(SYSMMR42)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0DD0; chip_mmr.o(SYSMMR43)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0DF8; chip_mmr.o(SYSMMR44)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E00; chip_mmr.o(SYSMMR45)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E10; chip_mmr.o(SYSMMR46)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E38; chip_mmr.o(SYSMMR47)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E40; chip_mmr.o(SYSMMR48)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E50; chip_mmr.o(SYSMMR49)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E78; chip_mmr.o(SYSMMR50)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E80; chip_mmr.o(SYSMMR51)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0E90; chip_mmr.o(SYSMMR52)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0EB8; chip_mmr.o(SYSMMR53)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0EC0; chip_mmr.o(SYSMMR54)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0ED0; chip_mmr.o(SYSMMR55)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0EF8; chip_mmr.o(SYSMMR56)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F00; chip_mmr.o(SYSMMR57)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F10; chip_mmr.o(SYSMMR58)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F38; chip_mmr.o(SYSMMR59)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F40; chip_mmr.o(SYSMMR60)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F50; chip_mmr.o(SYSMMR61)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F78; chip_mmr.o(SYSMMR62)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F80; chip_mmr.o(SYSMMR63)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0F90; chip_mmr.o(SYSMMR64)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0FB8; chip_mmr.o(SYSMMR65)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0FC0; chip_mmr.o(SYSMMR66)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0FD0; chip_mmr.o(SYSMMR67)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x0FF8; chip_mmr.o(SYSMMR68)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x1000; chip_mmr.o(SYSMMR69)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x1400; chip_mmr.o(SYSMMR70)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x1480; chip_mmr.o(SYSMMR71)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x1500; chip_mmr.o(SYSMMR72)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x1700; chip_mmr.o(SYSMMR73)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x2000; chip_mmr.o(SYSMMR74)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x201C; chip_mmr.o(SYSMMR75)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x2024; chip_mmr.o(SYSMMR76)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3000; chip_mmr.o(SYSMMR77)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x302C; chip_mmr.o(SYSMMR78)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3060; chip_mmr.o(SYSMMR79)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x30A0; chip_mmr.o(SYSMMR80)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3100; chip_mmr.o(SYSMMR81)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3180; chip_mmr.o(SYSMMR82)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3200; chip_mmr.o(SYSMMR83)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3210; chip_mmr.o(SYSMMR84)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3220; chip_mmr.o(SYSMMR85)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3240; chip_mmr.o(SYSMMR86)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3280; chip_mmr.o(SYSMMR87)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3288; chip_mmr.o(SYSMMR88)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3300; chip_mmr.o(SYSMMR89)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3340; chip_mmr.o(SYSMMR90)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3400; chip_mmr.o(SYSMMR91)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3500; chip_mmr.o(SYSMMR92)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3620; chip_mmr.o(SYSMMR93)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3680; chip_mmr.o(SYSMMR94)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3700; chip_mmr.o(SYSMMR95)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3800; chip_mmr.o(SYSMMR96)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3848; chip_mmr.o(SYSMMR97)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x3880; chip_mmr.o(SYSMMR98)}>MEM_SYSMMR
	.bss.sysmmr (NOLOAD) : { . = 0x38C0; chip_mmr.o(SYSMMR99)}>MEM_SYSMMR
*/




/* Blackfin 共有のCORE MMR 
   	.bss.coremmr (NOLOAD) : {                    cpu_mmr.o(DMEMCTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(DCPLBA)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(DCPLBD)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(DTESTC)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(DTESTD)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(IMEMCTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(ICPLBA)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(ICPLBD)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(ITESTC)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(ITESTD)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(EVT)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(CEC)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(CTMR)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000)+0x1000; cpu_mmr.o(DBG)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(TBCTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(TBUF)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(WPICTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0040); cpu_mmr.o(WPIADDR)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0040); cpu_mmr.o(WPICNT)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(WPDCTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0040); cpu_mmr.o(WPDADDR)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0040); cpu_mmr.o(WPDCNT)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(WPSTAT)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x1000); cpu_mmr.o(PFCTL)}> MEM_COREMMR
   	.bss.coremmr (NOLOAD) : { . = ALIGN(0x0100); cpu_mmr.o(PFCNT)}> MEM_COREMMR

*/
    .comment        0 : { *(.comment) }
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
}
