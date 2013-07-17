
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringSerial.h>

int main ()
{
  int fd ;
  int rec;
  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

// Loop, getting and printing characters

  for (;;)
  {
   // putchar (serialGetchar (fd)) ;
    rec=serialGetchar(fd);
    if (rec=='x')
    {
     rec=serialGetchar(fd);
       if (rec=='s')
         {
            rec=serialGetchar(fd);
               if (rec=='l')
              {
               return 1;
               }
          }
    }
  }
  return 1;
}
