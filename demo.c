#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/errno.h>

#define MAJOR_NUM 60
#define DEVICE_NAME "demo"
#define MODULE_NAME "demo"

static int ledPortList[] = {466, 397, 255, 429};
static char userChar[100];


static gpio_export(int gpio){
	char buf[64];
	snprintf(buf,sizeof(buf),"%d",gpio);

    struct file *fp;   
    loff_t pos=0;
    mm_segment_t old_fs;
    old_fs=get_fs();
    set_fs(get_ds());

    fp=filp_open("/sys/class/gpio/export",O_WRONLY,0);

    vfs_write(fp,buf,strlen(buf),&pos);

    filp_close(fp,NULL);
    set_fs(old_fs);
}

static void gpio_unexport(int gpio){
    char buf[64];
	snprintf(buf,sizeof(buf),"%d",gpio);

    struct file *fp;   
    loff_t pos=0;
    mm_segment_t old_fs;
    old_fs=get_fs();
    set_fs(get_ds());

    fp=filp_open("/sys/class/gpio/unexport",O_WRONLY,0);

    vfs_write(fp,buf,strlen(buf),&pos);

    filp_close(fp,NULL);
    set_fs(old_fs);
}

static void gpio_set_dir(int gpio,char* dirStatus){
    char buf[64];
	snprintf(buf,sizeof(buf),"/sys/class/gpio/gpio%d/direction",gpio);

    struct file *fp;   
    loff_t pos=0;
    mm_segment_t old_fs;
    old_fs=get_fs();
    set_fs(get_ds());

    fp=filp_open(buf,O_WRONLY,0);

    if(dirStatus=="out")
		vfs_write(fp,"out",4,&pos);
	else
		vfs_write(fp,"in",3,&pos);

    filp_close(fp,NULL);
    set_fs(old_fs);
}

static void gpio_set_value(int gpio,int value){
    char buf[64];
	snprintf(buf,sizeof(buf),"/sys/class/gpio/gpio%d/value",gpio);

    struct file *fp;   
    loff_t pos=0;
    mm_segment_t old_fs;
    old_fs=get_fs();
    set_fs(get_ds());

    fp=filp_open(buf,O_WRONLY,0);

    if(value==0){
		vfs_write(fp,"0",2,&pos);
		printk("GPIO:%d Status:OFF\n",gpio);
	}
	else{
		vfs_write(fp,"1",2,&pos);
		printk("GPIO:%d Status:ON\n",gpio);
	}

    filp_close(fp,NULL);
    set_fs(old_fs);
}

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Enter Open function\n");

    return 0;
}

static ssize_t dev_read(struct file *filp, char *buf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "Enter Read Function\n");
    printk(KERN_INFO "Enter Read Function\n");
    printk(KERN_INFO "Enter Read Function\n");
    return count;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Enter Release function\n");
    
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Enter Write function\n");
    if (len > sizeof(userChar) - 1) {
        return -EINVAL;
    }

    copy_from_user(userChar, buffer, len);
    userChar[len] = '\0';
    printk("userChar: %s\n", userChar);

    int gpio_num, value;
    sscanf(userChar, "%d %d", &gpio_num, &value);
    if(value==3){
        char readBuf[64];
        snprintf(readBuf,sizeof(readBuf),"/sys/class/gpio/gpio%d/value",gpio_num);
        struct file *fp;   
        loff_t pos=0;
        mm_segment_t old_fs;
        old_fs=get_fs();
        set_fs(get_ds());
        char newBuf[64];
        fp=filp_open(readBuf,O_RDONLY,0);
        vfs_read(fp,newBuf,sizeof(readBuf),&pos);
        printk("GPIO:%d Status:%s\n",gpio_num,newBuf);
        filp_close(fp,NULL);
        set_fs(old_fs);
    }else{
        printk("%d %d",gpio_num, value);
        gpio_set_value(gpio_num,value);
    }

    return len;
}
static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk("device ioctl\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .write = dev_write,
    .read = dev_read,
    .unlocked_ioctl = drv_ioctl,
};

static int __init demo_init(void) {
    printk(KERN_INFO "Initializing the demo LKM\n");
    int result = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    if (result < 0) {
        printk(KERN_ALERT "%s: can't get major %d\n", MODULE_NAME, MAJOR_NUM);
        return result;
    }
    printk(KERN_INFO "%s: started\n", MODULE_NAME);
    gpio_export(ledPortList[0]);
    gpio_export(ledPortList[1]);
    gpio_export(ledPortList[2]);
    gpio_export(ledPortList[3]);
    gpio_set_dir(ledPortList[0],"out");
    gpio_set_dir(ledPortList[1],"out");
    gpio_set_dir(ledPortList[2],"out");
    gpio_set_dir(ledPortList[3],"out");

    return 0;
}

static void __exit demo_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk(KERN_INFO "%s: removed\n", MODULE_NAME);
    gpio_unexport(ledPortList[0]);
    gpio_unexport(ledPortList[1]);
    gpio_unexport(ledPortList[2]);
    gpio_unexport(ledPortList[3]);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me :)");