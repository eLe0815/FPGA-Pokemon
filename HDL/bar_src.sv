module bar_src (
   input  logic clk,
   input  logic [10:0] x, y,     // treated as x-/y-axis
   output logic [11:0] bar_rgb 
   );

   // signal declaration
   logic [3:0] up, down;
   logic [3:0] r, g, b;
   logic [11:0] reg_d1_reg, reg_d2_reg;
   
   // body
   assign up = x[6:3];
   assign down = ~x[6:3];    // "not" reverse the binary sequence 
   
   always_comb begin
      // cpu status
      if ( (x >= 3 && x <= 240 && y==40)|| 
           (x >= 3 && x <= 240 && y==120) ||
           (y >= 40 && y <= 120 && x==3) ||
           (y >= 40 && y <= 120 && x==240)
          )
      begin
         r = 4'b0000;
         g = 4'b0000;
         b = 4'b0000;
      end   
      // user status
      else if 
         ( (x >= 400 && x <= 640 && y==240)|| 
           (x >= 400 && x <= 640 && y==380) ||
           (y >= 240 && y <= 380 && x==400) ||
           (y >= 240 && y <= 380 && x==640)
          )
      begin
         r = 4'b0000;
         g = 4'b0000;
         b = 4'b0000;
      end 
      else if //frame_p->fillRect(0, 380, 640, 100, 0x000);
         ( (x >= 0 && x <= 640 && y==380)|| 
           (x >= 0 && x <= 640 && y==480) ||
           (y >= 380 && y <= 480 && x==0) ||
           (y >= 380 && y <= 480 && x==640)
          )
      begin
         r = 4'b0000;
         g = 4'b0000;
         b = 4'b0000;
      end  
      else begin   
        r = 4'b1111;
        g = 4'b1111;
        b = 4'b1111;
      end // else
   end // always   
   // output with 2-stage delay line
   always_ff @(posedge clk) begin
      reg_d1_reg <= {r, g, b};
      reg_d2_reg <= reg_d1_reg;
   end
   assign bar_rgb = reg_d2_reg;
endmodule