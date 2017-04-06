module DE1_SoC (CLOCK_50, KEY, LEDR, SW, GPIO_0, GPIO_1, VGA_HS, VGA_VS, VGA_R, VGA_G, VGA_B, VGA_SYNC_N, VGA_BLANK_N, VGA_CLK); 
	output [9:0] LEDR;
	input [9:0] SW;
	input [3:0] KEY;
	input CLOCK_50;
	input [35:0] GPIO_0;
	output [35:0] GPIO_1;
	output VGA_HS, VGA_VS;
	output VGA_CLK;
	output VGA_BLANK_N;
	output VGA_SYNC_N;
	output [7:0] VGA_R, VGA_G, VGA_B;
	
	wire [31:0] clk;
	wire SRAM_flag, send_bit_bsc1, send_bit_bsc2;
	wire [7:0] SRAM_data_in, SRAM_data_out, SRAM_address;
	wire [3:0] bic_in, bsc_in, bic_out, bsc_out;
	wire char_r, char_s;
	wire Load_flag, transmit_enable, GetStartBit, char_r_flag, char_s_flag;
	wire [7:0] outputData_fromMicroprocessor;
	wire [7:0] outputData_fromStoP;
	
	clock_divider clk_d(CLOCK_50, clk);
	
	mem m(CLOCK_50, SRAM_flag, SRAM_address, SRAM_data_in, SRAM_data_out);
	startBit_Detect startbit(.clk(clk[7]), .reset(SW[8]), .data_in(GPIO_0[0]), .bic(bic_in), .enable(GetStartBit));
	
	Serial2Parallel s2p(.clk(clk[7]), .reset(SW[8]), .data_in(GPIO_0[0]), .bsc(bsc_in), .bic(bic_in), .data_out(outputData_fromStoP));
	P_to_S ptos(.clk(clk[7]), .reset(SW[8]), .data_in(outputData_fromMicroprocessor), .load(Load_flag), .bsc(bsc_out), .bic(bic_out), .data_out(GPIO_1[0]));
	bic bic1(.out(bic_in), .clk(clk[7]), .rst(SW[8]), .enable(GetStartBit), .sample(bsc_in), .char_rs(char_r), .flag(char_r_flag));
	bsc bsc1(.out(bsc_in), .clk(clk[7]), .rst(SW[8]), .enable(GetStartBit));
	
	bic bic2(.out(bic_out), .clk(clk[7]), .rst(SW[8]), .enable(transmit_enable), .sample(bsc_out), .char_rs(char_s), .flag(char_s_flag));
	bsc bsc2(.out(bsc_out), .clk(clk[7]), .rst(SW[8]), .enable(transmit_enable));
	
	
	wire [11:0] enable;
	wire getAttack;
	wire [9:0] blood1, blood2, magic;
	wire [2:0] life; // pokemon left (decode currentMon)
	wire [5:0] userin;
	
	GraphicDriver gd (CLOCK_50, SW[8], VGA_HS, VGA_VS, VGA_R, VGA_G, VGA_B, VGA_SYNC_N, VGA_BLANK_N, VGA_CLK, KEY[3:0], enable, getAttack, blood1, blood2, magic, life, userin);
	
 microprocessor u0 (
        .addr_export             (SRAM_address),             //             addr.export
        .charreceived_export     (char_r),     //     charreceived.export
        .charreceivedflag_export (char_r_flag), // charreceivedflag.export
        .charsent_export         (char_s),         //         charsent.export
        .charsentflag_export     (char_s_flag),     //     charsentflag.export
        .clk_clk                 (CLOCK_50),                 //              clk.clk
        .sram_data_in_export     (SRAM_data_in),             //             data.export
        .flag_export             (SRAM_flag),             //             flag.export
        .load_export             (Load_flag),             //             load.export
        .paralleldatai_export    (outputData_fromStoP),    //    paralleldatai.export
        .paralleldataled_export  (LEDR[7:0]),  //  paralleldataled.export
        .paralleldatao_export    (outputData_fromMicroprocessor),    //    paralleldatao.export
        .reset_reset_n           (SW[9]),           //            reset.reset_n
        .transmitenable_export   (transmit_enable),   //   transmitenable.export
        .userinput_export        (userin),         //        userinput.export
		  .sram_data_out_export    (SRAM_data_out),     //    sram_data_out.export
		  .enable_enemy_export     (enable[11:6]),     //     enable_enemy.export
        .enable_self_export      (enable[5:0]),        
		  .current_hp_export       (blood2[7:0]),       //       current_hp.export
        .current_mp_export       (magic[7:0]),       //       current_mp.export
        .current_ehp_export      (blood1[7:0]),      //      current_ehp.export
        .life_export             (life),              //             life.export
		  .get_attacked_export     (getAttack)      //     get_attacked.export
    );


endmodule


// divided_clocks[0] = 25MHz, [1] = 12.5Mhz, ... [23] = 3Hz, [24] = 1.5Hz, [25] = 0.75Hz, ...
module clock_divider (clock, divided_clocks);
	input clock;
	output [31:0] divided_clocks;
	reg [31:0] divided_clocks;
	initial
	divided_clocks = 0;
	always @(posedge clock)
	divided_clocks = divided_clocks + 1;
endmodule
