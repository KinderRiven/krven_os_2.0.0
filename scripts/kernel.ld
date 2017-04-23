ENTRY(_start)
SECTIONS
{
	. = 0x30400;
	.text :
	{
		*(.text)
	 	. = ALIGN(4096);
	}
	.data :
	{
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}
	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}
	.stab :
	{
		*(.stab)
		. = ALIGN(4096);
	}
	.stabstr :
	{
		*(.stab)
		. = ALIGN(4096);
	}
	
	/DISCARD/ : { *(.comment) *(.eh_frame) }
}

