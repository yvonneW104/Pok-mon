#include "sys/alt_stdio.h"
#include "altera_avalon_pio_regs.h"
#include <time.h>


#define transmit_enable (volatile char *) 0x9140
#define p_data_in (volatile char *) 0x9130   //output
#define load (volatile char *) 0x9120
#define LEDR (volatile char *) 0x9110
#define char_sent (volatile char *) 0x9100
#define p_data_out (volatile char *) 0x90f0  //input
#define char_received (volatile char *) 0x90e0
#define read_write (volatile char *) 0x90d0
#define data_in (volatile char *) 0x90c0
#define addr (volatile char *) 0x90b0
#define char_r_flag (volatile char *) 0x90a0
#define char_s_flag (volatile char *) 0x9090
#define userin (volatile char *) 0x9080
#define data_out (volatile char *) 0x9070
#define enable_self (volatile char *) 0x9060
#define HP_Left (volatile char *) 0x9050
#define MP_Left (volatile char *) 0x9040
#define Enemy_HP_Left (volatile char *) 0x9030
#define Life (volatile char *) 0x9020
#define enable_enemy (volatile char *) 0x9010
#define get_attacked (volatile char *) 0x9000



#define SEND_BAUD_WIDTH 100
#define RECEIVE_BAUD_WIDTH 100
int busyAddr; // memory busy boundary

typedef struct {
	char* name;
	int currentMon;
	int mon[3];
	int tg;
	int mg;
} player;

// function prototype
player initPlayer (player, int, int, int);
void memAlloc(player);
void setValue(char, char);
int getValue(char);
char pokemonSkill(int, int, int);
char pokemonProp(int, int);
char items(int, int);
void write(char);
char read(void);
int keyDecoder(int);
int getHP_Addr(int, int);
int getMP_Addr(int, int);
int getSkillDamage_Addr(int, int, int);
int getSkillCost_Addr(int, int, int);
int getTangoA_Addr(int);
int getTangoH_Addr(int);
int getMangoA_Addr(int);
int getMangoH_Addr(int);
char singlePokeDecoder(int);

