module minimized_module (
    input x0, x1, x2,
    output f
);

    assign f = (~x0 & ~x1 & x2) | (~x0 & x1 & x2) | (x0 & x1 & ~x2) | (x0 & x1 & x2);

endmodule
