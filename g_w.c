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

static unsigned char list[6*4*3+1];

//const unsigned char palBackground[16]={ 0x0f,0x38,0x17,0x28,0x0f,0x18,0x28,0x38,0x0f,0x19,0x29,0x39,0x0f,0x1c,0x2c,0x3c };
//const unsigned char palette[16]={ 0x0f,0x00,0x10,0x30,0x0f,0x38,0x21,0x28,0x0f,0x28,0x11,0x38,0x0f,0x09,0x19,0x29 };
const unsigned char palBackground[16]={ 0x0f,0x38,0x17,0x28,0x0f,0x18,0x28,0x38,0x0f,0x28,0x11,0x38,0x0f,0x1c,0x2c,0x3c };



static unsigned int oeuf[6]={6,6,1,6,6,1};


//init data for the update list, it contains MSB and LSB of a tile address
//in the nametable, then the tile number


void put_str(unsigned int adr,const char str)
{
	vram_adr(adr);
	vram_put(str);
}



void main(void)
{
	//rendering is disabled at the startup, the palette is all black

	//pal_col(1,0x30);//set while color
	pal_bg(palBackground);

	//memcpy(list,list_init,sizeof(list_init));
	k = 0;
	for(j=0;j<4;j++)
	{
		for(i=0;i<6;i++)
		{
			list[k*3] = MSB(NTADR_A(i+13,j+12));
			list[k*3+1] = LSB(NTADR_A(i+13,j+12));
			list[k*3+2] = 0x0B;
			k++;
		}
	}
	for(i=0;i<6;i++)
	{
		list[(i+18)*3] = MSB(NTADR_A(i+13,15));
		list[(i+18)*3+1] = LSB(NTADR_A(i+13,15));
		list[(i+18)*3+2] = 0x07;
	}
		for(i=0;i<6;i++)
	{
		list[i*3] = MSB(NTADR_A(i+13,12));
		list[i*3+1] = LSB(NTADR_A(i+13,12));
		list[i*3+2] = 0x08;
	}
	
	list[6*4*3] = NT_UPD_EOF;
	
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
	
	list[(pos_lapin+18)*3+2]=0x01;
	
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
			if(tir_y < 3) tir_y--;
			if(tir_oeuf_y >0) tir_oeuf_y++;
			for(i=0;i<2;i++)
			{
				if(oeuf[i*3]==0&&oeuf[i*3+2]==1)oeuf[i*3+2]=2;
				if(oeuf[i*3]==5&&oeuf[i*3+2]==2)oeuf[i*3+2]=1;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==1)oeuf[i*3]--;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==2)oeuf[i*3]++;
				if(6==oeuf[i*3]&&(rand8()%4)==1)oeuf[i*3]--;
				//if(6==oeuf[i*3])oeuf[i*3]--;
				if(tir_oeuf_y==0&&oeuf[i*3]!=6&&(rand8()%4)==1)
				{
					tir_oeuf_y++;
					tir_oeuf_x= oeuf[i*3];
				}
			}
			wait =0;
		}
		if(tir_y == 0)
		{
			list[(tir_x+6)*3+2]=0x0B;
			for(i=0;i<2;i++)
			{	
				if(oeuf[i*3]==tir_x)
				{
					oeuf[i*3+2]=3;
					score++;
					if(wait_max>10)wait_max-=2;
				}
			}			
			tir_y =3;
			tir_y_old =tir_y;
		}
		if(tir_oeuf_y == 3)
		{
			list[(tir_oeuf_x+12)*3+2]=0x0B;
			if(tir_oeuf_x==pos_lapin)
			{
				hit_lapin=1;
				wait_max =60;
			}
			tir_oeuf_y=0;
		}
		
		if(pos_lapin!=pos_lapin_old)
		{
			list[(pos_lapin+18)*3+2]=0x01;
			list[(pos_lapin_old+18)*3+2]=0x07;
			pos_lapin_old=pos_lapin;
		}
		if(tir_y!=tir_y_old)
		{
			list[(tir_x+(tir_y*6))*3+2]=0x05;
			if(tir_y <2)list[(tir_x+(tir_y_old*6))*3+2]=0x0B;
			tir_y_old=tir_y;
		}
		if(tir_oeuf_y!=tir_oeuf_y_old)
		{
			if(tir_oeuf_y>0)list[(tir_oeuf_x+(tir_oeuf_y*6))*3+2]=0x06;
			if(tir_oeuf_y_old>0)list[(tir_oeuf_x+(tir_oeuf_y_old*6))*3+2]=0x0B;
			tir_oeuf_y_old=tir_oeuf_y;
		}
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3]!=oeuf[i*3+1])
			{
				list[oeuf[i*3]*3+2]=0x02;
				if(oeuf[i*3+1]!=6)list[oeuf[i*3+1]*3+2]=0x08;
				oeuf[i*3+1]=oeuf[i*3];
			}
		}
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3+2]==3&&(wait%2)==0)
			{
				list[oeuf[i*3]*3+2]=0x02;
				oeuf[i*3+2]=4;
			}
			else if(oeuf[i*3+2]==4&&(wait%2)==0)
			{
				list[oeuf[i*3]*3+2]=0x04;
				oeuf[i*3+2]=3;
			}
			if(oeuf[i*3+2]>2&&wait==wait_max)
			{
				list[oeuf[i*3]*3+2]=0x08;
				oeuf[i*3]=6;
				oeuf[i*3+1]=6;
				oeuf[i*3+2]=1;
			}
		}
		if(hit_lapin==1&&(wait%2)==0)
		{
			list[(pos_lapin+18)*3+2]=0x03;
			hit_lapin=2;
		}
		else if(hit_lapin==2&&(wait%2)==0)
		{
			list[(pos_lapin+18)*3+2]=0x01;
			hit_lapin=1;
		}
		if(hit_lapin>0&&wait==wait_max)
		{
			list[(pos_lapin+18)*3+2]=0x07;
			list[18*3+2]=0x01;
			pos_lapin=0;
			pos_lapin_old=0;
			hit_lapin = 0;
		}
	}

}