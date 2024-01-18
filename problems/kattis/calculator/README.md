# Calculator

The problem description can be found [here](https://open.kattis.com/problems/calculator). My solution is a more general
solution using the known way of parsing math expressions using postfix notation, and it accounts for the power operation
(using `^` as the power operator symbol), last value evaluated with `_` (so you can do `1+2`, then `_+3` which will give
you `6`), and supports parsing decimal numbers with the fractional part using `.` (e.g. `1.5^0.5` is supported). 
