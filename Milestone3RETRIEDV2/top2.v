module top2 (inout sda_pin, input switch);

wire sda_oe;

assign sda_oe = switch;
assign sda_pin = (sda_oe) ? 1'bz : 1'b0;


endmodule
