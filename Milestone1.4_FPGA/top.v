module top (input stm32_in, input stm32_in2, input stm32_in3, output reg[2:0]led_out);
    assign led_out = {stm32_in, stm32_in2, stm32_in3};

endmodule