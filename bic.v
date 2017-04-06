module bic(out, clk, rst, enable, sample, char_rs, flag);
 output reg [3:0]out;
 output reg char_rs;
 input clk, rst, enable, flag;
 input [3:0] sample;

 
	always @(posedge clk)
	if (rst) begin
		out <= 4'b0000;
		char_rs <= 1'b0;
	end else if(flag) begin
		char_rs <= 1'b0;
	end else if (enable & (sample == 4'b1111) & out != 4'b1010) begin
		out <= out + 4'b0001;
		char_rs <= 1'b0;
	end else if (out == 4'b1010) begin
		out <= 4'b0000;
		char_rs <= 1'b1;
	end else if (out == 4'b1011) begin
		out <= 4'b0000;
		char_rs <= 1'b0;
	end //else if(enable == 0) begin
		//out <= 4'b0000;
		//char_rs <= 1'b0;
	//end
	
	//assign char_rs = (out == 4'b1010);

	
	
/*
	always @(posedge clk)
	if (rst) begin
		out <= 4'b0000;
		char_rs <= 1'b0;
	end else if (enable & (sample == 4'b1111) & out != 4'b1010) begin
		out <= out + 4'b0001;
		char_rs <= 1'b0;
	end else if (out == 4'b1001 & (sample == 4'b1001)) begin
		//out <= 4'b0000;
		char_rs <= 1'b1;
	end else if (out == 4'b1010) begin
		out <= 4'b0000;
		char_rs <= 1'b0;
	end else if(enable == 0) begin
		out <= 4'b0000;
		char_rs <= 1'b0;
	end
*/
	
endmodule