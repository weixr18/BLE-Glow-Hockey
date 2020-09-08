module CPPS(
	csi_CLK,
	csi_reset_n,
	avs_chipselect,
	avs_address,
	avs_read,
	avs_readdata,
	avs_write,
	avs_writedata,
	
	coe_inSQ,
	coe_Sctrl,
	coe_getit
);

input						csi_CLK;
input						csi_reset_n;
input		[4:0]			avs_address;
input						coe_inSQ;
input						avs_chipselect;
input						avs_read;
input						avs_write;
input		[31:0]		avs_writedata;
output reg	[31:0]	avs_readdata;
output reg	[1:0]		coe_Sctrl;	//		S3 S2
										// R	0	0
										// G	1	1
										// B	1	0

reg		[17:0]		clkCNT;
reg		[31:0]		resN;
reg						oCLK;
reg						fCLK;
reg						ready;
output reg				coe_getit;

parameter PRE = 3'b110;
parameter RED = 3'b000;
parameter R2G = 3'b001;
parameter GREEN = 3'b010;
parameter G2B = 3'b011;
parameter BLUE = 3'b100;
parameter FIN = 3'b101;

// stay in every state for 0.010485s
// Adjustable
parameter WHITE_R = 765;	// 62kHz
parameter WHITE_G = 765;	// 70kHz
parameter WHITE_B = 765;	// 91kHz

reg	[2:0]		currentStage = PRE;
reg 	[2:0]		nextStage = RED;

reg	[12:0]	redCNT = 0;
reg	[12:0]	greenCNT = 0;
reg	[12:0]	blueCNT = 0;
reg	[7:0]		Rvalue = 0;
reg	[7:0]		Gvalue = 0;
reg	[7:0]		Bvalue = 0;


always @(posedge csi_CLK)
begin
	clkCNT <= clkCNT+1'b1; 
end

always @(posedge clkCNT[17])
begin
	oCLK <= ~oCLK;
end

always @(posedge clkCNT[7])
begin
	fCLK <= ~fCLK;
end

always @(posedge oCLK)
begin
	currentStage = nextStage;
	case (currentStage)
		PRE:	nextStage = RED;
		RED: nextStage = R2G;
		R2G: nextStage = GREEN;
		GREEN: nextStage = G2B;
		G2B: nextStage = BLUE;
		BLUE: nextStage = FIN;
		FIN: nextStage = PRE;
	endcase
end

always @(posedge coe_inSQ)
begin
	case (currentStage)
		PRE:
		begin
			ready = 0;
			redCNT = 0;
			greenCNT = 0;
			blueCNT = 0;
			coe_Sctrl = 2'b00;
		end
		RED: redCNT = redCNT+1'b1;
		R2G: coe_Sctrl = 2'b11;
		GREEN: greenCNT = greenCNT+1'b1;
		G2B: coe_Sctrl = 2'b10;
		BLUE: blueCNT = blueCNT+1'b1;
		FIN: 
		begin
			if(redCNT > WHITE_R)
			begin
				Rvalue = 255;
			end
			else
			begin
				//Rvalue = 255*redCNT/WHITE_R;
				Rvalue = redCNT/3;
			end
			if(greenCNT > WHITE_G)
			begin
				Gvalue = 255;
			end
			else
			begin
				//Gvalue = 255*greenCNT/WHITE_G;
				Gvalue = greenCNT/3;
			end
			if(blueCNT > WHITE_B)
			begin
				Bvalue = 255;
			end
			else
			begin
				//Bvalue = 255*blueCNT/WHITE_B;
				Bvalue = blueCNT/3;
			end
			if(Bvalue<5 || Gvalue<5 || Rvalue<5)
				ready = 0;
			else
				ready = 1;	
		end
		default;
	endcase
end


always @(posedge fCLK)
begin
	if(ready == 1 )//&& !csi_reset_n)
	begin
		resN[31] = 1;
		resN[30:24] = 7'b0;
		resN[23:16] = Rvalue;
		resN[15:8] = Gvalue;
		resN[7:0] = Bvalue;
	end
	else
	begin
		resN = 32'b0;
	end
	//if((avs_chipselect == 1) && (avs_read == 1))
	//begin
	//avs_readdata = resN;
	//end
	//coe_getit = avs_readdata[31];
end

always @(posedge fCLK)
begin
	avs_readdata = resN;
	coe_getit = avs_readdata[31];
end

endmodule