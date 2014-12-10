#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>



MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A file for recording lux values to be read appropriately by the GUI.");

static struct proc_dir_entry *proc_entry;

static char *value; 

static int luxVal_init( void );
static void luxVal_exit( void );

static ssize_t luxVal_write( struct file *filp, const char __user *buff,
                        unsigned long len, void *data);
static int luxVal_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data);


// function for allocating space for our variables
// initializing the proc file
static int luxVal_init( void )
{

  int ret = 0;

  value = (char *)vmalloc( sizeof(char)*10 );

  if (!value) {
    ret = -ENOMEM;
  } else {

    memset( value, 0,sizeof(char)*10 );

    proc_entry = create_proc_entry( "luxVal", 0644, NULL );

    if (proc_entry == NULL) {

      ret = -ENOMEM;
      vfree(value);
      printk(KERN_INFO "luxVal: Couldn't create proc entry\n");

    } else {

      proc_entry->read_proc = luxVal_read;
      proc_entry->write_proc = luxVal_write;
      proc_entry->owner = THIS_MODULE;
      printk(KERN_INFO "luxVal: Module loaded.\n");

    }

  }

  return ret;
}

// cleaning up after ourselves
static void luxVal_exit( void )
{
  remove_proc_entry("luxVal", &proc_root);
  vfree(value);
  printk(KERN_INFO "luxVal: Module unloaded.\n");
}


// write function simply copies the value written to it to our value variable
static ssize_t luxVal_write( struct file *filp, const char __user *buff,
                        unsigned long len, void *data )
{

  if (copy_from_user( value, buff, len )) {
    return -EFAULT;
  }


  return len;

}

// upon read we simply return the value stored in our 'value' variable
static int luxVal_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data )
{
  int len;

  if (off > 0) {
    *eof = 1;
    return 0;
  }


  len = sprintf(page, "%s\n", value);

  return len;

}

module_init( luxVal_init );
module_exit( luxVal_exit );
