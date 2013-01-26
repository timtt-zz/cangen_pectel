#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int gen_frame_00 (struct can_frame *frame, int s1, int s2, int s3, int s4);
int gen_frame_08 (struct can_frame *frame, int tpsa, int tpsb, int pps, int fbwTargetTps);
int gen_frame_10 (struct can_frame *frame, int act, int ect, int eot, int fl);
int gen_frame_18 (struct can_frame *frame, int tex1, int tex2, int tex3, int aat);
int gen_frame_20 (struct can_frame *frame, int ecut, int spt1, int spt2, int spt3);

int gen_frame_70 (struct can_frame *frame, int rpm, int map, int engineStatus, int car_speed);
int dec_to_hex_pair (int i, char *o0, char *o1);

int main(int argc, char **argv)
{
  int s;
  struct sockaddr_can addr;
  struct ifreq ifr;

  s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

  strcpy(ifr.ifr_name, "vcan0" );
  ioctl(s, SIOCGIFINDEX, &ifr);

  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;

  bind(s, (struct sockaddr *)&addr, sizeof(addr));

  srand((unsigned)time(NULL));

  struct can_frame frame;
  int nbytes;
  /*
  int r = rand()%300;
  gen_frame_00 (&frame, 200, 201, 700, r);
  nbytes = write(s, &frame, sizeof(struct can_frame));

  r = rand()%5;
  gen_frame_08 (&frame, 30, 31, 45, r);
  nbytes = write(s, &frame, sizeof(struct can_frame));

  gen_frame_10 (&frame, 26, 85+r, 95+r, 66);
  nbytes = write(s, &frame, sizeof(struct can_frame));

  gen_frame_18 (&frame, 42, 43, 44, 28);
  nbytes = write(s, &frame, sizeof(struct can_frame));

  gen_frame_20 (&frame, 37, 0, 0, 0);
  nbytes = write(s, &frame, sizeof(struct can_frame));

  gen_frame_70 (&frame, 4239, 50, 0, 198);
  nbytes = write(s, &frame, sizeof(struct can_frame));
  */

  int rpm = 1000;
  int speed = 0;
  int gear = 1;
  unsigned int i=0;
    
  while (speed < 300) {

	struct can_frame frame;
  	int r;

  	// Frame 00
    gen_frame_00 (&frame, speed+1, speed+2, speed+3, speed+4);
    nbytes = write(s, &frame, sizeof(struct can_frame));
    //printf("(%u 00 %u) ", i, nbytes);

    r = rand()%5;

    // Frame 08
    if (speed%10 == 1) {
      gen_frame_08 (&frame, 31, 31, 86, r);
      nbytes = write(s, &frame, sizeof(struct can_frame));
      //printf("(%u 00 %u) ", i, nbytes);
    }

  	// Frame 10
    if ((speed+2)%10 == 1) {
        gen_frame_10 (&frame, 27, 72+r, 110-r, 42);
        nbytes = write(s, &frame, sizeof(struct can_frame));
        //printf("(%u 10 %u) ", i, nbytes);
    }

  	// Frame 70
    gen_frame_70 (&frame, rpm, 50, 0, speed);
    nbytes = write(s, &frame, sizeof(struct can_frame));
    //printf("(%u 70 %u) ", i, nbytes);

    rpm += (25 / gear);
    if (i%10 == 1)
      speed += 1;

    if (rpm > 8700) {
      rpm = 3500;
      gear += 1;
    }

    if (speed == 300) {
      speed = 0;
      rpm = 1000;
      gear = 1;
    }

    i += 1;
    //usleep (1000000); // sleep for 100 ms for 1Hz rate
    //usleep (100000); // sleep for 10 ms for 10Hz rate
    usleep (10000); // sleep for 1 ms for 100Hz rate
  }

  //printf("%i\n", nbytes);

  return 0;
}


/*
 * fl_speed: Front Left Wheel Speed, (X * 0.06) [kph]
 * fr_speed: Front Right Wheel Speed, (X * 0.06) [kph]
 * rl_speed: Rear Left Wheel Speed, (X * 0.06) [kph]
 * rr_speed: Rear Right Wheel Speed, (X * 0.06) [kph]
 */
