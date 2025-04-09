#include <pthread.h>
#include <stdio.h>
#include "message_queue.h"



int main()
{
message_queue q;
init_queue(&q);

printf("Message queue created. Press:\n");
printf("  'p' - create producer\n");
printf("  'c' - create consumer\n");
printf("  'q' - quit\n");
char command;
while (1) {
command = getchar();
if (command == '\n') continue;
if (command == 'p') {}
else if (command == 'c') {} 
else if (command == 'q') {break;}
    }
return  0;
}