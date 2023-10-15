/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/slab.h>
#include "aesdchar.h"

#define BUF_SIZE  (1024 * 1024)

int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("hyunjin-chung-woowahan"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    PDEBUG("open");
    /**
     * TODO: handle open
     */
    filp->private_data = container_of(inode->i_cdev, struct aesd_dev, cdev);
    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */

    if (mutex_lock_interruptible(&aesd_device.lock))
        return -ERESTARTSYS;

    do {
        uint8_t index;
    struct aesd_buffer_entry *entry;
        size_t offset_in_buf, copy_size;
        struct aesd_buffer_entry *pbuf =
          aesd_circular_buffer_find_entry_offset_for_fpos(&aesd_device.circular_buf, *f_pos, &offset_in_buf);

        AESD_CIRCULAR_BUFFER_FOREACH(entry,&aesd_device.circular_buf,index) {
          PDEBUG("%zu", entry->size);}
        PDEBUG("aesd_buffer_entry=%p", pbuf);

        if (!pbuf)
            goto out;

        copy_size = ((retval + pbuf->size - offset_in_buf) <= count) ?
          (pbuf->size - offset_in_buf) : (count - retval);


        if (copy_to_user(buf + retval, pbuf->buffptr + offset_in_buf, copy_size))
        {
            retval = -EFAULT;
            goto out;
        }

        retval += copy_size;
        *f_pos += copy_size;
    } while (retval < count);

out:
    mutex_unlock(&aesd_device.lock);
    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    char* ker_buf;
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    if (mutex_lock_interruptible(&aesd_device.lock))
        return -ERESTARTSYS;

    if (!aesd_device.cur_buf.buffptr)
    {
        aesd_device.cur_buf.buffptr = kmalloc(BUF_SIZE, GFP_KERNEL);
        if (!aesd_device.cur_buf.buffptr){
            goto out;
        }
        //memset(aesd_device.cur_buf.buffptr, 0, BUF_SIZE);
    }

    if ((aesd_device.cur_buf.size + count) > BUF_SIZE)
        goto out;

    ker_buf = (char *)aesd_device.cur_buf.buffptr + aesd_device.cur_buf.size;

    if (copy_from_user(ker_buf, buf, count)) {
        retval = -EFAULT;
        goto out;
    }

    aesd_device.cur_buf.size += count;

    if (ker_buf[count - 1] == '\n') {
        if (aesd_device.circular_buf.entry[aesd_device.circular_buf.in_offs].buffptr)
            kfree(aesd_device.circular_buf.entry[aesd_device.circular_buf.in_offs].buffptr);
        aesd_circular_buffer_add_entry(&aesd_device.circular_buf, &aesd_device.cur_buf);
        aesd_device.cur_buf.buffptr = NULL;
        aesd_device.cur_buf.size = 0;
        PDEBUG("add entry");
    }

    retval = count;

out:
    mutex_unlock(&aesd_device.lock);
    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}



int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device, 0, sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */
    aesd_circular_buffer_init(&aesd_device.circular_buf);
    mutex_init(&aesd_device.lock);
    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    uint8_t index;
    struct aesd_buffer_entry *entry;
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */
    AESD_CIRCULAR_BUFFER_FOREACH(entry,&aesd_device.circular_buf,index) {
        if (entry->buffptr)
            kfree(entry->buffptr);
    }

    unregister_chrdev_region(devno, 1);
}

module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
