module bsc(out, clk, rst, enable);
 output reg [3:0]out;
 //output reg send;
 input clk, rst, enable;

	always @(posedge clk)
	if (rst) begin
		out <= 4'b0000;
		//send <= 0;
	//end else if (enable & (out == 4'b1000)) begin
		//send <= 1;
		//out <= out + 4'b0001;
	end 
	else if (enable) begin
		out <= out + 4'b0001;
		//send <= 0;
	end //else begin
		//out <= out + 4'b0001;
		//send <= 0;
	//end
	
endmodule