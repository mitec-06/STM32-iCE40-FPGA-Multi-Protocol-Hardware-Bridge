module top(input clk_line, input clk, output led);

reg[2:0] sclk_sync;
reg[23:0] timer;
wire sclk_posedge;

assign led = (timer > 0);
assign sclk_posedge = sclk_sync[1] && !sclk_sync[2];

always @(posedge clk) begin
    sclk_sync <= {sclk_sync[1:0], clk_line}; 
    if (sclk_posedge) begin
     timer <= 24'd12000000;
    end
    else if (timer > 0 && !sclk_posedge) begin
        timer <= timer - 1;
    end
end
endmodule


