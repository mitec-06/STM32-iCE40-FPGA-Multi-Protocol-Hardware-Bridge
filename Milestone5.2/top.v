module top(input clk, output fpga_line);

    reg[23:0] counter = 24'h0;
    always@(posedge clk) begin
        fpga_line <= 1'b1;
        counter <= counter + 1;
        if (counter == 24'hFFFFFF) begin
            fpga_line <= 1'b0;
        end
    end

endmodule