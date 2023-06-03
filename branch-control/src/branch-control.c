#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include "argparse.h"

#define MAP_SIZE     0x10000
#define MAX_ADDRESS  0x0

union reg0 {
   struct { 
      unsigned int power1v5 : 1;
      unsigned int power3v8 : 1;
      unsigned int clock : 1;
      unsigned int reserved0 : 13;
      unsigned int good1v5 : 1;
      unsigned int good3v8 : 1;
      unsigned int reserved1 : 14;
   } field;

   unsigned int dword;   
};

int main(int argc, const char **argv) {

   const char *power1v5 = NULL;
   const char *power3v8 = NULL;
   bool poll1v5 = false;
   bool poll3v8 = false;
   const char *clock = NULL;
   unsigned int regaddr = -1;
   unsigned int regval = -1; 

   volatile unsigned int *ptr;
   union reg0 r0;          // register 0 from hardware
   union reg0 r0new;       // register 0 to set

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

   ptr = (volatile unsigned int *) mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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
      printf("0x%08X\n", ptr[regaddr]);
      exit(0);
   }

   // read register
   r0.dword = r0new.dword = ptr[0];
   printf("\n");

   if(power1v5 != NULL) {
      if(!strcmp(power1v5, "on") || !strcmp(power1v5, "ON")) {
         printf("User request: Power 1.5V: ON\n");
         r0new.field.power1v5 = 1;
         if(r0.field.power1v5 == 0) // current state: OFF - required state: ON
            poll1v5 = true;
         else
            printf("-- Power 1.5V is already ON\n");
      } else if(!strcmp(power1v5, "off") || !strcmp(power1v5, "OFF")) {
         printf("User request: Power 1.5V: OFF\n");
         r0new.field.power1v5 = 0;
         if(r0.field.power1v5)      // current state: ON - required state: OFF
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
         r0new.field.power3v8 = 1;
         if(r0.field.power3v8 == 0) // current state: OFF - required state: ON
            poll3v8 = true;
         else
            printf("-- Power 3.8V is already ON\n");
      } else if(!strcmp(power3v8, "off") || !strcmp(power3v8, "OFF")) {
         printf("User request: Power 3.8V: OFF\n");
         r0new.field.power3v8 = 0;
         if(r0.field.power3v8)      // current state: ON - required state: OFF
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
         r0new.field.clock = 1; 
         if(r0.field.clock)      // current state: ON - required state: ON
            printf("-- Clock is already ON\n");
      } else if(!strcmp(clock, "off") || !strcmp(clock, "OFF")) {
         printf("User request: Clock: OFF\n");
         r0new.field.clock = 0;
         if(r0.field.clock == 0) // current state: OFF - required state: OFF
            printf("-- Clock is already OFF\n"); 
      } else {
         printf("E: Clock wrong parameter (should be 'on' or 'off')\n");
      }
   }

   // write register
   if(r0new.dword != r0.dword) {

      ptr[0] = r0new.dword;

      if(poll1v5 || poll3v8) {

         printf("\nCheck power status...\n");

         for(int i=0; i<20; i++) {

            r0.dword = ptr[0];

            if(poll1v5) {
               usleep(10000);
               if(r0.field.power1v5) {    // expect status == GOOD
                  if(r0.field.good1v5) {
                     printf("I: power status 1.5V changed to GOOD\n");
                     poll1v5 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if(!r0.field.good1v5) {
                     printf("I: power status 1.5V changed to NOT GOOD\n");
                     poll1v5 = false;
                  }
               }
            }

            if(poll3v8) {
               usleep(10000);
               if(r0.field.power3v8) {    // expect status == GOOD
                  if(r0.field.good3v8) {
                     printf("I: power status 3.8V changed to GOOD\n");
                     poll3v8 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if(!r0.field.good3v8 == 0) {
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

   r0.dword = ptr[0];
   // print status
   printf("\nBranch status\n");
   printf("-------------\n");
   printf("Power 1.5V: ");
   (r0.field.power1v5)?printf("ON\n"):printf("OFF\n");
   printf("Power 3.8V: ");
   (r0.field.power3v8)?printf("ON\n"):printf("OFF\n");
   printf("Clock: ");
   (r0.field.clock)?printf("ON\n"):printf("OFF\n");
   printf("Power status 1.5V: ");
   (r0.field.good1v5)?printf("GOOD\n"):printf("NOT GOOD\n");
   printf("Power status 3.8V: ");
   (r0.field.good3v8)?printf("GOOD\n"):printf("NOT GOOD\n");
   printf("\n");
   
   return 0;
}
