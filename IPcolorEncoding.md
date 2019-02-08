# Color Encoding IP Addresses #
This device makes use of color encoded IP addresses, when the device is connected to a WLAN as client.
Besides mDNS this is another way, how the user can get access to the device.

## Octal based encoding ##
The color coding is based on the Octal system, where colors are assigned to the digits 0-7.

| Value | Color | Code |
|-------|-------|------|
|  0    | black |#000000|
|  1    | <span style="color:#F00">red</span> |#FF0000|
|  2    | <span style="color:#FF0;background:#555">yellow </span>|#FFFF00|
|  3    | <span style="color:#0F0">green </span>|#00FF00|
|  4    | <span style="color:#0FF;background:#555">cyan </span>|#00FFFF|
|  5    | <span style="color:#00F"> blue </span>|#0000FF|
|  6    | <span style="color:#F0F">magenta </span>|#FF00FF|
|  7    | <span style="color:#FFF;background:#555"> white |#FFFFFF|

For encoding a IP-address we use a string of seven digits, depending on the address range we have to submit one or two of those strings.  As most WLAN access points are using addresses in the range **192.168.x.x**, we handle this addresses in one string, which makes it in most cases a single string input.
We can't handle a full 32-bit IP-address with 9 LED, as 8^9 (134.217.728) is much smaller than 2^32 (4.294.967.296). Therefore we submit two 16-bit values.
The highest possible 16-bit value **65535**(dez) is **177777**(oct), which means, that we need only six digits.
To make the encoding more stable, we add a seventh digit for direction control.

## Encoding details ##
The first digit can only hold the value **0** or **1**, as the range goes from 000000 to 177777.
That allows us, to put additional information into that digit:
If we encode an IP-address from the 192.168.x subnet, we only encode the last two bytes from that address and thats it.
If we encode an IP-address from another network, we have to send two strings. One for the first two bytes, another for the second two bytes.  To recognize this situation we mark those two strings in the first digit:
We add the value of 2 to the first digit when encoding the two rightmost bytes, and we add the value 4 to the first digit, when sending the two leftmost bytes from the IP-address.
This allows the following association:

|Octal Range| Meaning|
|-----------|--------|
|000000-177777| this are the two rightmost bytes from an 192.168.x network|
|200000-377777| this are the two rightmost bytes from another IP-address,<br> to get the value subtract 200000(oct) from this value|
|400000-577777| this are the two leftmost bytes from another IP-address,<br> to get the value subtract 400000(oct) from this value|

This table shows, that the first digit can only be in the range from 1-5. The values 6 and 7 are not defined, which allows us, to add a seventh digit with the fixed value "**6**" to the octal representation.
The encoded octal codes will therefore look like: 

|Octal Range|
|-----------|
|000000**6**-177777**6**|
|200000**6**-377777**6**|
|400000**6**-577777**6**|

As soon, as we detect a digit of "**6**" at the first position, we know, that we have to reverse the string. This eases the handling, as we can use this encoding also in cases, where a direction of the LEDs is hard to define e.g. LED-rings etc.

### Some Examples ###

|octal Value| decoded IP-address | comment|
|-----------|--------------------|--------|
|0020016 | 192.168.4.1 | as the first digit is on the range of 0-1, its a 192.168 address|
|6100200 | 192.168.4.1 | the same address like before, but entered in reverse|
|5133116 |150.201     | the two leftmost bytes from 150.201.100.90  |
|4256906 | &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;.100.90| the two rightmost bytes from 150.201.100.90  |

The first and last digit can't have the value "**7**".





