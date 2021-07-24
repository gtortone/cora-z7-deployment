#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "argparse.h"

#define POWER1V5     (1)
#define POWER3V8     (1<<1)
#define CLOCK        (1<<2)
#define GOOD1V5      (1<<16)
#define GOOD3V8      (1<<17)

#define MAP_SIZE     0x10000
#define MAX_ADDRESS  0x0

int main(int argc, const char **argv) {

   const char *power1v5 = NULL;
   const char *power3v8 = NULL;
   bool poll1v5 = false;
   bool poll3v8 = false;
   const char *clock = NULL;
   int regaddr = -1;
   int regval = -1; 

   volatile int *ptr;
   int regval0 = 0;

   static const char *const usage[] = {
      "lc [options]",
      NULL,
   };

   struct argparse_option options[] = {
      OPT_HELP(),
      OPT_GROUP("Power options"),
      OPT_STRING(0, "1V5", &power1v5, "on/off 1.5V power line"),
      OPT_STRING(0, "3V8", &power3v8, "on/off 3.8V power line"),
      OPT_GROUP("Clock options"),
      OPT_STRING(0, "clock", &clock, "on/off clock line"),
      OPT_GROUP("Register options"),
      OPT_INTEGER('a', "address", &regaddr, "register address to read/write"),
      OPT_INTEGER('v', "value", &regval, "register value to write"),
      OPT_END(),
   };   

   struct argparse argparse;
   argparse_init(&argparse, options, usage, 0);
   argparse_describe(&argparse, "\nBranch control", "\nDefine AXICTRL_UIO_ID environment variable to specify UIO device file id (default: 0 => /dev/uio0)\n\n");
   argparse_parse(&argparse, argc, argv);

   // init UIO device
   const char *uioid = getenv("AXICTRL_UIO_ID");
   char uiodev[16] = "/dev/uio";
   if(uioid != NULL)
      uiodev[strlen(uiodev)] = uioid[0];
   else
      uiodev[strlen(uiodev)] = '0';
      
   int fd = open(uiodev, O_RDWR);
   if(fd < 1) {
      printf("E: failed to open UIO device (%s)\n", uiodev);
      exit(-1);
   }

   ptr = (volatile int *) mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   // check if raw register access is required
   if(regaddr != -1) {
      if(regaddr > MAX_ADDRESS) {
         printf("E: register address out of range (max address: %d)\n", MAX_ADDRESS);
         exit(-1);
      }
      if(regval != -1) {
         // write register
         ptr[regaddr] = regval;
      }
      printf("0x%X\n", ptr[regaddr]);
      exit(0);
   }

   // read register
   regval0 = ptr[0];
   printf("\n");

   if(power1v5 != NULL) {
      if(!strcmp(power1v5, "on") || !strcmp(power1v5, "ON")) {
         printf("User request: Power 1.5V: ON\n");
         regval0 |= POWER1V5; 
         if((ptr[0] & POWER1V5) == 0)     // current state: OFF - required state: ON
            poll1v5 = true;
         else
            printf("-- Power 1.5V is already ON\n");
      } else if(!strcmp(power1v5, "off") || !strcmp(power1v5, "OFF")) {
         printf("User request: Power 1.5V: OFF\n");
         regval0 &= ~POWER1V5;
         if(ptr[0] & POWER1V5)            // current state: ON - required state: OFF
            poll1v5 = true;
         else
            printf("-- Power 1.5V is already OFF\n");
      } else {
         printf("E: Power 1.5V wrong parameter (should be 'on' or 'off')\n");
      }
   }

   if(power3v8 != NULL) {      
      if(!strcmp(power3v8, "on") || !strcmp(power3v8, "ON")) {
         printf("User request: Power 3.8V: ON\n");
         regval0 |= POWER3V8;
         if((ptr[0] & POWER3V8) == 0)     // current state: OFF - required state: ON
            poll3v8 = true;
         else
            printf("-- Power 3.8V is already ON\n");
      } else if(!strcmp(power3v8, "off") || !strcmp(power3v8, "OFF")) {
         printf("User request: Power 3.8V: OFF\n");
         regval0 &= ~POWER3V8;
         if(ptr[0] & POWER3V8)            // current state: ON - required state: OFF
            poll3v8 = true;
         else
            printf("-- Power 3.8V is already OFF\n");
      } else {
         printf("E: Power 3.8V wrong parameter (should be 'on' or 'off')\n");
      }
   }

   if(clock != NULL) {
      if(!strcmp(clock, "on") || !strcmp(clock, "ON")) {
         printf("User request: Clock: ON\n");
         regval0 |= CLOCK;
         if((ptr[0] & CLOCK))             // current state: ON - required state: ON
            printf("-- Clock is already ON\n");
      } else if(!strcmp(clock, "off") || !strcmp(clock, "OFF")) {
         printf("User request: Clock: OFF\n");
         regval0 &= ~CLOCK;
         if((ptr[0] & CLOCK) == 0)        // current state: OFF - required state: OFF
            printf("-- Clock is already OFF\n"); 
      } else {
         printf("E: Clock wrong parameter (should be 'on' or 'off')\n");
      }
   }

   // write register
   if(regval0 != ptr[0]) {

      ptr[0] = regval0;

      if(poll1v5 || poll3v8) {

         printf("\nCheck power status...\n");

         for(int i=0; i<20; i++) {
            if(poll1v5) {
               usleep(10000);
               if(ptr[0] & POWER1V5) {    // expect status == GOOD
                  if(ptr[0] & GOOD1V5) {
                     printf("I: power status 1.5V changed to GOOD\n");
                     poll1v5 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if((ptr[0] & GOOD1V5) == 0) {
                     printf("I: power status 1.5V changed to NOT GOOD\n");
                     poll1v5 = false;
                  }
               }
            }

            if(poll3v8) {
               usleep(10000);
               if(ptr[0] & POWER3V8) {    // expect status == GOOD
                  if(ptr[0] & GOOD3V8) {
                     printf("I: power status 3.8V changed to GOOD\n");
                     poll3v8 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if((ptr[0] & GOOD3V8) == 0) {
                     printf("I: power status 3.8V changed to NOT GOOD\n");
                     poll3v8 = false;
                  }
               }
            }
            
         } // end for
      }
   }  

   if(poll1v5) {
      printf("ERROR: power status 1.5V NOT changed !!!\n");
   }

   if(poll3v8) {
      printf("ERROR: power status 3.8V NOT changed !!!\n");
   }

   regval0 = ptr[0];
   // print status
   printf("\nBranch status\n");
   printf("-------------\n");
   printf("Power 1.5V: ");
   (regval0 & POWER1V5)?printf("ON\n"):printf("OFF\n");
   printf("Power 3.8V: ");
   (regval0 & POWER3V8)?printf("ON\n"):printf("OFF\n");
   printf("Clock: ");
   (regval0 & CLOCK)?printf("ON\n"):printf("OFF\n");
   printf("Power status 1.5V: ");
   (regval0 & GOOD1V5)?printf("GOOD\n"):printf("NOT GOOD\n");
   printf("Power status 3.8V: ");
   (regval0 & GOOD3V8)?printf("GOOD\n"):printf("NOT GOOD\n");
   printf("\n");
   
   return 0;
}
