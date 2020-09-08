
module kernel (
	clk_clk,
	color_conduit_end_0_in_sq,
	color_conduit_end_0_s_ctrl,
	color_conduit_end_0_get_it,
	reset_reset_n,
	sdram_wire_addr,
	sdram_wire_ba,
	sdram_wire_cas_n,
	sdram_wire_cke,
	sdram_wire_cs_n,
	sdram_wire_dq,
	sdram_wire_dqm,
	sdram_wire_ras_n,
	sdram_wire_we_n,
	uart_external_connection_rxd,
	uart_external_connection_txd);	

	input		clk_clk;
	input		color_conduit_end_0_in_sq;
	output	[1:0]	color_conduit_end_0_s_ctrl;
	output		color_conduit_end_0_get_it;
	input		reset_reset_n;
	output	[12:0]	sdram_wire_addr;
	output	[1:0]	sdram_wire_ba;
	output		sdram_wire_cas_n;
	output		sdram_wire_cke;
	output		sdram_wire_cs_n;
	inout	[31:0]	sdram_wire_dq;
	output	[3:0]	sdram_wire_dqm;
	output		sdram_wire_ras_n;
	output		sdram_wire_we_n;
	input		uart_external_connection_rxd;
	output		uart_external_connection_txd;
endmodule
