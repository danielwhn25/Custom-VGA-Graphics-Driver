

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "address_map_niosv.h"



typedef uint16_t pixel_t;

volatile pixel_t *pVGA = (pixel_t *) FPGA_PIXEL_BUF_BASE;

const pixel_t blk = 0x0000;
const pixel_t wht = 0xffff;
const pixel_t red = 0xf800;
const pixel_t grn = 0x07e0;
const pixel_t blu = 0x001f;

void delay( int N )
{
	for( int i=0; i<N; i++ ) 
		*pVGA; // read volatile memory location to waste time
}

/* STARTER CODE BELOW. FEEL FREE TO DELETE IT AND START OVER */



void drawPixel( int y, int x, pixel_t colour )
{
	*(pVGA + (y<<YSHIFT) + x ) = colour;
}

pixel_t readPixel(int y, int x)
{
    return *(pVGA + (y<<YSHIFT) + x);
}

pixel_t makePixel( uint8_t r8, uint8_t g8, uint8_t b8 )
{
	// inputs: 8b of each: red, green, blue
	const uint16_t r5 = (r8 & 0xf8)>>3; // keep 5b red
	const uint16_t g6 = (g8 & 0xfc)>>2; // keep 6b green
	const uint16_t b5 = (b8 & 0xf8)>>3; // keep 5b blue
	return (pixel_t)( (r5<<11) | (g6<<5) | b5 );
}

void rect( int y1, int y2, int x1, int x2, pixel_t c )
{
	for( int y=y1; y<y2; y++ )
		for( int x=x1; x<x2; x++ )
			drawPixel( y, x, c );
}

void tri( int y1, int y2, int x1, int x2, pixel_t c)
{
	for (int y = y1; y < y2; y++)
	{
		for (int x = x1; x < y; x++)
		{
			drawPixel(y, x, c);
		}
	}
}

// generates random starting places for each triangle 
int randx1() { return (rand() % (MAX_X -8)) + 1; } 
int randy1() { return (rand() % (MAX_Y -8)) + 1; }

void rand_obstacle(int n)
{
	if (n == 1)
	{
		// let's generate a random number from 5 to 15 of obstacles to make
		int randnum = (rand() % (15 - 5 + 1)) + 1;

		// I want the size of my obstacles to be triangles 
		// that have a delta y  = delta x = 7


		// generate a random number of obstacles each time
		for (int j = 0; j < randnum; j++)
		{
			int x1 = randx1();	
			int y1 = randy1();
			int x2 = x1 + 7;
			int y2 = y1 + 7;
			tri(y1, y2, x1, x2, wht);
		}
	}
}

void norm_obstacle(int n)
{
	if (n == 1)
	{
		int x1 = MAX_X/2-50;
		int y1 = MAX_Y/2-50;
		// I want the size of my obstacles to be triangles 
		// that have a delta y  = delta x = 7
		// let's make 7 obstacles
		int i=0;
		int j=0;
		
		for (int m = 0; m < 6; m++)
		{
			int x2 = x1 + 7;
			int y2 = y1 + 7;
			
			rect(y1 + i, y2 +i, x1 +j, x2+j, wht);
			i = 10 + i; 
			j = 18 + j;
		
		}
		
		int x12 = 0;
		int y12 = MAX_Y/2;
		int a = 0;
		int b = 5;
		for (int n = 0; n < 6; n++)
		{
			int x2 = x12 + 7;
			int y2 = y12 + 7;
			
			rect(y12 + a, y2 +a, x12 +b, x2+b, wht);
			a = a-10; 
			b = b+10;
		}
		
		//rect(MAX_Y/2-60, MAX_Y/2, MAX_X-10, MAX_X, wht); 
		// will be compensated appropriately on the de10-lite
	}
} 
void GUI(int n)
{
	if (n>=1)
	{
		rect( 0, MAX_Y, 0, MAX_X, blk ); 		// black box
		for (int x = 0; x < MAX_X; x++) {    	// Top horizontal line
			drawPixel(0, x, wht);
		}
		for (int x = 0; x < MAX_X; x++) {    	// Bottom horizontal
			drawPixel(MAX_Y-3, x, wht);
		}
		for (int y = 0; y < MAX_Y-3; y++) {    // Left vertical
			drawPixel(y, 0, wht);
		}
		for (int y = 0; y < MAX_Y-3; y++) {    // Right vertical
			drawPixel(y, MAX_X-1, wht);
		}
	}
}

