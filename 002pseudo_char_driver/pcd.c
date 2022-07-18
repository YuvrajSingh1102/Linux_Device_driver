#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s :" fmt,__func__
#define DEV_MEM_SIZE 512

//pseudo device memory
char device_buffer[DEV_MEM_SIZE];

/*This hold the device number */
dev_t device_number;

/* Cdev variable */
struct cdev pcd_cdev;

static int number_open = 0;

loff_t pcd_lseek (struct file *filp, loff_t offset, int whence)
{
	loff_t temp;

	pr_info("Lseek requested\n");
	pr_info("Current value of the file position = %lld\n", filp->f_pos);
	
	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
				return -EINVAL;
			filp->f_pos = offset;
			break;
		case SEEK_CUR:
			temp = filp->f_pos + offset;
			if((temp > DEV_MEM_SIZE) || (offset < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		case SEEK_END:
			temp = DEV_MEM_SIZE + offset;
			if((temp > DEV_MEM_SIZE) || (temp < 0))
				return -EINVAL;
			filp->f_pos = temp;
			break;
		default:
			return -EINVAL;
	}
	
	pr_info("New value of the file position = %lld\n", filp->f_pos);
	return filp->f_pos;
}

ssize_t pcd_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Read request for %ld bytes \n",count);
	pr_info("Current file position = %lld\n",*f_pos);
	/* Adjusting the 'count' */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;			//f_pos = current file position
	
	/* copy to user */
	if(copy_to_user(buff,&device_buffer[*f_pos],count)){		//buff = Destination
		return -EFAULT;
	}

	/* update the current file information */
	*f_pos += count;
	
	pr_info("Number of bytes successful read = %ld\n",count);
	pr_info("Updated the file = %lld\n",*f_pos);
	
	/* Return number of bytes which have been successful read */
	return count;
}

ssize_t pcd_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Write requested for %ld bytes \n", count);
	pr_info("Current file position is %lld\n", *f_pos);	

	/* Adjusting the count */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;
	
	if(!count) {
		pr_err("No space left on the device\n");
		return -ENOMEM;
	}

	/* Copy from user */
	if(copy_from_user(&device_buffer[*f_pos], buff, count))
	{
		return -EFAULT;
	}
	
	/* Updating the current file information */
	*f_pos += count;

	pr_info("Number of bytes successful written = %ld\n", count);
	pr_info("Updated the file = %lld\n", *f_pos);

	/* Return number of bytes which have been successful written */
	return (count);
}
int pcd_open (struct inode *inode, struct file *filp)
{
	number_open++;
	pr_info("Open was successful\n");
	pr_info("Device has been opened %d this many times\n", number_open);
	return 0;
}
int pcd_release (struct inode *inode, struct file *filp)
{
	pr_info("Release was successful");
	return 0;
}

/* File operations of the driver */
struct file_operations pcd_fops = 
{
	.open = pcd_open,
	.write = pcd_write,
	.read = pcd_read,
	.llseek = pcd_lseek,
	.release = pcd_release,
	.owner = THIS_MODULE
};

struct class *class_pcd;

struct device *device_pcd;

static int __init pcd_driver_init(void) 
{
	int ret;

	/*1.Dynamically allocate a device number */
	ret = alloc_chrdev_region(&device_number,0,1,"pcd_devices");
			
	if(ret < 0) {
		pr_err("Alloc chardev failed\n");
		goto out;
	}

	/* you can use %s, __fun__ to display the function name from which it is executing or you can define pr_tmt and use that for every print function */
	pr_info("The major and minor device number is: %d:%d\n", MAJOR(device_number), MINOR(device_number));

	/*2.Initialize the cdev structure with fops */
	cdev_init(&pcd_cdev, &pcd_fops);

	/*3.Register a device (cdev structure) with VFS*/
	pcd_cdev.owner = THIS_MODULE;
	ret = cdev_add(&pcd_cdev, device_number, 1);
	
	if(ret < 0) {
		pr_err("cdev add failed\n");
		goto unreg_chrdev;
	}
	/* Create device class under /sys/class/ */
	class_pcd = class_create(THIS_MODULE,"pcd_class");

	if(IS_ERR(class_pcd)){
		pr_err("Class creation is failed.\n");
		ret = PTR_ERR(class_pcd);
		goto cdev_del;
	}
	
	/* populate the sysfs with device information.*/
	device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
	
	if(IS_ERR(device_pcd)) {
		pr_err("Device create failed.\n");
		ret = PTR_ERR(device_pcd);
		goto class_del;
	} 
	pr_info("Module initialization was successful\n");

	return 0;

class_del:
	class_destroy(class_pcd);

cdev_del:
	cdev_del(&pcd_cdev);

unreg_chrdev:
	unregister_chrdev_region(device_number, 1);

out:
	pr_info("Module insertion failed\n");
	return ret;
}

static void __exit pcd_driver_cleanup(void)
{
	device_destroy(class_pcd, device_number);
	class_destroy(class_pcd);
	cdev_del(&pcd_cdev);
	unregister_chrdev_region(device_number, 1);
	pr_info("Module Unloaded\n");
}

module_init(pcd_driver_init);
module_exit(pcd_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("YUVRAJ");
MODULE_DESCRIPTION("A pseudo character driver");
