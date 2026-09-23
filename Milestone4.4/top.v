module top(input clk_line, input cs_line, input mosi_line, output miso_line, input clk, output [2:0]led);

reg[2:0] sclk_sync;
reg [2:0] cs_sync;
reg [1:0] mosi_sync;
wire sclk_posedge;
wire sclk_negedge;

localparam BASE_CYCLES = 32'd6_000_000;



always@(posedge clk) begin
    sclk_sync <= {sclk_sync[1:0], clk_line}; 
    cs_sync <= {cs_sync[1:0], cs_line};
    mosi_sync <= {mosi_sync[0], mosi_line};
end

assign sclk_posedge = sclk_sync[1] && !sclk_sync[2];
assign sclk_negedge = !sclk_sync[1] && sclk_sync[2];
wire mosi_data = mosi_sync[1];
wire cs_active = !cs_sync[1];

// -----------------------------

reg [7:0] tx_shift_reg;
reg [7:0] rx_shift_reg;
reg [3:0] bit_cnt;

reg rw = 1'b0;
reg[6:0] address = 7'h0;

reg[7:0] led_reg = 8'h00;
reg[7:0] counter_reg = 8'h00;

reg reset = 1'b0;




assign led[2:0] = blink_state ? ~led_reg[2:0] : 3'b111;

assign miso_line = cs_active ? tx_shift_reg[7] : 1'bz;

wire[31:0] threshold = (counter_reg + 1) * BASE_CYCLES;
reg[31:0] timer_cnt = 32'd0;
reg blink_tick = 1'b0;
reg blink_state = 1'b0;

//-------------------------------------

always @(posedge clk or posedge reset) begin
    if (reset) begin
        blink_tick <= 1'b0;
        timer_cnt <= 32'd0;
    end else if (timer_cnt >= (threshold - 1)) begin
        timer_cnt <= 32'd0;
        blink_tick <= 1'b1;
    end else begin
        timer_cnt <= timer_cnt + 1;
        blink_tick <= 1'b0;
    end
end

always @(posedge clk or posedge reset) begin
    if (reset) begin
        blink_state <= 1'b0;
    end else if (blink_tick) begin
        blink_state <= ~blink_state;
    end
end

always @(posedge clk) begin
    if (!cs_active) begin
        bit_cnt <= 0;
        tx_shift_reg <= 8'h00;
    end else begin

    // RISING EDGE, SAMPLE MOSI
    if (sclk_posedge) begin
        rx_shift_reg <= {rx_shift_reg[6:0], mosi_data};
        bit_cnt <= bit_cnt + 1;

        if (bit_cnt == 4'd7) begin
            rw <= rx_shift_reg[6];
            address <= {rx_shift_reg[5:0], mosi_data};

            // READ OPERATION
            if (rx_shift_reg[6] == 1'b0) begin
                case({rx_shift_reg[5:0], mosi_data}) 
                    7'h01: tx_shift_reg <= led_reg;
                    7'h02: tx_shift_reg <= counter_reg;
                    default: tx_shift_reg <= tx_shift_reg;
                endcase
            end
        end

        if (bit_cnt == 4'd15) begin
            if (rw == 1'b1) begin
                case (address)
                    7'h01: led_reg <= {rx_shift_reg[6:0], mosi_data};
                    7'h02: counter_reg <= {rx_shift_reg[6:0], mosi_data};
                endcase
            end
        end

        // NEGEDGE, SHIFTS MISO OUT

        
    end
    if (sclk_negedge) begin
            if (bit_cnt > 4'd8 && rw == 1'b0) begin
                tx_shift_reg <= {tx_shift_reg[6:0], 1'b0};
            end
    end

    end
end
endmodule