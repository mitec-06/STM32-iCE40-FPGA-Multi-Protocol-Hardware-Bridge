module top(input clk_line, input cs_line, input mosi_line, input clk, output [2:0]led);

reg[2:0] sclk_sync;
reg [2:0] cs_sync;
reg [1:0] mosi_sync;
reg[7:0] shift_reg;
reg [3:0] bit_cnt;
reg [2:0] color_store = 3'b000;
wire sclk_posedge;

assign led[2] = ~color_store[2];
assign led[1] = ~color_store[1];
assign led[0] = ~color_store[0];

assign sclk_posedge = sclk_sync[1] && !sclk_sync[2];
wire mosi_data = mosi_sync[1];

always @(posedge clk) begin
    sclk_sync <= {sclk_sync[1:0], clk_line}; 
    cs_sync <= {cs_sync[1:0], cs_line};
    mosi_sync <= {mosi_sync[0], mosi_line};
    

    if (cs_sync[1]) begin
        bit_cnt <= 0;
    end
    else if (sclk_posedge) begin
     shift_reg <= {shift_reg[6:0], mosi_data};
     bit_cnt <= bit_cnt + 1;

     if (bit_cnt == 4'd7) begin
        case({shift_reg[1:0], mosi_data}) 
        3'b001: color_store <= 3'b001; // 0x01
        3'b010: color_store <= 3'b010;
        3'b011: color_store <= 3'b011;
        3'b100: color_store <= 3'b100;
        3'b101: color_store <= 3'b101;
        3'b110: color_store <= 3'b110;
        3'b111: color_store <= 3'b111;
        default: color_store <= color_store;
        endcase
    end
    end
end
endmodule