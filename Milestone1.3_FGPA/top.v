module top (input stm32_in, input stm32_in2, output reg[1:0]led_out);
wire [1:0] combined;
    assign combined = {stm32_in, stm32_in2};
    always@(*)begin
    case (combined)
        2'b00: led_out = 2'b00;
        2'b01: led_out = 2'b01;
        2'b10: led_out = 2'b10;
        2'b11: led_out = 2'b11;
        default: led_out = 2'b00;
    endcase
    end

endmodule