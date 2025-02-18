#pragma once




static void biosmap()
{
	uint32_t value = 0x000400;

//e9_printf("Printing data from %x", value);
/*	e9_printf("com1 %x: %d", &value->com1, *value->com1);
	e9_printf("com2 %x: %d", &value->com2, value->com2);
	e9_printf("com3 %x: %d", &value->com3, value->com3);
	e9_printf("com4 %x: %d", &value->com4, value->com4);
	e9_printf("lpt1 %x: %d", &value->lpt1, value->lpt1);
	e9_printf("lpt2 %x: %d", &value->lpt2, value->lpt2);
	e9_printf("lpt3 %x: %d", &value->lpt3, value->lpt3);
	e9_printf("ebdabase %x: %x", &value->ebdabase, value->ebdabase);
	e9_printf("bitflags %x: %x", &value->bitflags, value->bitflags);
	e9_printf("ebdamem kb %x: %d", &value->ebdamem,  value->ebdamem);
	e9_printf("kbstate %x: %d", &value->keyboardstate, value->keyboardstate);
	
	e9_printf("kbbuffer %x", &value->keyboardbuffer);
	e9_printf("displaymode %x: %d", &value->displaymode, value->displaymode);

	e9_printf("columnstext %x: %d", &value->columnstext, value->columnstext);
	e9_printf("baseiovideo %x: %d", &value->baseiovideo, value->baseiovideo);
*/
	
	uint16_t valuer = value+0x075;
	

	//e9_printf("ticksboot %x: %d", value+0x075, valuer);

	
	

}