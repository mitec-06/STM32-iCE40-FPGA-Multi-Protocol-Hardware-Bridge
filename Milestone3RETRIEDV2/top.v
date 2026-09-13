module top (input rx_uart, inout sda_pin, input clk);

    wire [7:0]rx_data;
    wire rx_valid;

    uart_rx #(
        .DATA_WIDTH(8)
    ) u_uart_rx (
        .clk           (clk),
        .rst           (1'b0),
        .m_axis_tdata  (rx_data),
        .m_axis_tvalid (rx_valid),
        .m_axis_tready (1'b1),      // Always ready to receive
        .rxd           (rx_uart),
        .prescale       (16'd13)
    );

    reg [24:0] timer = 0;
    wire sda_oe;
    assign sda_oe = (timer > 0);
    assign sda_pin = (sda_oe) ? 1'b0 : 1'bz;
    always @(posedge clk) begin
        if (rx_valid) begin
            case (rx_data)
                "1" : timer <= 25'd6000000;
                "2" : timer <= 25'd12000000;
                "3" : timer <= 25'd24000000;
                default : timer <= timer;
            endcase 
        end else if (timer > 0) begin
            timer <= timer - 1'b1;
        end
    end

endmodule
