module startBit_Detect(clk, reset, data_in, bic, enable);
	input clk, reset, data_in;
	input [3:0] bic;
	output reg enable;
	
	always @(*)
	if (reset)
		enable = 1'b0;
	else if (bic == 4'b0000 & data_in == 1'b0)
		//begin 
			//if (data_in == 1'b0) 
				enable = 1'b1;
			//else 
				//enable <= 1'b0;
		//end
	else if (bic == 4'b1010)
		enable = 1'b0;
	//else enable <= enable;
	
endmodule 