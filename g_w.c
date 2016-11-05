//this example code shows how to put some text in nametable
//it assumes that you have ASCII-encoded font in the CHR tiles $00-$3f
//it also shows how to detect PAL/NTSCvideo system

#include "neslib.h"
#include "g_w.h"
#include "rand.h"


static unsigned int i, calc;
static unsigned int pos_lapin,pos_lapin_old, wait, wait_max, tir_x, tir_y,tir_y_old, score, compteur;
static unsigned int tir_oeuf_x, tir_oeuf_y,tir_oeuf_y_old,hit_lapin;
static unsigned char pad, move;

//put a string into the nametable

static unsigned char list[5*4*3*2+9+1];
// structure de la liste :
// lapin : 4 tules x (x,y,id_tule)
// oeuf 1 : 4 tules x (x,y,id_tule)
// oeuf 2 : 4 tules x (x,y,id_tule)

// on verra plus tard
// tir : 4 tules x (x,y,id_tule)f

const unsigned char palBackground[16]={ 0x0f,0x38,0x17,0x28,0x0f,0x18,0x28,0x38,0x0f,0x28,0x11,0x38,0x0f,0x1c,0x2c,0x3c };



static unsigned int oeuf[6]={6,6,1,6,6,1};


//init data for the update list, it contains MSB and LSB of a tile address
//in the nametable, then the tile number

void place_lapin(unsigned int pos_lapin)
{
	list[12]= MSB(NTADR_A(10+(pos_lapin*2),16));
	list[13]= LSB(NTADR_A(10+(pos_lapin*2),16));
	list[15]= MSB(NTADR_A(11+(pos_lapin*2),16));
	list[16]= LSB(NTADR_A(11+(pos_lapin*2),16));
	list[18]= MSB(NTADR_A(10+(pos_lapin*2),17));
	list[19]= LSB(NTADR_A(10+(pos_lapin*2),17));
	list[21]= MSB(NTADR_A(11+(pos_lapin*2),17));
	list[22]= LSB(NTADR_A(11+(pos_lapin*2),17));
	list[14]=0x00;
	list[17]=0x01;
	list[20]=0x02;
	list[23]=0x03;
} 

void efface_lapin(unsigned int pos_lapin)
{
	list[0]= MSB(NTADR_A(10+(pos_lapin*2),16));
	list[1]= LSB(NTADR_A(10+(pos_lapin*2),16));
	list[3]= MSB(NTADR_A(11+(pos_lapin*2),16));
	list[4]= LSB(NTADR_A(11+(pos_lapin*2),16));
	list[6]= MSB(NTADR_A(10+(pos_lapin*2),17));
	list[7]= LSB(NTADR_A(10+(pos_lapin*2),17));
	list[9]= MSB(NTADR_A(11+(pos_lapin*2),17));
	list[10]= LSB(NTADR_A(11+(pos_lapin*2),17));
	list[2]=0x20;
	list[5]=0x21;
	list[8]=0x22;
	list[11]=0x23;
}

void mort_lapin(unsigned int pos_lapin)
{
	list[12]= MSB(NTADR_A(10+(pos_lapin*2),16));
	list[13]= LSB(NTADR_A(10+(pos_lapin*2),16));
	list[15]= MSB(NTADR_A(11+(pos_lapin*2),16));
	list[16]= LSB(NTADR_A(11+(pos_lapin*2),16));
	list[18]= MSB(NTADR_A(10+(pos_lapin*2),17));
	list[19]= LSB(NTADR_A(10+(pos_lapin*2),17));
	list[21]= MSB(NTADR_A(11+(pos_lapin*2),17));
	list[22]= LSB(NTADR_A(11+(pos_lapin*2),17));
	list[14]=0x04;
	list[17]=0x05;
	list[20]=0x06;
	list[23]=0x07;
} 