int gen_frame_00 (struct can_frame *frame, int fl_speed, int fr_speed, int rl_speed, int rr_speed)
{
  frame->can_id = 0x00;
  frame->can_dlc = 8;

  dec_to_hex_pair((fl_speed/0.06), (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  //printf("%x %x\n", frame->data[0], frame->data[1]);
  dec_to_hex_pair((fr_speed/0.06), (char*) &frame->data[2], (char*) &frame->data[3]);
  //printf("%x %x\n", frame->data[2], frame->data[3]);
  dec_to_hex_pair((rl_speed/0.06), (char*) &frame->data[4], (char*) &frame->data[5]);
  //printf("%x %x\n", frame->data[4], frame->data[5]);
  dec_to_hex_pair((rr_speed/0.06), (char *) &frame->data[6], (char*) &frame->data[7]);
  //printf("%x %x\n", frame->data[6], frame->data[7]);

  return 0;
}


/*
 * tpsa: Throttle Position A, (X / 10) deg
 * tpsb: Throttle Position B, (X / 10) deg
 * pps: Pedal Position, (X / 10) %
 * fbwTargetTps: FBW Throttle Dmd, (X / 81.92) deg
 */
int gen_frame_08 (struct can_frame *frame, int tpsa, int tpsb, int pps, int fbwTargetTps)
{
  frame->can_id = 0x08;
  frame->can_dlc = 8;

  dec_to_hex_pair((tpsa*10), (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  dec_to_hex_pair((tpsb*10), (char*) &frame->data[2], (char*) &frame->data[3]);
  dec_to_hex_pair((pps*10), (char*) &frame->data[4], (char*) &frame->data[5]);
  dec_to_hex_pair((fbwTargetTps*81.92), (char *) &frame->data[6], (char*) &frame->data[7]);

  return 0;
}


/*
 * act: Air Temperature, (X / 10) - 100 deg C
 * ect: Engine Coolant Temperature, (X / 10) - 100 deg C
 * eot: Engine Oil Temperature, (X / 10) - 100 deg C
 * fl: Fuel Level, (X / 10) %
 */
int gen_frame_10 (struct can_frame *frame, int act, int ect, int eot, int fl)
{
  frame->can_id = 0x10;
  frame->can_dlc = 8;

  dec_to_hex_pair((act+100)*10, (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  dec_to_hex_pair((ect+100)*10, (char*) &frame->data[2], (char*) &frame->data[3]);
  dec_to_hex_pair((eot+100)*10, (char*) &frame->data[4], (char*) &frame->data[5]);
  dec_to_hex_pair(fl*10, (char *) &frame->data[6], (char*) &frame->data[7]);

  return 0;
}


/*
 * tex1: Thermocouple 1 Temperature, (X / 10) - 100 deg C
 * tex2: Thermocouple 2 Temperature, (X / 10) - 100 deg C
 * tex3: Thermocouple 3 Temperature, (X / 10) - 100 deg C
 * aat: Ambient Temperature, (X / 10) - 100 deg C
 */
int gen_frame_18 (struct can_frame *frame, int tex1, int tex2, int tex3, int aat)
{
  frame->can_id = 0x18;
  frame->can_dlc = 8;

  dec_to_hex_pair((tex1+100)*10, (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  dec_to_hex_pair((tex2+100)*10, (char*) &frame->data[2], (char*) &frame->data[3]);
  dec_to_hex_pair((tex3+100)*10, (char*) &frame->data[4], (char*) &frame->data[5]);
  dec_to_hex_pair((aat+100)*10, (char *) &frame->data[6], (char*) &frame->data[7]);

  return 0;
}


/*
 * ecut: ECU Temperature, (X / 10) - 100 deg C
 * spt1: Spare Temperature 1, (X / 10) - 100 deg C
 * spt2: Spare Temperature 2, (X / 10) - 100 deg C
 * spt3: Spare Temperature 3, (X / 10) - 100 deg C
 */
int gen_frame_20 (struct can_frame *frame, int ecut, int spt1, int spt2, int spt3)
{
  frame->can_id = 0x20;
  frame->can_dlc = 8;

  dec_to_hex_pair((ecut+100)*10, (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  dec_to_hex_pair((spt1+100)*10, (char*) &frame->data[2], (char*) &frame->data[3]);
  dec_to_hex_pair((spt2+100)*10, (char*) &frame->data[4], (char*) &frame->data[5]);
  dec_to_hex_pair((spt3+100)*10, (char *) &frame->data[6], (char*) &frame->data[7]);

  return 0;
}


/*
 * rpm: Engine Speed
 * map: Manifold Absolute Pressure
 * engineStatus: Engine Status Bitfield, bits
 * car_speed: Car Speed, (X * 0.06) kph
 */
int gen_frame_70 (struct can_frame *frame, int rpm, int map, int engineStatus, int car_speed)
{
  frame->can_id = 0x70;
  frame->can_dlc = 8;

  dec_to_hex_pair(rpm, (char*) &(frame->data[0]), (char*) &(frame->data[1]));
  dec_to_hex_pair(map, (char*) &frame->data[2], (char*) &frame->data[3]);
  dec_to_hex_pair(engineStatus, (char*) &frame->data[4], (char*) &frame->data[5]);
  dec_to_hex_pair((car_speed/0.06), (char *) &frame->data[6], (char*) &frame->data[7]);

  return 0;
}


int dec_to_hex_pair (int i, char *o0, char *o1)
{
  char hx[5];
  char *hxp = hx;

  sprintf(hx, "%x", i);

  if (strlen(hx) == 4) {
    sscanf(hxp, "%2x", (unsigned int*)o0);
    hxp += 2;
  }
  else if (strlen(hx) == 3) {
    sscanf(hxp, "%1x", (unsigned int*)o0);
    hxp += 1;
  }

  sscanf(hxp, "%2x", (unsigned int*)o1);

  //printf("i=%i hex= %2x %2x\n", i,  *o0,  *o1);

  return 0;
}
