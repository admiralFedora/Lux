#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>         /* printk() */
#include <linux/slab.h>           /* kmalloc() */
#include <linux/fs.h>             /* everything... */
#include <linux/errno.h>          /* error codes */
#include <linux/types.h>          /* size_t */
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/fcntl.h>          /* O_ACCMODE */
#include <linux/delay.h>
#include <asm/system.h>           /* cli(), *_flags */
#include <asm/uaccess.h>          /* copy_from/to_user */
#include <linux/timer.h>          /* timer in kernel */
#include <linux/ktime.h> 
#include <linux/hrtimer.h>        /* high resolution timer */
#include <linux/jiffies.h>        /* jiffies */
#include <linux/pid.h>            /* find_pid() */
#include <asm/arch/gpio.h>
#include <asm/hardware.h>
#include <asm/arch/i2c.h>
#include <asm/arch/pxa-regs.h>
#include <linux/string.h>
#include <asm-arm/arch/hardware.h>
#include <linux/interrupt.h>

/*
  Comments here will be related to the Adafruit stepper motor (5v Unipolar) used in this project
  Consult your manual if you're using this for something else...
*/


/*
  High-res timers are used if you want to achieve a high RPM for the stepper motor
  However, for the purposes of this project a regular timer would have suffised due to the time being 2ms per step
*/

#define P1	28 // pink wire
#define P2	29 // blue wire
#define P3	30 // orange wire
#define P4	31 // yellow wire

#define FORWARD		1
#define BACKWARD	-1


MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Stepper Module");

static struct hrtimer motor_timer;
static ktime_t k_motor;
static int step;
static int totalSteps;
static unsigned stepsWanted;
static int direction;
static unsigned cap = 256; // how many characters we write into our buffer
static int major = 61;
static char *buffer;
static int count = 0;

static int motor_init(void);
static void motor_exit(void);

static ssize_t motor_write( struct file *filp, const char __user *buff,
                      size_t len, loff_t *f_pos);
static int motor_open(struct inode *inode, struct file *filp);
static int motor_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
static int motor_release(struct inode *inode, struct file *filp);

// timer call back function
static enum hrtimer_restart motor_handler(struct hrtimer *timer);

// stepping function
static void do_step(void);

static struct file_operations motor_fops = {
	write:motor_write,
	open: motor_open,
	release: motor_release,
	read: motor_read
};

static int motor_init(void)
{
  int result;

  buffer = (char *) kmalloc(cap, GFP_KERNEL);
  if(!buffer)
  {
    printk(KERN_ALERT "Motor: Insufficient kernel memory\n");
    result = -ENOMEM;
    goto fail;
  }

  result = register_chrdev(major, "motor", &motor_fops);

  if (result < 0) {
    printk(KERN_INFO "motor: Couldn't create proc entry\n");
	goto fail;
  } else {

    gpio_direction_output(P1,0);
    gpio_direction_output(P2,0);
    gpio_direction_output(P3,0);
    gpio_direction_output(P4,0);

    // making sure our motor is off
    pxa_gpio_set_value(P1,0);
    pxa_gpio_set_value(P2,0);
    pxa_gpio_set_value(P3,0);
    pxa_gpio_set_value(P4,0);

    // initialize our timer, but not starting it
    hrtimer_init(&motor_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    motor_timer.function = &motor_handler;
    
    step = 0;

    // High-res timers require you to set your time as a ktimer variable
    k_motor = ktime_set(0,2*1E6L);

    printk(KERN_INFO "motor: Module installed\n");
  }

fail:
  return result;

}

static void motor_exit(void)
{
    unregister_chrdev(major, "motor");
    hrtimer_cancel(&motor_timer);
    kfree(buffer);
    printk(KERN_INFO "motor: Module removed\n");
}

/*
  Module accepts a command in the following format %c%c%d
  arg1:
    f - forward
    b - backwards
  arg2: 
    g - go
    s - stop
  arg3:
    # - steps to move
*/

static ssize_t motor_write( struct file *filp, const char __user *buff,
                      size_t len, loff_t *f_pos)
{
  memset(buffer, 0, cap);
  if (copy_from_user(buffer, buff, len)){
    return -EFAULT;
  }
  printk(KERN_ALERT"command received: %s\n",buffer);

  switch(buffer[0]){
    case 'f':
      direction = FORWARD;
      break;
    case 'b':
      direction = BACKWARD;
      break;
    default:
      printk(KERN_ALERT "wrong input\n");
      break;
  }
	printk(KERN_ALERT "direction: %d\n",direction);

    switch(buffer[1]){
      case 'g':
        hrtimer_start(&motor_timer,k_motor,HRTIMER_MODE_REL);
        break;
      case 's':
        hrtimer_cancel(&motor_timer);
        pxa_gpio_set_value(P1,0);
        pxa_gpio_set_value(P2,0);
        pxa_gpio_set_value(P3,0);
        pxa_gpio_set_value(P4,0);
        break;
      default:
        printk(KERN_ALERT "wrong input\n");
        break;
    }
  

  stepsWanted = simple_strtoul((buffer+2), NULL, 10);
  printk(KERN_INFO "steps: %u\n", stepsWanted);
  return len;
}


/*
  FYI you absolutely must set all GPIO pins to low after finishing motor movements
  
  Forgetting to set all GPIO pins to 0 after finishing motor movements will result in the motor
  heating up to dangerous levels and possibly cause it to burn out
*/

static enum hrtimer_restart motor_handler(struct hrtimer *timer)
{
  do_step();
  step = (( (step+direction) % 4 ) + 4) % 4;
  totalSteps = (( (totalSteps+direction) % 513) + 513) % 513;
  if(stepsWanted > 0)
  {
    hrtimer_start(&motor_timer,k_motor,HRTIMER_MODE_REL);
	  stepsWanted--;
  }
  else
  {
    pxa_gpio_set_value(P1,0);
    pxa_gpio_set_value(P2,0);
    pxa_gpio_set_value(P3,0);
    pxa_gpio_set_value(P4,0);
  }

  return HRTIMER_NORESTART;

}

/*
  consulted http://www.tigoe.net/pcomp/code/circuits/motors/stepper-motors/
  On how a unipolar stepper should go through sequences

  Slightly modified through testing after realzing that the motor was not moving as efficiently as possible

  Function is essentially a state machine
*/
static void do_step(void)
{
  switch(step){
    case 0:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,0);
      break;
    case 1:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,0);
      break;
    case 2:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,1);
      break;
    case 3:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,1);
      break;
  };
}

/*
  these follow FOPs don't need to do anything
*/
static int motor_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int motor_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int motor_read(struct file *filp,char *buf, size_t count, loff_t *f_pos)
{
	return count;
}



/*
  function defintions here are required for correct compilation
*/
void __aeabi_d2uiz(void){}
void __aeabi_dadd(void){}
void __aeabi_i2d(void){}
void __aeabi_f2d(void){}
void __aeabi_f2uiz(void){}
void __aeabi_fmul(void){}
void __aeabi_fadd(void){}
void __aeabi_i2f(void){}
void __aeabi_dmul(void){}

module_init( motor_init );
module_exit( motor_exit );