void efface_oeuf(unsigned int pos,unsigned int num)
{
	list[24+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[25+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[27+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[28+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[30+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[31+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[33+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[34+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[26+(12*num)]=0x24;
	list[29+(12*num)]=0x25;
	list[32+(12*num)]=0x26;
	list[35+(12*num)]=0x27;
	
} 

void oeuf_gueule_1(unsigned int pos,unsigned int num)
{
	list[48+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[49+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[51+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[52+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[54+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[55+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[57+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[58+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[50+(12*num)]=0x10;
	list[53+(12*num)]=0x11;
	list[56+(12*num)]=0x12;
	list[59+(12*num)]=0x13;
} 

void oeuf_gueule_2(unsigned int pos,unsigned int num)
{
	list[48+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[49+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[51+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[52+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[54+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[55+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[57+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[58+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[50+(12*num)]=0x14;
	list[53+(12*num)]=0x15;
	list[56+(12*num)]=0x16;
	list[59+(12*num)]=0x17;
} 

/*
void oeuf_gueule_1(unsigned int pos,unsigned int num)
{
	list[24+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[25+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[27+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[28+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[30+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[31+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[33+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[34+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[26+(12*num)]=0x10;
	list[29+(12*num)]=0x11;
	list[32+(12*num)]=0x12;
	list[35+(12*num)]=0x13;
} 

void oeuf_gueule_2(unsigned int pos,unsigned int num)
{
	list[24+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[25+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[27+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[28+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[30+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[31+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[33+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[34+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[26+(12*num)]=0x14;
	list[29+(12*num)]=0x15;
	list[32+(12*num)]=0x16;
	list[35+(12*num)]=0x17;
} 
*/

void place_oeuf(unsigned int pos,unsigned int num)
{
	list[48+(12*num)]= MSB(NTADR_A(10+(pos*2),10));
	list[49+(12*num)]= LSB(NTADR_A(10+(pos*2),10));
	list[51+(12*num)]= MSB(NTADR_A(11+(pos*2),10));
	list[52+(12*num)]= LSB(NTADR_A(11+(pos*2),10));
	list[54+(12*num)]= MSB(NTADR_A(10+(pos*2),11));
	list[55+(12*num)]= LSB(NTADR_A(10+(pos*2),11));
	list[57+(12*num)]= MSB(NTADR_A(11+(pos*2),11));
	list[58+(12*num)]= LSB(NTADR_A(11+(pos*2),11));
	list[50+(12*num)]=0x0c;
	list[53+(12*num)]=0x0d;
	list[56+(12*num)]=0x0e;
	list[59+(12*num)]=0x0f;
} 

void efface_tir(unsigned int pos_x,unsigned int pos_y)
{
	list[72]= MSB(NTADR_A(pos_x,pos_y));
	list[73]= LSB(NTADR_A(pos_x,pos_y));
	list[75]= MSB(NTADR_A(pos_x+1,pos_y));
	list[76]= LSB(NTADR_A(pos_x+1,pos_y));
	list[78]= MSB(NTADR_A(pos_x,pos_y+1));
	list[79]= LSB(NTADR_A(pos_x,pos_y+1));
	list[81]= MSB(NTADR_A(pos_x+1,pos_y+1));
	list[82]= LSB(NTADR_A(pos_x+1,pos_y+1));
	list[74]=0x28;
	list[77]=0x29;
	list[80]=0x2a;
	list[83]=0x2b;
} 

void efface_tir_o(unsigned int pos_x,unsigned int pos_y)
{
	list[84]= MSB(NTADR_A(pos_x,pos_y));
	list[85]= LSB(NTADR_A(pos_x,pos_y));
	list[87]= MSB(NTADR_A(pos_x+1,pos_y));
	list[88]= LSB(NTADR_A(pos_x+1,pos_y));
	list[90]= MSB(NTADR_A(pos_x,pos_y+1));
	list[91]= LSB(NTADR_A(pos_x,pos_y+1));
	list[93]= MSB(NTADR_A(pos_x+1,pos_y+1));
	list[94]= LSB(NTADR_A(pos_x+1,pos_y+1));
	list[86]=0x28;
	list[89]=0x29;
	list[92]=0x2a;
	list[95]=0x2b;
}

void place_tir_o(unsigned int pos_x,unsigned int pos_y)
{
	list[96]= MSB(NTADR_A(pos_x,pos_y));
	list[97]= LSB(NTADR_A(pos_x,pos_y));
	list[99]= MSB(NTADR_A(pos_x+1,pos_y));
	list[100]= LSB(NTADR_A(pos_x+1,pos_y));
	list[102]= MSB(NTADR_A(pos_x,pos_y+1));
	list[103]= LSB(NTADR_A(pos_x,pos_y+1));
	list[105]= MSB(NTADR_A(pos_x+1,pos_y+1));
	list[106]= LSB(NTADR_A(pos_x+1,pos_y+1));
	list[98]=0x1c;
	list[101]=0x1d;
	list[104]=0x1e;
	list[107]=0x1f;
} 

void place_tir(unsigned int pos_x,unsigned int pos_y)
{
	list[108]= MSB(NTADR_A(pos_x,pos_y));
	list[109]= LSB(NTADR_A(pos_x,pos_y));
	list[111]= MSB(NTADR_A(pos_x+1,pos_y));
	list[112]= LSB(NTADR_A(pos_x+1,pos_y));
	list[114]= MSB(NTADR_A(pos_x,pos_y+1));
	list[115]= LSB(NTADR_A(pos_x,pos_y+1));
	list[117]= MSB(NTADR_A(pos_x+1,pos_y+1));
	list[118]= LSB(NTADR_A(pos_x+1,pos_y+1));
	list[110]=0x18;
	list[113]=0x19;
	list[116]=0x1a;
	list[119]=0x1b;
}

void put_score(const int sco)
{
	list[120]= MSB(NTADR_A(21,9));
	list[121]= LSB(NTADR_A(21,9));
	list[122]= 0x30+(sco%10);
	list[123]= MSB(NTADR_A(20,9));
	list[124]= LSB(NTADR_A(20,9));
	list[125]= 0x30+(sco/10)%10;
	list[126]= MSB(NTADR_A(19,9));
	list[127]= LSB(NTADR_A(19,9));
	list[128]= 0x30+(sco/100);
}

void kill(unsigned int num)
{
	for(i=0;i<4;i++)
	{
		list[num+i*3] = MSB(NTADR_A(0,0));
		list[num+i*3+1] = LSB(NTADR_A(0,0));
		list[num+i*3+2] = 0x27;
	}
}

void main(void)
{
	//rendering is disabled at the startup, the palette is all black

	pal_bg(palBackground);

	for(i=0;i<(10*12+3);i++)
	{
		list[i*3] = MSB(NTADR_A(0,0));
		list[i*3+1] = LSB(NTADR_A(0,0));
		list[i*3+2] = 0x27;
	}
	
	list[5*4*3*2+9] = NT_UPD_EOF;
	
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
	compteur = 0;
	
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
		compteur++;

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
			//wait =0;
		}
		if(pad==0)move =0;
		
		
		 if(wait>wait_max)
		{
			//set_rand(compteur);
			if(tir_y < 3) tir_y--;
			if(tir_oeuf_y >0) tir_oeuf_y++;
			for(i=0;i<2;i++)
			{
				// déplacements oeufs
				if(oeuf[i*3]==0&&oeuf[i*3+2]==1)oeuf[i*3+2]=2;
				if(oeuf[i*3]==5&&oeuf[i*3+2]==2)oeuf[i*3+2]=1;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==1)oeuf[i*3]--;
				if(oeuf[i*3]<6&&oeuf[i*3+2]==2)oeuf[i*3]++;
				if(6==oeuf[i*3]&& (rand()%5)==1)oeuf[i*3]--;
				if(tir_oeuf_y==0&&oeuf[i*3]!=6&&(rand()%5)==1)
				{
					tir_oeuf_y++;
					tir_oeuf_x= oeuf[i*3];
				}
			}
			put_score(score);
			wait =0;
		}
		if(tir_y == 0)
		{
			// list[(tir_x+6)*3+2]=0x0B;
			efface_tir(10+(tir_x*2),10+(tir_y_old*2));
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
			kill(108); // supprimer l'affichage du tir
			tir_y_old =tir_y;
		}
		if(tir_oeuf_y == 3)
		{
			// list[(tir_oeuf_x+12)*3+2]=0x0B;
			efface_tir_o(12+(tir_oeuf_x*2),10);
			if(tir_oeuf_x==pos_lapin)
			{
				hit_lapin=1;
				wait_max =60;
			}
			kill(96); // supprimer l'affichage du tir
			tir_oeuf_y=0;
		}
		
		// déplacements lapin
		if(pos_lapin!=pos_lapin_old)
		{
			efface_lapin(pos_lapin_old);
			place_lapin(pos_lapin);
			pos_lapin_old=pos_lapin;
		}
		if(tir_y!=tir_y_old)
		{
			if(tir_y_old <3) efface_tir(10+(tir_x*2),10+(tir_y_old*2));
			place_tir(10+(tir_x*2),10+(tir_y*2));
			//list[(tir_x+(tir_y*6))*3+2]=0x05;
			//list[(tir_x+(tir_y_old*6))*3+2]=0x0B;
			tir_y_old=tir_y;
		}
		if(tir_oeuf_y!=tir_oeuf_y_old)
		{
			if(tir_oeuf_y>0)
				// list[(tir_oeuf_x+(tir_oeuf_y*6))*3+2]=0x06;
				place_tir_o(10+(tir_oeuf_x*2),10+(tir_oeuf_y*2));
			if(tir_oeuf_y_old>0) 
				//list[(tir_oeuf_x+(tir_oeuf_y_old*6))*3+2]=0x0B;
				efface_tir_o(10+(tir_oeuf_x*2),10+(tir_oeuf_y_old*2));
			tir_oeuf_y_old=tir_oeuf_y;
		}
		// déplacements oeufs
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3]!=oeuf[i*3+1])
			{
				if(oeuf[i*3+1]!=6) efface_oeuf(oeuf[i*3+1],i);
			}
		}
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3]!=oeuf[i*3+1])
			{
				place_oeuf(oeuf[i*3],i);
				oeuf[i*3+1]=oeuf[i*3];
			}
		}
		for(i=0;i<2;i++)
		{
			if(oeuf[i*3+2]==3&&(wait%2)==0)
			{
				// list[oeuf[i*3]*3+2]=0x02;
				oeuf_gueule_1(oeuf[i*3+1],i);
				oeuf[i*3+2]=4;
			}
			else if(oeuf[i*3+2]==4&&(wait%2)==0)
			{
				// list[oeuf[i*3]*3+2]=0x04;
				oeuf_gueule_2(oeuf[i*3+1],i);
				oeuf[i*3+2]=3;
			}
			if(oeuf[i*3+2]>2&&wait==wait_max)
			{
				// list[oeuf[i*3]*3+2]=0x08;
				efface_oeuf(oeuf[i*3+1],i);
				oeuf[i*3]=6;
				oeuf[i*3+1]=6;
				oeuf[i*3+2]=1;
				calc = 48 + 12 * i;
				kill(calc); //supprimer l'affichage de l'oeuf i
			}
		}
		if(hit_lapin==1&&(wait%2)==0)
		{
			// list[(pos_lapin+18)*3+2]=0x03;
			mort_lapin(pos_lapin);
			hit_lapin=2;
		}
		else if(hit_lapin==2&&(wait%2)==0)
		{
			// list[(pos_lapin+18)*3+2]=0x01;
			place_lapin(pos_lapin);
			hit_lapin=1;
		}
		if(hit_lapin>0&&wait==wait_max)
		{
			efface_lapin(pos_lapin);
			pos_lapin=0;
			place_lapin(pos_lapin);
			pos_lapin_old=0;
			hit_lapin = 0;
		}
	}

}