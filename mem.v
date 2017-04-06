module mem(clk, write, address, data_in, data_out);
	input clk, write;
	input [7:0] address;
	input [7:0] data_in;
	output reg [7:0] data_out;
	
	reg [7:0] mem [2047:0];
	
	always @(posedge clk)
	begin
	if(write) begin
		mem[address] <= data_in;
		end
	else begin
		data_out <= mem[address];
		end
	end

endmodule 