int main() {
	int round; // 0 for attack, 1 for incoming
	char currentHP, currentMP, currentPokemon, maxHP, maxMP;
	char currentEHP, currentEnemy, enemyMaxHP;
	char dmg, cst, tangoA, mangoA; // damage, cost
	char healM, healH;
	char inDmg, eHeal; // enemy's damadge, heal
	char command = 0;
	int endGame;
	int count = 2;
	busyAddr = 0;

	player JunE; //self
	player JunJun; //enemy
	JunE.name = "JUNE";
	JunE = initPlayer(JunE, 3, 4, 5);
	JunJun.name = "JUNJUN";
	JunJun = initPlayer(JunJun, 0, 1, 2);

	usleep(1000);

	maxHP = getValue(getHP_Addr(JunE.currentMon, 0));
	maxMP = getValue(getMP_Addr(JunE.currentMon, 0));
	enemyMaxHP = getValue(getHP_Addr(JunJun.currentMon, 1));

	round = 0; // 0 for attack
	endGame = 0;

	while(endGame == 0) {
		currentHP = getValue(getHP_Addr(JunE.currentMon, 0));
		currentMP = getValue(getMP_Addr(JunE.currentMon, 0));
		currentEHP = getValue(getHP_Addr(JunJun.currentMon, 1));
		*HP_Left = currentHP;
		*MP_Left = currentMP;
		*Enemy_HP_Left = currentEHP;
		*Life = 0b111 >> JunE.currentMon;
		*enable_self = singlePokeDecoder(JunE.mon[JunE.currentMon]);
		*enable_enemy = singlePokeDecoder(JunJun.mon[JunJun.currentMon]);
		*get_attacked = 0;

		//*enable_Mon = pokemonDecoder(JunE, JunJun);
		alt_printf("current HP: %x\n", currentHP);
		alt_printf("current MP: %x\n", currentMP);
		alt_printf("Your enemy's monster current HP: %x\n", currentEHP);
		alt_printf("\n");
		/*
		int k;
		for (k = 0; k < 80; k++) {
			alt_printf("0x%x: %x \t", k, getValue(k));
		}
		alt_printf("\n");
		alt_printf("\n");
		*/
		dmg = 0; // init dmg
		healH = 0; // init heal
		healM = 0;
		cst = 0; //init cst
		command = 0;
		//print SRAM

		if (round == 0) { //attack
			//alt_printf("userin: %x", *userin);
			while (command == 0) {
				//command = *userin;
				command = keyDecoder(*userin); //check command
			}
			//alt_printf("got command & userin: %x", *userin);

			if (command > 0 && command <= 4) { //skill
				healH = 0;
				healM = 0;
				cst = getValue(getSkillCost_Addr(JunE.currentMon, command,  0)); //get cost of MP
				if (currentMP - cst >= 0) {
					dmg = getValue(getSkillDamage_Addr(JunE.currentMon, command, 0)); // get the damage
					currentMP -= cst; //new MP
					setValue(getMP_Addr(JunE.currentMon, 0), currentMP); //update new MP
				} else {
					alt_printf("Not enough MP.\n");
					dmg = 0;
					healH = 0;
					healM = 0;
				}
			} else if (command == 5) { //use tango
				dmg = 0;
				healM = 0;
				tangoA = getValue(getTangoA_Addr(0)); //get tango's amount
				if (tangoA > 0) {
					setValue(getTangoA_Addr(0), tangoA - 1); //decrement tango's amount
					healH = getValue(getTangoH_Addr(0));
				} else {
					alt_printf("No more Tango left.\n");
					dmg = 0;
					healH = 0;
					healM = 0;
				}
			} else if (command == 6) { //use mango
				dmg = 0;
				healH = 0;
				mangoA = getValue(getMangoA_Addr(0)); //get mango's amount
				if (mangoA > 0) {
					setValue(getMangoA_Addr(0), mangoA - 1); //decrement mango's amount
					healM = getValue(getMangoH_Addr(0));
				} else {
					alt_printf("No more Mango left.\n");
					dmg = 0;
					healM = 0;
					healH = 0;
				}
			} else {
				dmg = 0;
				healM = 0;
				healH = 0;
			}

			if (dmg != 0 || healM != 0 || healH != 0) {
				if(currentMP + healM > maxMP) {
					healM = maxMP - currentMP;
				}
				setValue(getMP_Addr(JunE.currentMon, 0), currentMP + healM);

				if(currentHP + healH > maxHP) {
					healH = maxHP - currentHP;
				}
				setValue(getHP_Addr(JunE.currentMon, 0), currentHP + healH);

				write(dmg);
				usleep(1000);
				write(healH);

				if (dmg != 0) {
					currentEHP -= dmg;
					if (currentEHP <= 0 && JunJun.currentMon != 2) {
						setValue(getHP_Addr(JunJun.currentMon, 1), 0);
						JunJun.currentMon++;
						alt_printf("You beat one monster!\n");
						enemyMaxHP = getValue(getHP_Addr(JunJun.currentMon, 1));
					} else if (currentEHP <= 0 && JunJun.currentMon == 2) {
						setValue(getHP_Addr(JunJun.currentMon, 1), 0);
						*Enemy_HP_Left = 0;
						alt_printf("You win\n");
						*enable_enemy = 0;
						endGame = 1;
					} else {
						setValue(getHP_Addr(JunJun.currentMon, 1), currentEHP);
					}
				}
				alt_printf("damage: %x\n", dmg);
				round = 1; //switch round

			}
			command = 0;
			//round = 1; //switch round
		} else { //incoming
			//char trash = read();
			inDmg = read();
			alt_printf("inDmg: %x\n", inDmg);
			//*char_r_flag = 0;
			usleep(500);
			eHeal = read();
			//*char_r_flag = 0;
			//alt_printf("inDmg: %x\n", inDmg); //get damage from enemy
			alt_printf("eHeal: %x\n", eHeal); //enemy's heal

			if (inDmg != 0 && eHeal == 0) {
				currentHP -= inDmg;
				*get_attacked = 1;
				usleep(500000);
				*get_attacked = 0;
				if (currentHP <= 0 && JunE.currentMon != 2) { //die
					setValue(getHP_Addr(JunE.currentMon, 0), 0); //set currentMon to 0;
					JunE.currentMon++; //increment current monster
					maxHP = getValue(getHP_Addr(JunE.currentMon, 0));
					maxMP = getValue(getMP_Addr(JunE.currentMon, 0));
					alt_printf("You lose one monster. Change to No.%x.\n", count);
					count++;
				} else if (currentHP <= 0 && JunE.currentMon == 2){ //June die
					alt_printf("You lose\n");
					*enable_self = 0;
					*Life = 0;
					endGame = 1;
					setValue(getHP_Addr(JunE.currentMon, 0), 0);
					*HP_Left = 0;
				} else {
					setValue(getHP_Addr(JunE.currentMon, 0), currentHP); //update current HP;
				}
				round = 0;
			} else if (inDmg == 0 && eHeal != 0) {
				setValue(getHP_Addr(JunJun.currentMon, 1), currentEHP + eHeal); //update enenmy's HP
				round = 0;
			} else {
				round = 0;
			}
			command = 0;
			round = 0;
		}
	}
	//alt_printf("game over\n");
	return 0;
}

void write(char userIn) {
	//*p_data_in = 0b11111111;
	*char_s_flag = 0;
	*p_data_in = userIn;
	//*transmit_enable = 1;
	//*p_data_in = userIn;
	*load = 1;
	*transmit_enable = 1;
	while(*char_sent != 1) {
		usleep(3);
	}
	*load = 0;
	*transmit_enable = 0;
	*char_s_flag = 1;
	usleep(100);
	*char_s_flag = 0;
}

