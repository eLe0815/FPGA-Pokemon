/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG#
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "ps2_core.h"
#include "spi_core.h"
#include <cstring>
#include <cmath>


void test_start(GpoCore *led_p) {
   int i;

   for (i = 0; i < 20; i++) {
      led_p->write(0xff00);
      sleep_ms(50);
      led_p->write(0x0000);
      sleep_ms(50);
   }
}

/**
 * check bar generator core
 * @param bar_p pointer to Gpv instance
 */
void bar_check(GpvCore *bar_p) {
   bar_p->bypass(0);
   sleep_ms(3000);
}

/**
 * check color-to-grayscale core
 * @param gray_p pointer to Gpv instance
 */
void gray_check(GpvCore *gray_p) {
   gray_p->bypass(0);
   sleep_ms(3000);
   gray_p->bypass(1);
}

/**
 * check osd core
 * @param osd_p pointer to osd instance
 */
void osd_check(OsdCore *osd_p) {
   osd_p->set_color(0x0f0, 0x001); // dark gray/green
   osd_p->bypass(0);
   osd_p->clr_screen();
   for (int i = 0; i < 64; i++) {
      osd_p->wr_char(8 + i, 20, i);
      osd_p->wr_char(8 + i, 21, 64 + i, 1);
      sleep_ms(100);
   }
   sleep_ms(3000);
}

/**
 * test frame buffer core
 * @param frame_p pointer to frame buffer instance
 */