typedef struct player_t{
	int score, x, y, dx, dy, life;
	pixel_t colour;
} player_t;
	player_t p1 = {0, MAX_X/2-3, MAX_Y/2, -1, 0, 1, blu}; 
	player_t p2 = {0, MAX_X/2+3, MAX_Y/2,  1, 0, 1, red}; 

void robotdir(player_t *p2)
{
    if (p2->dx == 1)
    {
        int front = readPixel(p2->y, p2->x + 1);     
        int up    = readPixel(p2->y - 1, p2->x);   
        int down  = readPixel(p2->y + 1, p2->x);     

        if (front == blk) return;     

        if (up == blk) {          
            p2->dx = 0;  p2->dy = -1;
            return;
        }

        if (down == blk) {           
            p2->dx = 0;  p2->dy = 1;
            return;
        }

        return; 
    }

    if (p2->dx == -1)
    {
        int front = readPixel(p2->y, p2->x - 1);
        int down  = readPixel(p2->y + 1, p2->x); 
        int up    = readPixel(p2->y - 1, p2->x); 

        if (front == blk) return;

        if (down == blk) {            
            p2->dx = 0;  
			p2->dy = 1;
            return;
        }

        if (up == blk) {             
            p2->dx = 0;  
			p2->dy = -1;
            return;
        }

        return;
    }

    if (p2->dy == 1)
    {
        int front = readPixel(p2->y + 1, p2->x);
        int right = readPixel(p2->y, p2->x + 1);
        int left  = readPixel(p2->y, p2->x - 1);

        if (front == blk) return;

        if (right == blk) {          
            p2->dx = 1;  
			p2->dy = 0;
            return;
        }

        if (left == blk) {            
            p2->dx = -1; 
			p2->dy = 0;
            return;
        }

        return;
    }

    
    if (p2->dy == -1)
    {
        int front = readPixel(p2->y - 1, p2->x);
        int left  = readPixel(p2->y, p2->x - 1);
        int right = readPixel(p2->y, p2->x + 1);

        if (front == blk) return;

        if (left == blk) {           
            p2->dx = -1; 
			p2->dy = 0;
            return;
        }

        if (right == blk) {        
            p2->dx = 1;  
			p2->dy = 0;
            return;
        }

        return;
    }
}
void writehex(int x, int y) // active high btw AND LSB = top segment -> MSB = segment g. E.g. 0b0111111 has all segments on except for segment g
{
	uint16_t lut[10] = 
	{	
		0b0111111, // 0
		0b0000110, // 1
		0b1011011, // 2
		0b1001111, // 3
		0b1100110, // 4
		0b1101101, // 5
		0b1111101, // 6
		0b0000111, // 7
		0b1111111, // 8
		0b1101111  
	};
	*(uint32_t*) (HEX3_HEX0_BASE+16) = lut[x];	 // HEX4 = p1.score
	*(uint32_t*) (HEX3_HEX0_BASE)    = lut[y]; // HEX0 = p2.score
}


/*****************************
 ***********MAIN**************
 *****************************/

