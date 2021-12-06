/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 57-5 */

/* ud_ucase.h

   Header file for ud_ucase_sv.c and ud_ucase_cl.c.

   These programs employ sockets in /tmp. This makes it easy to compile
   and run the programs. However, for a security reasons, a real-world
   application should never create sensitive files in /tmp. (As a simple of
   example of the kind of security problems that can result, a malicious
   user could create a file using the name defined in SV_SOCK_PATH, and
   thereby cause a denial of service attack against this application.
   See Section 38.7 of "The Linux Programming Interface" for more details
   on this subject.)
*/
#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 10 /* Maximum size of messages exchanged \
                       between client and server */

#define SV_SOCK_PATH "/tmp/ud_ucase"

#define MAX_ITEMS 25

struct
{
   int io_number;
   int period;
   int last_toggle;
} typedef Rpi_pin;

int is_duplicate(Rpi_pin *p, Rpi_pin a, int *num_items)
{
   int i;
   for (i = 0; i < *num_items; i++)
   {
      if (p[i].io_number == a.io_number)
      {
         return i;
      }
   }
   return -1;
}

void add_item(Rpi_pin *p, Rpi_pin a, int *num_items)
{
   if (*num_items < MAX_ITEMS)
   {
      p[*num_items] = a;
      *num_items += 1;
   }
}

void delete_item(Rpi_pin *p, int *num_items, int item)
{
   if (*num_items > 0 && item < *num_items && item > -1)
   {
      int last_index = *num_items - 1;
      for (int i = item; i < last_index; i++)
      {
         p[i] = p[i + 1];
      }
      *num_items -= 1;
   }
}

void print_struct(Rpi_pin *p, int *num_items)
{
   if (*num_items > 0)
   {
      int last_index = *num_items - 1;
      for (int i = 0; i <= last_index; i++)
      {
         printf("Item: %d, Pin nummer: %d, Periode: %d \n", i, p[i].io_number, p[i].period);
      }
      printf("\n");
   }
}