char read(void) {
	*char_r_flag = 0;
	while(*char_received != 1) {
		usleep(3);
	}
	*char_r_flag = 1;
	usleep(100);
	*char_r_flag = 0;
	return *p_data_out;
}

//write data to SRAM
void setValue(char address, char data) {
	*addr = address;
	*read_write = 1;
	*data_in = data;
	*read_write = 0;
}

//read data from SRAM
int getValue(char address) {
	*addr = address;
	return *data_out;
}

player initPlayer (player plr, int mon0, int mon1, int mon2) {
	plr.mon[0] = mon0;
	plr.mon[1] = mon1;
	plr.mon[2] = mon2;
	plr.currentMon = 0;
	memAlloc(plr);
	return plr;
}

void memAlloc(player plr) {
	int i, a;
	for (i = 0; i < 3; i++) {
		setValue(busyAddr + i * 12, i + 1); // pokemon No.
		setValue(busyAddr + 1 + i * 12, 0);
		setValue(busyAddr + 2 + i * 12, pokemonProp(plr.mon[i], 0)); //hp
		setValue(busyAddr + 3 + i * 12, pokemonProp(plr.mon[i], 1)); //mp
		// skill memory allocation
		for(a = 0; a < 4; a++) {
			setValue(busyAddr + 4 + i * 12 + a * 2, pokemonSkill(plr.mon[i], a, 0)); // damage
			setValue(busyAddr + 5 + i * 12 + a * 2, pokemonSkill(plr.mon[i], a, 1)); //cose
		}
	}
	//tango
	setValue(busyAddr + 36, items(0, 0)); // amount
	setValue(busyAddr + 37, items(0, 1)); // heal
	//mango
	setValue(busyAddr + 38, items(1, 0)); // amount
	setValue(busyAddr + 39, items(1, 1)); // heal

	busyAddr += 40;
}

char pokemonSkill(int pokemonNum, int skillNum, int type) {
	char skill[6][4][2]= {
			{{5, 0}, {25, 25}, {15, 10}, {30, 25}}, // pokemon 0
			{{5, 0}, {40, 50}, {30, 25}, {20, 15}}, // pokemon 1
			{{5, 0}, {20, 15}, {20, 20}, {20, 25}},  // pokemon 2
			{{5, 0}, {30, 30}, {25, 25}, {15, 15}}, // pokemon 3
			{{10, 0}, {35, 35}, {20, 20}, {25, 30}}, // pokemon 4
			{{10, 0}, {35, 30}, {25, 30}, {30, 35}} // pokemon 5
	};
	return skill[pokemonNum][skillNum][type];
}


char pokemonProp (int pokemonNum, int prop) {
	char pokemonProperty[6][2] = {
								{100, 90},
								{90, 100},
								{120, 70},
								{110, 80},
								{80, 110},
								{80, 120}
								};
	return pokemonProperty[pokemonNum][prop];
}

char items(int type, int prop) {
	char item[2][2] = {
					  {2, 50}, // tango
 					  {2, 50}  // mango
					  };
	return item[type][prop];
}


int getHP_Addr(int currentMon, int playerNo){
	return playerNo * 40 + currentMon * 12 + 2;
}

int getMP_Addr(int currentMon, int playerNo){
	return playerNo * 40 + currentMon * 12 + 3;
}

int getSkillDamage_Addr(int currentMon, int skill, int playerNo){
	return playerNo * 40 + currentMon * 12 + skill * 2 + 2;
}

int getSkillCost_Addr(int currentMon, int skill, int playerNo){
	return playerNo * 40 + currentMon * 12 + skill * 2 + 3;
}

int getTangoA_Addr(int playerNo){
	return playerNo * 40 + 36;
}

int getTangoH_Addr(int playerNo){
	return playerNo * 40 + 37;
}

int getMangoA_Addr(int playerNo){
	return playerNo * 40 + 38;
}

int getMangoH_Addr(int playerNo){
	return playerNo * 40 + 39;
}

// decode the key input
int keyDecoder(int command) {
	switch(command) {
		case 0b00000001:
			return 1;
		case 0b00000010:
			return 2;
		case 0b00000100:
			return 3;
		case 0b00001000:
			return 4;
		case 0b00010000:
			return 5;
		case 0b00100000:
			return 6;
		default :
			return 0;
	}
}


char singlePokeDecoder(int currentMon) {
	char enableBus = 0;
	switch(currentMon) {
		case 0:
			return 0b000001;
		case 1:
			return 0b000010;
		case 2:
			return 0b000100;
		case 3:
			return 0b001000;
		case 4:
			return 0b010000;
		case 5:
			return 0b100000;
		default:
			return enableBus;
	}

}


























