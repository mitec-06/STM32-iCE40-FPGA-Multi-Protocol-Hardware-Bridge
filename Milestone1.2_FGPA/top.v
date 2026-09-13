module top (input clk, input stm32_in, input stm32_in2, output led_out, output led_out2, output led_out3);

// Adding delay
/*
reg [50:0] delay;
always @(posedge clk) begin
    delay <= delay + 1;
end
*/

assign led_out = stm32_in; // BLUE

assign led_out2 = stm32_in; //GREEN
assign led_out3 = stm32_in2; // RED

endmodule