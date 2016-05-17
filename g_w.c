//this example code shows how to put some text in nametable
//it assumes that you have ASCII-encoded font in the CHR tiles $00-$3f
//it also shows how to detect PAL/NTSCvideo system

#include "neslib.h"
#include "g_w.h"

#define MAP_WDT			16
#define MAP_WDT_BIT		4
#define MAP_HGT			13


static unsigned int i,j,k;
static unsigned int pos_lapin,pos_lapin_old, wait, wait_max, tir_x, tir_y,tir_y_old, score;
static unsigned int tir_oeuf_x, tir_oeuf_y,tir_oeuf_y_old,hit_lapin;
static unsigned char pad, move;

//put a string into the nametable

static unsigned char list[3*4*3*2+1];
// structure de la liste :
// lapin : 4 tules x (x,y,id_tule)
// oeuf 1 : 4 tules x (x,y,id_tule)
// oeuf 2 : 4 tules x (x,y,id_tule)

// on verra plus tard
// tir : 4 tules x (x,y,id_tule)

const unsigned char palBackground[16]={ 0x0f,0x38,0x17,0x28,0x0f,0x18,0x28,0x38,0x0f,0x28,0x11,0x38,0x0f,0x1c,0x2c,0x3c };



static unsigned int oeuf[6]={6,6,1,6,6,1};


//init data for the update list, it contains MSB and LSB of a tile address
//in the nametable, then the tile number


void put_str(unsigned int adr,const char str)
{
	vram_adr(adr);
	vram_put(str);
}

void place_lapin(unsigned int pos_lapin)
{
	list[0]= MSB(NTADR_A(10+(pos_lapin*2),16));
	list[1]= LSB(NTADR_A(10+(pos_lapin*2),16));
	list[3]= MSB(NTADR_A(11+(pos_lapin*2),16));
	list[4]= LSB(NTADR_A(11+(pos_lapin*2),16));
	list[6]= MSB(NTADR_A(10+(pos_lapin*2),17));
	list[7]= LSB(NTADR_A(10+(pos_lapin*2),17));
	list[9]= MSB(NTADR_A(11+(pos_lapin*2),17));
	list[10]= LSB(NTADR_A(11+(pos_lapin*2),17));
	list[2]=0x00;
	list[5]=0x01;
	list[8]=0x02;
	list[11]=0x03;
} 

void efface_lapin(unsigned int pos_lapin)
{
	list[12]= MSB(NTADR_A(10+(pos_lapin*2),16));
	list[13]= LSB(NTADR_A(10+(pos_lapin*2),16));
	list[15]= MSB(NTADR_A(11+(pos_lapin*2),16));
	list[16]= LSB(NTADR_A(11+(pos_lapin*2),16));
	list[18]= MSB(NTADR_A(10+(pos_lapin*2),17));
	list[19]= LSB(NTADR_A(10+(pos_lapin*2),17));
	list[21]= MSB(NTADR_A(11+(pos_lapin*2),17));
	list[22]= LSB(NTADR_A(11+(pos_lapin*2),17));
	list[14]=0x1c;
	list[17]=0x09;
	list[20]=0x1d;
	list[23]=0x1e;
}

