# D1 mini: Display of text received via RS232 on OLED 0.66"
Sketch: D1_oop89_serial_screen13.ino, Version 2021-02-28   
[--> Deutsche Version](./LIESMICH.md "Deutsche Version")   

A text received via the serial interface is displayed on a 0.66" OLED display (baud rate 115200Bd) and - similar to the display on the OLED - sent back via the serial interface.   
For the display `Screen13` from the class `Screen1` is used.  This prefabricated screen has a double sized first line with 5 characters and three further lines for 10 characters each. In order for the text to be displayed in the correct line, the individual lines must be separated by a double underscore character (`__`). If a line text is longer than 10 characters, it is continued in the next line.   
Furthermore, the built-in blue LED flashes at 1Hz to indicate that the D1 mini is running.   

__*Examples:*__   
`__` clears the display and gives the following output on the serial interface   
```
  |----------
1 |
```
-----   
`123__**Line 2**__ Line 3__ Line 4` gives the output   
```
  |----------
1 |123
2 |**Line 2**
3 |Line 3
4 | Line 4
```
-----   
`TITLE__Long text over several lines....__-x-x-` gives the output   
```
TITLE
Long text 
-x-x-x-
al lines..
```
which means that the original content of the 3rd line ("over sever") is overwritten. The output via the serial interface is:   
```
  |----------
1 |TITLE
2 |Long text 
3 |over sever
4 |al lines..
3 |-x-x-
```   


