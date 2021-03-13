# CNLab-Ass01
Group 15
## Usage
- ```./traceroute <domain_name>``` or ```./traceroute <ip_address>```
- if use Makefile:
    - ```make```: Compile the .c file
    - ```make test```: execute ```./traceroute localhost```  
    - ```make test2```: execute ```./traceroute www.csie.ntu.edu.tw```
    - ```make clean```: remove the executable file.
## Reference
[DNS Look-up](https://github.com/CyberChimeraUSA/C-Networking/blob/master/C-DNS%20lookup%20using%20getaddrinfo/dnsUpdatedvid.c)
[traceroute implementation](https://stackoverflow.com/questions/15458438/implementing-traceroute-using-icmp-in-c)
[another implementation](https://stackoverflow.com/questions/29344543/simple-icmp-traceroute-implementation-in-c) (this one is closer to our assignment)
## Current problem
- The program can only work when target is localhost, other ip address will always timeout even the nearest router.
## Scoreboard
- Environment:
    - [O] Localhost (10%)
    - [X] Nearest router (10%)
    - [X] No response (10%)
    - [O] Handle DNS lookup (10%)
    - [X] Use TCP and UDP to implement traceroute (10%)
- Report:
    - [X] Environment (Ubuntu 16.04.7 LTS) (5%)
    - [X] How to detect and defend traceroute? (5%)
    - [X] Why tracetroute cannot show the full route? (10%)
    - [X] Why the result may not be always same? (10%)
    - [X] Compare the result with local and foreign, and explain what causes the difference. (10%)
    - [X] Explain the difference by using TCP, UDP and ICMP. (10%)
- Current Score: 20%