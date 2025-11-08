module minimized_module (
    input x0, x1,
    output f
);

    assign f = (~x0 & x1) | (x0 & ~x1) | (x0 & x1);

endmodule
