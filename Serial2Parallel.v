module Serial2Parallel (clk, reset, data_in, bsc, bic, data_out);
	input clk, reset, data_in;
	input [3:0] bsc;
	input [3:0] bic;
	output [7:0] data_out;
	
	reg [9:0] data_temp;
	
	always @(posedge clk)
		if (reset)
			data_temp <= 10'b0;
		else if (bsc == 4'b1000)
			data_temp <= {data_temp[8:0], data_in};
		
	assign data_out = data_temp[8:1];
		
endmodule 