void place_oeuf(unsigned int pos,unsigned int num)
{
	list[24+(3*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[25+(3*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[27+(3*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[28+(3*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[30+(3*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[31+(3*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[33+(3*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[34+(3*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[26+(3*num)]=0x0c;
	list[29+(3*num)]=0x0d;
	list[32+(3*num)]=0x0e;
	list[35+(3*num)]=0x0f;
} 

void efface_oeuf(unsigned int pos,unsigned int num)
{
	list[36+(3*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[37+(3*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[39+(3*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[40+(3*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[42+(3*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[43+(3*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[45+(3*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[46+(3*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[38+(3*num)]=0x1f;
	list[41+(3*num)]=0x20;
	list[44+(3*num)]=0x21;
	list[47+(3*num)]=0x22;
} 



void main(void)
{
	//rendering is disabled at the startup, the palette is all black

	//pal_col(1,0x30);//set while color
	pal_bg(palBackground);

	//memcpy(list,list_init,sizeof(list_init));
	k = 0;
	for(i=0;i<24;i++)
	{
		list[i*3] = MSB(NTADR_A(0,0));
		list[i*3+1] = LSB(NTADR_A(0,0));
		list[i*3+2] = 0x27;
	}
	
	list[3*4*3*2] = NT_UPD_EOF;
	
	set_vram_update(list);
	
	pos_lapin = 0;
	pos_lapin_old = pos_lapin;
	tir_x = 0;
	tir_y = 3;
	tir_y_old = tir_y;
	tir_oeuf_x = 0;
	tir_oeuf_y = 0;
	tir_oeuf_y_old = tir_oeuf_y;
	wait = 0;
	move = 0;
	wait_max = 60;
	score = 0;
	
	// set le premier lapin
	place_lapin(pos_lapin);
	
	vram_adr(NAMETABLE_A);//set VRAM address
	vram_unrle(g_w);
	ppu_on_all();//enable rendering
	while(1)
	{
		ppu_wait_frame();
		pad=pad_poll(0);
		wait++;

		if(pad&PAD_LEFT && pos_lapin>  0 && move ==0&&hit_lapin==0)
		{
			pos_lapin--;
			move =1;
		}
		if(pad&PAD_RIGHT && pos_lapin<  5&& move ==0&&hit_lapin==0)
		{
			pos_lapin++;
			move =1;
		}
		if(pad&PAD_A && tir_y ==3&&hit_lapin==0)
		{
			tir_x = pos_lapin;
			tir_y--;
			wait =0;
		}
		if(pad==0)move =0;
		
		
		 if(wait>wait_max)
		{
			// if(tir_y < 3) tir_y--;
			// if(tir_oeuf_y >0) tir_oeuf_y++;
			for(i=0;i<2;i++)
			{
				// déplacements oeufs
				if(oeuf[i*3]==0&&oeuf[i*3+2]==1)oeuf[i*3+2]=2;
				if(oeuf[i*3]==5&&oeuf[i*3+2]==2)oeuf[i*3+2]=1;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==1)oeuf[i*3]--;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==2)oeuf[i*3]++;
				if(6==oeuf[i*3]&&(rand8()%4)==1)oeuf[i*3]--;
				if(6==oeuf[i*3])oeuf[i*3]--;
				// if(tir_oeuf_y==0&&oeuf[i*3]!=6&&(rand8()%4)==1)
				// {
					// tir_oeuf_y++;
					// tir_oeuf_x= oeuf[i*3];
				// }
			}
			wait =0;
		}
		// if(tir_y == 0)
		// {
			// list[(tir_x+6)*3+2]=0x0B;
			// for(i=0;i<2;i++)
			// {	
				// if(oeuf[i*3]==tir_x)
				// {
					// oeuf[i*3+2]=3;
					// score++;
					// if(wait_max>10)wait_max-=2;
				// }
			// }			
			// tir_y =3;
			// tir_y_old =tir_y;
		// }
		// if(tir_oeuf_y == 3)
		// {
			// list[(tir_oeuf_x+12)*3+2]=0x0B;
			// if(tir_oeuf_x==pos_lapin)
			// {
				// hit_lapin=1;
				// wait_max =60;
			// }
			// tir_oeuf_y=0;
		// }
		
		// déplacements lapin
		if(pos_lapin!=pos_lapin_old)
		{
			efface_lapin(pos_lapin_old);
			place_lapin(pos_lapin);
			pos_lapin_old=pos_lapin;
		}
		// if(tir_y!=tir_y_old)
		// {
			// list[(tir_x+(tir_y*6))*3+2]=0x05;
			// if(tir_y <2)list[(tir_x+(tir_y_old*6))*3+2]=0x0B;
			// tir_y_old=tir_y;
		// }
		// if(tir_oeuf_y!=tir_oeuf_y_old)
		// {
			// if(tir_oeuf_y>0)list[(tir_oeuf_x+(tir_oeuf_y*6))*3+2]=0x06;
			// if(tir_oeuf_y_old>0)list[(tir_oeuf_x+(tir_oeuf_y_old*6))*3+2]=0x0B;
			// tir_oeuf_y_old=tir_oeuf_y;
		// }
		// déplacements oeufs
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3]!=oeuf[i*3+1])
			{
				place_oeuf(oeuf[i*3],i);
				// list[oeuf[i*3]*3+2]=0x02;
				if(oeuf[i*3+1]!=6) efface_oeuf(oeuf[i*3+1],i);
				// list[oeuf[i*3+1]*3+2]=0x08;
				oeuf[i*3+1]=oeuf[i*3];
			}
		}
		// for(i=0;i<2;i++)
		// {
			// if(oeuf[i*3+2]==3&&(wait%2)==0)
			// {
				// list[oeuf[i*3]*3+2]=0x02;
				// oeuf[i*3+2]=4;
			// }
			// else if(oeuf[i*3+2]==4&&(wait%2)==0)
			// {
				// list[oeuf[i*3]*3+2]=0x04;
				// oeuf[i*3+2]=3;
			// }
			// if(oeuf[i*3+2]>2&&wait==wait_max)
			// {
				// list[oeuf[i*3]*3+2]=0x08;
				// oeuf[i*3]=6;
				// oeuf[i*3+1]=6;
				// oeuf[i*3+2]=1;
			// }
		// }
		// if(hit_lapin==1&&(wait%2)==0)
		// {
			// list[(pos_lapin+18)*3+2]=0x03;
			// hit_lapin=2;
		// }
		// else if(hit_lapin==2&&(wait%2)==0)
		// {
			// list[(pos_lapin+18)*3+2]=0x01;
			// hit_lapin=1;
		// }
		// if(hit_lapin>0&&wait==wait_max)
		// {
			// list[(pos_lapin+18)*3+2]=0x07;
			// list[18*3+2]=0x01;
			// pos_lapin=0;
			// pos_lapin_old=0;
			// hit_lapin = 0;
		// }
	}

}