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

#define  MAP_SIZE    0x10000

typedef struct {
   uint32_t reg0;
} ctrl_t;

int main(int argc, const char **argv) {

   const char *power1v5 = NULL;
   const char *power3v8 = NULL;
   bool poll1v5 = false;
   bool poll3v8 = false;
   const char *clock = NULL;

   volatile ctrl_t *ptr;
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
      OPT_END(),
   };   

   struct argparse argparse;
   argparse_init(&argparse, options, usage, 0);
   argparse_describe(&argparse, "\nBranch control", "");
   argparse_parse(&argparse, argc, argv);

   // init UIO device
   int fd = open("/dev/uio0", O_RDWR);
   if(fd < 1) {
      printf("E: failed to open UIO device\n");
      exit(-1);
   }

   ptr = (volatile ctrl_t *) mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

   // read register
   regval0 = ptr->reg0;
   printf("\n");

   if(power1v5 != NULL) {
      if(!strcmp(power1v5, "on") || !strcmp(power1v5, "ON")) {
         printf("User request: Power 1.5V: ON\n");
         regval0 |= POWER1V5; 
         if((ptr->reg0 & POWER1V5) == 0)     // current state: OFF - required state: ON
            poll1v5 = true;
         else
            printf("-- Power 1.5V is already ON\n");
      } else if(!strcmp(power1v5, "off") || !strcmp(power1v5, "OFF")) {
         printf("User request: Power 1.5V: OFF\n");
         regval0 &= ~POWER1V5;
         if(ptr->reg0 & POWER1V5)            // current state: ON - required state: OFF
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
         if((ptr->reg0 & POWER3V8) == 0)     // current state: OFF - required state: ON
            poll3v8 = true;
         else
            printf("-- Power 3.8V is already ON\n");
      } else if(!strcmp(power3v8, "off") || !strcmp(power3v8, "OFF")) {
         printf("User request: Power 3.8V: OFF\n");
         regval0 &= ~POWER3V8;
         if(ptr->reg0 & POWER3V8)            // current state: ON - required state: OFF
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
         if((ptr->reg0 & CLOCK))             // current state: ON - required state: ON
            printf("-- Clock is already ON\n");
      } else if(!strcmp(clock, "off") || !strcmp(clock, "OFF")) {
         printf("User request: Clock: OFF\n");
         regval0 &= ~CLOCK;
         if((ptr->reg0 & CLOCK) == 0)        // current state: OFF - required state: OFF
            printf("-- Clock is already OFF\n"); 
      } else {
         printf("E: Clock wrong parameter (should be 'on' or 'off')\n");
      }
   }

   // write register
   if(regval0 != ptr->reg0) {

      ptr->reg0 = regval0;

      if(poll1v5 || poll3v8) {

         printf("\nCheck power status...\n");

         for(int i=0; i<20; i++) {
            if(poll1v5) {
               usleep(10000);
               if(ptr->reg0 & POWER1V5) {    // expect status == GOOD
                  if(ptr->reg0 & GOOD1V5) {
                     printf("I: power status 1.5V changed to GOOD\n");
                     poll1v5 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if((ptr->reg0 & GOOD1V5) == 0) {
                     printf("I: power status 1.5V changed to NOT GOOD\n");
                     poll1v5 = false;
                  }
               }
            }

            if(poll3v8) {
               usleep(10000);
               if(ptr->reg0 & POWER3V8) {    // expect status == GOOD
                  if(ptr->reg0 & GOOD3V8) {
                     printf("I: power status 3.8V changed to GOOD\n");
                     poll3v8 = false;
                  }
               } else {                      // expect status == NOT GOOD
                  if((ptr->reg0 & GOOD3V8) == 0) {
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

   regval0 = ptr->reg0;
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