int main()
{	

	//int* HEX0 = (int*) HEX3_HEX0_BASE;


	srand(time(NULL)); // if you want to generate random obstacles
	volatile int* key_press = (int*) KEY_BASE;


	




	while (p1.score < 9 && p2.score <9)
	{
		printf("\nYour score: %d\n", p1.score);
		printf("\nRobot's score: %d\n______________________\n", p2.score);

		writehex(p1.score, p2.score);
	

		GUI(1);
		delay (2000000); // delay in between games after generating the GUI;

		p1.x = MAX_X/2 - 3; 
		p1.y = MAX_Y/2-40;
		p1.dx = -1;  
		p1.dy = 0;
		p1.life = 1;
		
		p2.x = MAX_X/2 + 3;
		p2.y = MAX_Y/2;
		p2.dx = 1;  
		p2.dy = 0;
		p2.life = 1;

		rand_obstacle(1);
		norm_obstacle(1);

		while (p1.life && p2.life)
		{
			p1.y += p1.dy;
			p1.x += p1.dx;

			if (p1.x < 0 || p1.x >= MAX_X || p1.y < 0 || p1.y >= MAX_Y)
				{p1.life = 0; p2.score++; break;}
			if (readPixel(p1.y, p1.x) != blk )
				{p1.life = 0; p2.score++; break;}
			
			drawPixel(p1.y, p1.x, p1.colour); 

			robotdir(&p2);
			p2.x += p2.dx;
			p2.y += p2.dy;

			if (p2.x < 0 || p2.x >= MAX_X || p2.y < 0 || p2.y >= MAX_Y)
				{p2.life = 0; p1.score++; break;}
			if (readPixel(p2.y, p2.x) != blk)
				{p2.life = 0; p1.score++; break;}
			
			drawPixel(p2.y, p2.x, p2.colour);

		
			// player 1 control logic
			if (p1.dx == 1 && p1.dy == 0)
			{
				if (*key_press & 0x2) // KEY[1] will allow us to turn "left" or "up"
				{p1.dx = 0;
					p1.dy = -1; 
					*key_press = 0x2;  
					delay(1000000/2.75);
				}
				else if (*key_press & 0x1)
				{
					p1.dx = 0;
					p1.dy = 1;  
					*key_press = 0x1;  
					delay(1000000/2.75);
				}
			}
			else if (p1.dx == -1 && p1.dy == 0)
			{
				if (*key_press & 0x2)
				{
					p1.dx = 0;
					p1.dy = 1;  
					*key_press = 0x2;
					delay(1000000/2.75);
				}
				else if (*key_press & 0x1)
				{
					p1.dx = 0;
					p1.dy = -1;  
					*key_press = 0x1;
					delay(1000000/2.75);
				}
			}
			else if (p1.dx == 0 && p1.dy == 1)  
			{
				if (*key_press & 0x2)
				{
					p1.dx = 1;  
					p1.dy = 0;
					*key_press = 0x2;
					delay(1000000/2.75);
				}
				else if (*key_press & 0x1)
				{
					p1.dx = -1;  
					p1.dy = 0;
					*key_press = 0x1;
					delay(1000000/2.75);
				}
			}
			else if (p1.dx == 0 && p1.dy == -1)  
			{
				if (*key_press & 0x2)
				{
					p1.dx = -1; 
					p1.dy = 0;
					*key_press = 0x2;
					delay(1000000/2.75);
				}
				else if (*key_press & 0x1)
				{
					p1.dx = 1;  
					p1.dy = 0;
					*key_press = 0x1;
					delay(1000000/2.75);
				}
			}
			delay(450000);  // Slows down the game so you can see movement
		}
	}
	
	rect (MAX_Y/4, MAX_Y/2, MAX_X/3-100, MAX_X/2-100, wht);
	
	if (p1.score == 9)
	{
		printf("\nP1 WINS! Final score: %d - %d\n", p1.score, p2.score);
		rect(0, MAX_Y, 0, MAX_X, grn);
	}

	else
	{
		printf("\n You lost. Robot wins! Final score: %d - %d\n", p2.score, p1.score);
		rect(0, MAX_Y, 0, MAX_X, red);
	}
	writehex(p1.score, p2.score);

	
	return 0;
}