void frame_check(FrameCore *frame_p) {
   int x, y, color;

   frame_p->bypass(0);
   for (int i = 0; i < 10; i++) {
      frame_p->clr_screen(0x008);  // dark green
      for (int j = 0; j < 20; j++) {
         x = rand() % 640;
         y = rand() % 480;
         color = rand() % 512;
         frame_p->plot_line(400, 200, x, y, color);
      }
      sleep_ms(300);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to mouse sprite instance
 */
void mouse_check(SpriteCore *mouse_p) {
   int x, y;

   mouse_p->bypass(0);
   // clear top and bottom lines
   for (int i = 0; i < 32; i++) {
      mouse_p->wr_mem(i, 0);
      mouse_p->wr_mem(31 * 32 + i, 0);
   }

   // slowly move mouse pointer
   x = 0;
   y = 0;
   for (int i = 0; i < 80; i++) {
      mouse_p->move_xy(x, y);
      sleep_ms(50);
      x = x + 4;
      y = y + 3;
   }
   sleep_ms(3000);
   // load top and bottom rows
   for (int i = 0; i < 32; i++) {
      sleep_ms(20);
      mouse_p->wr_mem(i, 0x00f);
      mouse_p->wr_mem(31 * 32 + i, 0xf00);
   }
   sleep_ms(3000);
}

/**
 * test ghost sprite
 * @param ghost_p pointer to ghost sprite instance
 */
void ghost_check(SpriteCore *ghost_p) {
   int x, y;

   // slowly move mouse pointer
   ghost_p->bypass(0);
   ghost_p->wr_ctrl(0x1c);  //animation; blue ghost
   x = 0;
   y = 100;
   for (int i = 0; i < 156; i++) {
      ghost_p->move_xy(x, y);
      sleep_ms(100);
      x = x + 4;
      if (i == 80) {
         // change to red ghost half way
         ghost_p->wr_ctrl(0x04);
      }
   }
   sleep_ms(3000);
}

void start_screen(FrameCore *frame_p, OsdCore *osd_p){
	frame_p->bypass(0);
	frame_p->clr_screen(0x0000);
	osd_p->set_color(0x0ff, 0x111);
	osd_p->bypass(0);
	osd_p->clr_screen();
	char text[] = {' ',' ','F','P','G','A',' ','P','O','K','E','M','O','N',' ',' '};
	char start[] = {'A','N','Y',' ','K','E','Y',' ','T','O',' ','S','T','A','R','T'};
	for(int i = 0; i < 16; i++){
		osd_p->wr_char(i + 34, 10, text[i]);
		osd_p->wr_char(i + 34, 15, start[i]);
	}
}

void start_key(Ps2Core *ps2_p){
	char ch;
	while(!ps2_p->get_kb_ch(&ch)){

	}
	return;
}

void game_over(FrameCore *frame_p, OsdCore *osd_p){
	frame_p->clr_screen(0x00);
	osd_p->set_color(0xf00, 0x000);
	osd_p->clr_screen();
	char gameOver[] = {'G','A','M','E',' ','O','V','E','R'};
	for(int i = 0; i < 9; i++){
		osd_p->wr_char(i + 38, 10, gameOver[i]);
	}
	char gameAgain[] = {'P','L','A','Y',' ','A','G','A','I','N','?'};
	for(int i = 0; i < 11; i++){
		osd_p->wr_char(i + 37, 15, gameAgain[i]);
	}
}


// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore mewtwo(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore snorlax(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore cursor(get_sprite_addr(BRIDGE_BASE, V4_USER4), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
SpiCore spi(get_slot_addr(BRIDGE_BASE, S9_SPI));

void environmentInit(FrameCore *frame_p) {
    //background
    frame.clr_screen(0xfff);

    //player platform
    frame_p->fillRoundRect(-50, 300, 400, 150, 600, 0x092);
    frame_p->fillRoundRect(-40, 305, 380, 140, 200, 0x0db);
    frame_p->fillRoundRect(-30, 315, 360, 120, 200, 0x16d);

    //cpu platform
    frame_p->fillRoundRect(330, 130, 300, 90, 600, 0x092); //416, 47 center
    frame_p->fillRoundRect(340, 135, 280, 80, 200, 0x0db);
    frame_p->fillRoundRect(350, 140, 260, 70, 200, 0x16d);

    //bottom text
    frame_p->fillRect(0, 380, 640, 100, 0x000);
    frame_p->fillRoundRect(10, 385, 620, 95, 20, 0xfff);

    //player status
    frame_p->fillRoundRect(400, 240, 240, 140, 20, 0x000);
    frame_p->fillRoundRect(405, 245, 230, 130, 20, 0xfff);

    //cpu status
    frame_p->fillRoundRect(3, 40, 240, 80, 20, 0x000);
    frame_p->fillRoundRect(8, 45, 230, 70, 20, 0xfff);
}

class Move{
public:
	char Name[20];
	int damage;
	bool nonAtk = false;
	//constructor
	Move(const char* name, int damage, bool nonAtk) : damage(damage), nonAtk(nonAtk) {
		strncpy(Name, name, sizeof(Name) - 1);
		Name[sizeof(Name) - 1] = '\0';
	}
};

class Pokemon{
public:
    char Name[8];
    int speed, def, attack, level, health;
    int maxHP;
    bool isFainted = false;
    Move moves[4];

    // Constructor
    Pokemon(const char* name, int spd, int def, int att, int lvl, int h, const Move& move1, const Move& move2, const Move& move3, const Move& move4):
    	speed(spd), def(def), attack(att), level(lvl), health(h), moves{move1,move2,move3,move4} {
//    	moves[0] = move1;
//    	moves[1] = move2;
//    	moves[2] = move3;
//    	moves[3] = move4;
		strncpy(Name, name, sizeof(Name) - 1);
		Name[sizeof(Name) - 1] = '\0';
		maxHP = h;
    }

    void makeMove(Pokemon& attackingPokemon, const Move& move, Pokemon& attackedPokemon){
    	if(move.nonAtk == false){
    		attackedPokemon.health = attackedPokemon.health - move.damage;
    		if(health < 0) health = 0;
    	}
    	else if(strcmp(move.Name, "Recover") == 0){
    		int recoveredHealth = floor(attackingPokemon.health/2);
    		attackingPokemon.health = attackingPokemon.health + recoveredHealth;
    		if(attackingPokemon.health > attackingPokemon.maxHP)
    			attackingPokemon.health = attackingPokemon.maxHP;
    	}
    	else if(strcmp(move.Name, "Rest") == 0){
    		attackingPokemon.health = attackingPokemon.maxHP;
    	}
    	else if(strcmp(move.Name, "Amnesia")){
    		attackingPokemon.def = attackingPokemon.def * 2;
    	}
    }

};

	bool isOneToFour(char c){
		if(c >= 0x31 && c <= 0x34)
			return true;
		else
			return false;
	}

	char asciiToDigit(char c){
		c = c - 0x30;
		return c;
	}

	void fainted(OsdCore *osd_p, Pokemon& pokemon){
		char fainted[] = {'h','a','s',' ','f','a','i','n','t','e','d'};
		for(int i = 0; i < 8; i++){
		osd_p->wr_char(i + 5, 24, pokemon.Name[i]);
		osd_p->wr_char(i + 5, 24, fainted[i]);
		}
	}

	void hpBar(OsdCore * osd, Pokemon &poke1, Pokemon &poke2){
	    int healthHundreths = poke1.health / 100;
	    int healthTens = (poke1.health % 100) / 10;
	    int healthOnes = poke1.health % 10;

	    char HPValue1[] = {healthHundreths+48, healthTens+48, healthOnes+48, '/', '5', '2', '3'};
	    for(int i = 0; i < 7; i++){
	        osd->wr_char(i + 54, 19, HPValue1[i]);
	    }

	    healthHundreths = poke2.health / 100;
	    healthTens = (poke2.health % 100) / 10;
	    healthOnes = poke2.health % 10;

	    char HPValue2[] = {healthHundreths+48, healthTens+48, healthOnes+48, '/', '4', '1', '5'};
	        for(int i = 0; i < 7; i++){
	            osd->wr_char(i + 5, 5, HPValue2[i]);
	        }
	}

	void show_status(FrameCore *frame, OsdCore *osd, Pokemon *Snorlax, Pokemon *Mewtwo){

	    char playerName[] = {'S', 'N', 'O', 'R', 'L', 'A', 'X'};
	    for(int i = 0; i < 7; i++) {osd->wr_char(i + 51, 16, playerName[i]);}
	    char gender[] = {11};
	    osd->wr_char(58, 16, gender[0]);

	    char cpuName[] = {'M', 'E', 'W', 'T', 'W', 'O'};
	    for(int i = 0; i < 6; i++) {osd->wr_char(i + 2, 3, cpuName[i]);}

	    char lvl[] = {'L', 'v', ':', '1', '0', '0'};
	    for(int i = 0; i < 6; i++) {osd->wr_char(i + 73, 16, lvl[i]);}
	    for(int i = 0; i < 6; i++) {osd->wr_char(i + 21, 3, lvl[i]);}

	    char HP[] = {'H', 'P',};
	    for(int i = 0; i < 2; i++) {osd->wr_char(i + 51, 19, HP[i]);}
	    for(int i = 0; i < 2; i++) {osd->wr_char(i + 2, 5, HP[i]);}

	    hpBar(osd, *Snorlax, *Mewtwo);


	}

	void MewtwoAtk(Pokemon& snorlax, Pokemon& mewtwo, OsdCore *osd_p, SpriteCore *mewtwo_p){
		osd_p->clr_screen();
		show_status(&frame, &osd, &snorlax, &mewtwo);
		int random;
		random = rand() % 4 + 1;
		switch(random){
			case 1: {//future sight
				char m2Future[] = {'M','e','w','t','w','o',' ','u','s','e','d',' ','F','u','t','u','r','e',' ','s','i','g','h','t','!'};
				for(int i = 0; i < 50; i++) {
				    mewtwo_p->move_xy(416-i, 47+i);
				    sleep_ms(5);
				}
				mewtwo_p->move_xy(416,47);
				snorlax.health = snorlax.health - mewtwo.moves[0].damage;
				//uart.disp(snorlax.health);
				for(int i = 0; i < 25; i++){
					osd_p->wr_char(i + 5, 24, m2Future[i]);
					sleep_ms(25);
				}
				if(snorlax.health < 0){
					snorlax.isFainted = true;
					fainted(&osd, snorlax);
				}
				break;
			}
			case 2: {//psychic
				char m2Psychic[] = {'M','e','w','t','w','o',' ','u','s','e','d',' ','P','s','y','c','h','i','c','!'};
				for(int i = 0; i < 50; i++) {
				    mewtwo_p->move_xy(416-i, 47+i);
				    sleep_ms(5);
				}
				mewtwo_p->move_xy(416,47);
				snorlax.health = snorlax.health - mewtwo.moves[1].damage;
				//uart.disp(snorlax.health);
				for(int i = 0; i < 20; i++){
					osd_p->wr_char(i + 5, 24, m2Psychic[i]);
					sleep_ms(25);
				}
				if(snorlax.health < 0){
					snorlax.isFainted = true;
					fainted(&osd, snorlax);
				}
				break;
			}
			case 3: {//psystrike
				char m2Psystrike[] = {'M','e','w','t','w','o',' ','u','s','e','d',' ','P','s','y','s','t','r','i','k','e','!'};
				for(int i = 0; i < 50; i++) {
				    mewtwo_p->move_xy(416-i, 47+i);
				    sleep_ms(5);
				}
				mewtwo_p->move_xy(416,47);
				snorlax.health = snorlax.health - mewtwo.moves[2].damage;
				//uart.disp(snorlax.health);
				for(int i = 0; i < 22; i++){
					osd_p->wr_char(i + 5, 24, m2Psystrike[i]);
					sleep_ms(25);
				}
				if(snorlax.health < 0){
					snorlax.isFainted = true;
					fainted(&osd, snorlax);
				}
				break;
			}
			case 4: {//giga impact
				char m2Giga[] = {'M','e','w','t','w','o',' ','u','s','e','d',' ','G','i','g','a',' ','I','m','p','a','c','t','!'};
				for(int i = 0; i < 50; i++) {
				    mewtwo_p->move_xy(416-i, 47+i);
				    sleep_ms(5);
				}
				mewtwo_p->move_xy(416,47);
				snorlax.health = snorlax.health - mewtwo.moves[3].damage;
				//uart.disp(snorlax.health);
				for(int i = 0; i < 24; i++){
					osd_p->wr_char(i + 5, 24, m2Giga[i]);
					sleep_ms(25);
				}
				if(snorlax.health < 0){
					snorlax.isFainted = true;
					fainted(&osd, snorlax);
				}
				break;
			}
		}
	}


	void SnorlaxMove(Ps2Core *ps2_p, Pokemon& snorlax, Pokemon& mewtwo, OsdCore *osd_p, SpriteCore *snorlax_p){
		char buf = '\0';
		int moveNum = 0;
		bool pressed = false;
		while(pressed == false){
			if(ps2_p->get_kb_ch(&buf)){
				pressed = true;
			}
		}
			if(isOneToFour(buf)){
				int moveNum = (int) asciiToDigit(buf);
				uart.disp("movenum: ");
				uart.disp(moveNum);
				uart.disp("\n\r");
				uart.disp("buf: ");
				uart.disp(buf);
				uart.disp("\n\r");
				switch(moveNum){
					case 1://rest
					{
						//moveNum = '\0';
						osd_p->clr_screen();
						char snorlaxRest[] = {'S','n','o','r','l','a','x',' ','u','s','e','d',' ','R','e','s','t','!'};
						for(int i = 0; i < 50; i++) {
						    snorlax_p->move_xy(97+i, 279-i);
						    sleep_ms(5);
						}
						snorlax_p->move_xy(97,279);
						snorlax.health = snorlax.maxHP;
						for(int i = 0; i < 18; i++){
							osd_p->wr_char(i + 5, 24, snorlaxRest[i]);
							sleep_ms(25);
						}

						break;
					}

					case 2://bodyslam
					{
						//moveNum = '\0';
						osd_p->clr_screen();
						char snorlaxSlam[] = {'S','n','o','r','l','a','x',' ','u','s','e','d',' ','B','o','d','y','s','l','a','m','!'};
						for(int i = 0; i < 50; i++) {
							snorlax_p->move_xy(97+i, 279-i);
							sleep_ms(5);
						}
						snorlax_p->move_xy(97,279);
						mewtwo.health = mewtwo.health - snorlax.moves[1].damage;
						for(int i = 0; i < 22; i++){
							osd_p->wr_char(i + 5, 24, snorlaxSlam[i]);
							sleep_ms(25);
						}
						if(mewtwo.health < 0){
							mewtwo.isFainted = true;
							fainted(&osd, mewtwo);
						}
						break;
					}


					case 3://gigaimpact
					{
						//moveNum = '\0';
						osd_p->clr_screen();
						char snorlaxGiga[] = {'S','n','o','r','l','a','x',' ','u','s','e','d',' ','G','i','g','a',' ','I','m','p','a','c','t','!'};
						for(int i = 0; i < 50; i++) {
						    snorlax_p->move_xy(97+i, 279-i);
						    sleep_ms(5);
						}
						snorlax_p->move_xy(97,279);
						mewtwo.health = mewtwo.health - snorlax.moves[2].damage;
						for(int i = 0; i < 25; i++){
							osd_p->wr_char(i + 5, 24, snorlaxGiga[i]);
							sleep_ms(25);
						}
						if(mewtwo.health < 0){
							mewtwo.isFainted = true;
							fainted(&osd, mewtwo);
						}
						break;
					}


					case 4://bellydrum
					{
						//moveNum = '\0';
						osd_p->clr_screen();
						char snorlaxDrum[] = {'S','n','o','r','l','a','x',' ','u','s','e','d',' ','B','e','l','l','y',' ','D','r','u','m','!'};
						for(int i = 0; i < 50; i++) {
						    snorlax_p->move_xy(97+i, 279-i);
						    sleep_ms(5);
						}
						snorlax_p->move_xy(97,279);
						for(int i = 0; i < 24; i++){
							osd_p->wr_char(i + 5, 24, snorlaxDrum[i]);
							sleep_ms(25);
						}
						snorlax.moves[1].damage *= 2;
						snorlax.moves[2].damage *= 2;
						snorlax.health = floor(snorlax.health/2);
						break;
					}
				}
			}
	}


	double mapValue(double value, double fromMin, double fromMax, double toMin, double toMax) {
	    // First, normalize the value to the range [0, 1] within the original range
	    double normalizedValue = (value - fromMin) / (fromMax - fromMin);

	    // Then, scale the normalized value to the new range
	    double mappedValue = toMin + normalizedValue * (toMax - toMin);

	    return mappedValue;
	}

	void tapDetection(SpiCore *spi_p, bool &gameOver){
	    const uint8_t RD_CMD = 0x0b;
	       const uint8_t PART_ID_REG = 0x02;
	       const uint8_t DATA_REG = 0x08;
	       const float raw_max = 127.0 / 2.0;  //128 max 8-bit reading for +/-2g
	       static double magnitude, magnitudeTemp;
	       int totalLed, totalLedTemp;

	       int8_t xraw, yraw, zraw;
	       float x, y, z;
	       int id;

	       spi_p->set_freq(400000);
	       spi_p->set_mode(0, 0);
	       // check part id
	       spi_p->assert_ss(0);    // activate
	       spi_p->transfer(RD_CMD);  // for read operation
	       spi_p->transfer(PART_ID_REG);  // part id address
	       id = (int) spi_p->transfer(0x00);
	       spi_p->deassert_ss(0);
	       spi_p->assert_ss(0);    // activate
	       spi_p->transfer(RD_CMD);  // for read operation
	       spi_p->transfer(DATA_REG);  //
	       xraw = spi_p->transfer(0x00);
	       yraw = spi_p->transfer(0x00);
	       zraw = spi_p->transfer(0x00);
	       spi_p->deassert_ss(0);
	       x = (float) xraw / raw_max;
	       y = (float) yraw / raw_max;
	       z = (float) zraw / raw_max;
	       magnitude = abs(x)+abs(y)+abs(z);


	       if(!(magnitudeTemp <= (magnitude + 0.035) && magnitudeTemp >= (magnitude - 0.035))) {
	    	   totalLed = (int) mapValue(magnitude, 0.5, 2, 0, 15);
	    	   if(totalLed >= 5) {gameOver = true;}
	       }
	       magnitudeTemp = magnitude;
	}

	void win_screen(FrameCore *frame_p, OsdCore *osd_p){
	    frame_p->clr_screen(0xfff);
	    osd_p->set_color(0x0f0, 0x000);
	    osd_p->clr_screen();
	    char win[] = {'Y','O','U',' ','W','I','N'};
	    for(int i = 0; i < 7; i++){
	        osd_p->wr_char(i + 38, 10, win[i]);
	    }
	    char gameAgain[] = {'P','L','A','Y',' ','A','G','A','I','N','?'};
	    for(int i = 0; i < 11; i++){
	        osd_p->wr_char(i + 37, 15, gameAgain[i]);
	    }
	}


int main() {

//    Move Rest("Rest", 0, true);
//    Move BodySlam("Body Slam", 85, false);
//    Move GigaImpact("Giga Impact", 150, false);
//    Move BellyDrum("Belly Drum", 0, true);
//
//    Pokemon Snorlax("SNORLAX", 96, 319, 283, 100, 523, Rest, BodySlam, GigaImpact, BellyDrum);
//
//    Move FutureSight("Future Sight", 120, false);
//    Move Psychic("Psychic", 90, false);
//    Move Psystrike("Psystrike", 100, false);
//
//    Pokemon Mewtwo("MEWTWO", 296, 216, 447, 100, 415, FutureSight, Psychic, Psystrike, GigaImpact);

    //title screen
    osd.bypass(1);
    frame.bypass(1);
    gray.bypass(1);
    start_screen(&frame, &osd);
    cursor.bypass(1);
    mewtwo.bypass(1);
    snorlax.bypass(1);
    start_key(&ps2);

    osd.clr_screen();


while (1) {
	    Move Rest("Rest", 0, true);
	    Move BodySlam("Body Slam", 85, false);
	    Move GigaImpact("Giga Impact", 150, false);
	    Move BellyDrum("Belly Drum", 0, true);

	    Pokemon Snorlax("SNORLAX", 96, 319, 283, 100, 523, Rest, BodySlam, GigaImpact, BellyDrum);

	    Move FutureSight("Future Sight", 120, false);
	    Move Psychic("Psychic", 90, false);
	    Move Psystrike("Psystrike", 100, false);

	    Pokemon Mewtwo("MEWTWO", 296, 216, 447, 100, 415, FutureSight, Psychic, Psystrike, GigaImpact);
	    bool gameOver = false;
	    bool won = false;
//    test_start(&led);
  //  bypass all cores
//    frame.bypass(1);
      bar.bypass(1);
      frame.bypass(0);
      gray.bypass(1);
      osd.bypass(1);
      cursor.bypass(1);
      mewtwo.bypass(1);
//      osd.bypass(1);
      snorlax.bypass(1);
//      sleep_ms(3500);
      environmentInit(&frame);




      //start positions
      mewtwo.move_xy(416, 47);
      snorlax.move_xy(97,279);
      cursor.move_xy(320, 240);



      osd.set_color(0x111, 0x000);
      osd.bypass(0);

	    snorlax.bypass(0);
	    osd.clr_screen();
	    char intro[] = {'A',' ','W','i','l','d',' ','M','e','w','t','w','o',' ','A','p','p','e','a','r','e','d','!'};
	    for(int i = 0; i < 23; i++){
		    osd.wr_char(i + 3, 24, intro[i]);
		    sleep_ms(50);
	    }
	    show_status(&frame,&osd,&Snorlax,&Mewtwo);
	    mewtwo.bypass(0);
	    osd.clr_screen();
  	  show_status(&frame,&osd,&Snorlax,&Mewtwo);

      while (gameOver == false){
    	  osd.clr_screen();
		    show_status(&frame,&osd,&Snorlax,&Mewtwo);
    	  char moves1[] = {'1','.','R','e','s','t',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','2','.','B','o','d','y',' ','S','l','a','m'};
    	  char moves2[] = {'3','.','G','i','g','a',' ','I','m','p','a','c','t',' ',' ','4','.','B','e','l','l','y',' ','d','r','u','m'};
		    for(int i = 0; i < 27; i++){
			    osd.wr_char(i + 5, 24, moves1[i]);
			    osd.wr_char(i + 5, 26, moves2[i]);
			    sleep_ms(20);
		    }
		    char pick[] = {'S','e','l','e','c','t',' ','a',' ','M','o','v','e','.','.','.'};
		    for(int i = 0; i < 16; i++){
			    osd.wr_char(i + 5, 28, pick[i]);
			    sleep_ms(20);
		    }

		    tapDetection(&spi, gameOver);
		    show_status(&frame,&osd,&Snorlax,&Mewtwo);
		    SnorlaxMove(&ps2, Snorlax, Mewtwo, &osd, &snorlax);
		    show_status(&frame,&osd,&Snorlax,&Mewtwo);
		    MewtwoAtk(Snorlax, Mewtwo, &osd, &mewtwo);
		    show_status(&frame,&osd,&Snorlax,&Mewtwo);
		    tapDetection(&spi, gameOver);

		    if(Mewtwo.isFainted || Snorlax.isFainted)
			    gameOver = true;
		    if(Mewtwo.isFainted)
			    won = true;

        } //while
      if(gameOver && won)
    	  win_screen(&frame,&osd);
      else
    	  game_over(&frame,&osd);
      start_key(&ps2);
   } // while
} //main




//int main() {
//
//	//pokemon classes
//	Move Rest("Rest", 0, true);
//	Move BodySlam("Body Slam", 85, false);
//	Move GigaImpact("Giga Impact", 150, false);
//	Move BellyDrum("Belly Drum", 0, true);
//	Pokemon Snorlax("SNORLAX", 96, 319, 283, 100, 523, Rest, BodySlam, GigaImpact, BellyDrum);
//
//	Move Amnesia("Amnesia", 0, true);
//	Move Psychic("Psychic", 90, false);
//	Move Thunderbolt("Thunderbolt", 90, false);
//	Move Recover("Recover", 0, true);
//	Pokemon Mewtwo("MEWTWO", 296, 216, 447, 100, 415, Amnesia, Psychic, Thunderbolt, Recover);
//
//
//	//title screen
//	start_screen(&frame, &osd);
//	cursor.bypass(1);
//	mewtwo.bypass(1);
//	snorlax.bypass(1);
//	bar.bypass(1);
//
//	start_key(&ps2);
//
//
//	osd.clr_screen();
//
//   while (1) {
//	  //start sequence
//	  for(int i = 0; i < 2; i++) { environmentInit(&frame); }
//	  cursor.bypass(0);
//	  mewtwo.bypass(0);
//	  snorlax.bypass(0);
//
//	  //start positions
//	  mewtwo.move_xy(416, 47);
//	  snorlax.move_xy(97,279);
//	  cursor.move_xy(320, 240);
//
//	osd.set_color(0x111, 0xfff);
//	osd.bypass(0);
//	show_status(&frame, &osd, &Snorlax, &Mewtwo);
//
//
//	char intro[] = {'A', ' ', 'W', 'i', 'l', 'd', ' ', 'M', 'E', 'W', 'T', 'W', 'O', ' ', 'A', 'p', 'p', 'e', 'a', 'r', 'e', 'd', '!'};
//	for(int i = 0; i < 23; i++){
//		osd.wr_char(i + 3, 25, intro[i]);
//		sleep_ms(20);
//		}
//	while(!ps2_p->get_kb_ch(&ch)){
//
//		}
//
//	cursor.move_xy(595, 440);
//
//
//      while (sw.read(0)) {
//         snorlax.bypass(sw.read(1));
////         bar.bypass(sw.read(2));
//         mewtwo.bypass(sw.read(3));
//         cursor.bypass(sw.read(6));
//      } //while
//   } // while
//} //main
















