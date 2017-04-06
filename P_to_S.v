module P_to_S (clk, reset, data_in, load, bsc, bic, data_out);
	input clk, reset;
	input load;
	input [7:0] data_in;
	input [3:0] bsc;
	input [3:0] bic;
	output data_out;
	
	reg [9:0] data_temp;
	
	always @(posedge clk)
		if (reset)
			data_temp <= 10'b1111111111;
		else if(load == 1'b1 & bic == 4'b0000 & bsc == 4'b1000) begin
			//data_out <= 1'b0; //start bit
			data_temp[9] <= 1'b0;
			data_temp[8:1] <= data_in;
			data_temp[0] <= 1'b1;
			//data_out <= 1'b0;
		end else if(load == 1'b1 & bsc == 4'b1000 & bic != 1010) begin
			data_temp <= {data_temp[8:0], 1'b1};
			//data_out <= data_temp[9];		
		end
		//else if(bic == 1010) data_out <= 1'b1;
		
		assign data_out = data_temp[9];

endmodule 